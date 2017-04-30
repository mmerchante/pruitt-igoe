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
	this->cameraController->GetTransform()->SetWorldPosition(glm::vec3(175, 25, 175));
	this->cameraController->GetTransform()->LookAt(glm::vec3(0.0, 20.0, 0.0));
	this->cameraController->camera->SetRenderTexture(raymarchingTarget);

	GameObject * terrainGO = GameObject::Instantiate("terrain");
	this->terrain = terrainGO->AddComponent<Terrain>();

	Texture * heightmap = AssetDatabase::GetInstance()->LoadAsset<Texture>("resources/heightfield_1.png", TextureParameters(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP));

	terrain->material->SetTexture("Heightfield", heightmap);
		
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

	this->terrain->material->SetVector("CameraPosition", glm::vec4(this->cameraController->GetTransform()->WorldPosition(), 1.0));
}