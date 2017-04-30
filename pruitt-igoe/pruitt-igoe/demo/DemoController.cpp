#include "DemoController.h"
#include "Terrain.h"

void DemoController::Awake()
{
	glm::vec2 screenSize = glm::vec2(Engine::GetScreenSize());
	RenderTexture * raymarchingTarget = new RenderTexture(screenSize.x, screenSize.y, true, 32, TextureParameters(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP));
	//raymarchingTarget->AddDrawBuffer(GL_DEPTH_ATTACHMENT);
	raymarchingTarget->Load();

	GameObject * raymarchingCamera = GameObject::Instantiate("raymarchingCamera");
	this->cameraController = raymarchingCamera->AddComponent<DemoCameraController>();
	this->cameraController->GetTransform()->SetWorldPosition(glm::vec3(1024, 64, 512));
	this->cameraController->GetTransform()->LookAt(glm::vec3(0.0, 20.0, 0.0));
	this->cameraController->camera->SetRenderTexture(raymarchingTarget);
	this->cameraController->camera->backgroundColor = glm::vec4(.4, .5, .6, 1.0);

	GameObject * terrainGO = GameObject::Instantiate("terrain");
	this->terrain = terrainGO->AddComponent<Terrain>();

	Texture * heightmap = AssetDatabase::GetInstance()->LoadAsset<Texture>("resources/heightfield_3.png", TextureParameters(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP));

	terrain->material->SetTexture("Heightfield", heightmap);

	GameObject * lightPillar = GameObject::Instantiate("lightPillar");
	MeshRenderer * pillarRenderer = lightPillar->AddComponent<MeshRenderer>();
	pillarRenderer->SetMesh(MeshFactory::BuildCube(true, true));
	pillarRenderer->GetTransform()->SetLocalScale(glm::vec3(10.f, 1000.f, 10.f));
	pillarRenderer->GetTransform()->SetLocalPosition(glm::vec3(35.f, 500.f, 50.f));

	Material * pillarMaterial = new Material("raymarched/light_pillar");
	//pillarMaterial->SetFeature(GL_CULL_FACE, false);
	pillarRenderer->SetMaterial(pillarMaterial);

	raymarchedMaterials.push_back(pillarMaterial);
	raymarchedMaterials.push_back(terrain->material);

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