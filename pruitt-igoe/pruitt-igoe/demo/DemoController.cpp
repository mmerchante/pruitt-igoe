#include "DemoController.h"
#include "Terrain.h"
#include <random>

void DemoController::Awake()
{
	glm::vec2 screenSize = glm::vec2(Engine::GetScreenSize());
	RenderTexture * raymarchingTarget = new RenderTexture(screenSize.x, screenSize.y, true, TextureParameters(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP));
	//raymarchingTarget->AddDrawBuffer(GL_DEPTH_ATTACHMENT);
	raymarchingTarget->Load();

	Texture * randomTexture = BuildRandomTexture();

	GameObject * raymarchingCamera = GameObject::Instantiate("raymarchingCamera");
	this->cameraController = raymarchingCamera->AddComponent<DemoCameraController>();
	this->cameraController->GetTransform()->SetWorldPosition(glm::vec3(1024, 64, 512));
	this->cameraController->GetTransform()->LookAt(glm::vec3(0.0, 20.0, 0.0));
	this->cameraController->camera->SetRenderTexture(raymarchingTarget);
	this->cameraController->camera->backgroundColor = glm::vec4(.9, .95, 1.0, 1.0);
	this->cameraController->camera->SetFarClip(2000);
	this->cameraController->camera->SetNearClip(10);
	this->cameraController->camera->clearStencil = true;

	GameObject * portalGO = GameObject::Instantiate("portal");
	portalGO->GetTransform()->SetLocalScale(glm::vec3(50.f, 150.f, 20.f));
	portalGO->GetTransform()->SetLocalPosition(glm::vec3(256.f, 0.f, 256.f));
	MeshRenderer * portalRenderer = portalGO->AddComponent<MeshRenderer>();
	portalRenderer->SetMesh(MeshFactory::BuildCube(true));
	Material * portalMaterial = new Material("flat");
	portalRenderer->SetMaterial(portalMaterial);

	Material::StencilOperation stencilPortal;
	stencilPortal.mask = 0xFF;
	stencilPortal.operation = GL_ALWAYS;
	stencilPortal.fail = GL_REPLACE;
	stencilPortal.zFail = GL_REPLACE;
	stencilPortal.pass = GL_REPLACE;
	portalMaterial->SetFeature(GL_STENCIL_TEST, true);
	portalMaterial->SetDepthWrite(false);
	portalMaterial->SetStencilOperation(stencilPortal);

	GameObject * terrainGO = GameObject::Instantiate("terrain");
	this->terrain = terrainGO->AddComponent<Terrain>();

	Material::StencilOperation stencilTerrain;
	stencilTerrain.mask = 0x00;
	stencilTerrain.operation = GL_NOTEQUAL;
	stencilTerrain.fail = GL_KEEP;
	stencilTerrain.zFail = GL_KEEP;
	stencilTerrain.pass = GL_KEEP;
	this->terrain->material->SetFeature(GL_STENCIL_TEST, true);
	this->terrain->material->SetStencilOperation(stencilTerrain);

	GameObject * secondaryTerrainGO = GameObject::Instantiate("terrain");
	Terrain * secondaryTerrain = secondaryTerrainGO->AddComponent<Terrain>();
	secondaryTerrain->GetTransform()->SetLocalRotation(glm::vec3(glm::radians(180.f), 0.f, 0.f));
	secondaryTerrain->GetTransform()->SetLocalPosition(glm::vec3(0, 100.f, 1024.f));

	Material::StencilOperation stencilTerrainSecondary;
	stencilTerrainSecondary.mask = 0x00;
	stencilTerrainSecondary.operation = GL_EQUAL;
	stencilTerrainSecondary.fail = GL_KEEP;
	stencilTerrainSecondary.zFail = GL_KEEP;
	stencilTerrainSecondary.pass = GL_KEEP;
	secondaryTerrain->material->SetFeature(GL_STENCIL_TEST, true);
	secondaryTerrain->material->SetStencilOperation(stencilTerrainSecondary);


	GameObject * waterGO = GameObject::Instantiate("water");
	waterGO->GetTransform()->SetLocalPosition(glm::vec3(0.f, -105.f, 0.f));
	waterGO->GetTransform()->SetLocalRotation(glm::vec3(glm::radians(-90.f), 0.f, 0.f));
	waterGO->GetTransform()->SetLocalScale(glm::vec3(1000.f));
	MeshRenderer * waterRenderer = waterGO->AddComponent<MeshRenderer>();
	waterRenderer->SetMesh(MeshFactory::BuildQuad());
	Material * waterMaterial = new Material("raymarched/water");
	waterMaterial->SetTexture("ReflectedHeightfield", terrain->floatingPointHeightmap);
	waterMaterial->SetVector("TerrainScale", glm::vec4(1.f / 1024.f, 1.f, 1.f / 1024.f, 0.f));
	waterMaterial->SetTexture("RandomTexture", randomTexture);
	waterRenderer->SetMaterial(waterMaterial);


	GameObject * lightPillar = GameObject::Instantiate("lightPillar");
	MeshRenderer * pillarRenderer = lightPillar->AddComponent<MeshRenderer>();
	pillarRenderer->SetMesh(MeshFactory::BuildCube(true, true));
	pillarRenderer->GetTransform()->SetLocalScale(glm::vec3(10.f, 1000.f, 10.f));
	pillarRenderer->GetTransform()->SetLocalPosition(glm::vec3(512, 64, 512));

	Material * pillarMaterial = new Material("raymarched/light_pillar");
	pillarMaterial->SetTexture("RandomTexture", randomTexture);
	pillarMaterial->SetFeature(GL_BLEND, true);
	pillarMaterial->SetBlendOperation(Material::BlendOperation(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	pillarRenderer->SetMaterial(pillarMaterial);
	

	raymarchedMaterials.push_back(pillarMaterial);
	raymarchedMaterials.push_back(terrain->material);
	raymarchedMaterials.push_back(waterMaterial);
	raymarchedMaterials.push_back(secondaryTerrain->material);

	ShaderPassComposer * composer = new ShaderPassComposer();
	composer->SetSourceTarget(raymarchingTarget);

	//// Iterative raymarcher
	
	//ShaderPass * raymarchingPass = new ShaderPass("terrain", raymarchingTarget);
	//mainQuadMaterial = raymarchingPass->GetMaterial();

	//composer->AddPass(raymarchingPass);

	//// Copy buffer to feedback buffer
	//RenderTexture * feedbackBuffer = new RenderTexture(screenSize.x, screenSize.y, true, 32, TextureParameters(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP));
	//ShaderPass * copyPass = new ShaderPass("minKernel", feedbackBuffer);
	//copyPass->SetIgnoreTarget(true);
	//composer->AddPass(copyPass);

	//mainQuadMaterial->SetTexture("Heightfield", heightmap);
	//mainQuadMaterial->SetTexture("FeedbackBuffer", feedbackBuffer);
	
	// Shade
	ShaderPass * shadingPass = new ShaderPass("pass_passthrough");
	composer->AddPass(shadingPass);

	Engine::GetInstance()->AddShaderComposer(composer);
}

void DemoController::Update()
{
	/*this->mainQuadMaterial->SetVector("CameraPosition", glm::vec4(this->cameraController->GetTransform()->WorldPosition(), 1.0));
	glm::mat4 invProj = glm::inverse(this->cameraController->camera->GetViewProjectionMatrix());
	this->mainQuadMaterial->SetMatrix("InvViewProjection", invProj);*/

	for(int i = 0; i < raymarchedMaterials.size(); i++)
		raymarchedMaterials[i]->SetVector("CameraPosition", glm::vec4(this->cameraController->GetTransform()->WorldPosition(), 1.0));
}

Texture * DemoController::BuildRandomTexture()
{
	int length = 512;
	int size = length * length;
	std::mt19937 mersenne(140401956);
	std::uniform_real_distribution<> distr(0, 255);

	uint8_t * pixels = new uint8_t[size*4];

	for (int i = 0; i < size; i++)
	{
		glm::uint r = distr(mersenne);
		glm::uint g = distr(mersenne);
		glm::uint b = distr(mersenne);
		glm::uint a = distr(mersenne);

		pixels[i * 4] = r;
		pixels[i * 4 + 1] = g;
		pixels[i * 4 + 2] = b;
		pixels[i * 4 + 3] = a;
	}

	Texture * t = new Texture();
	t->LoadFromRaw(pixels, length, length, TextureParameters(GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT));
	return t;
}
