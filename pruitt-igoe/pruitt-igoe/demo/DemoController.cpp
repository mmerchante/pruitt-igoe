#include "DemoController.h"


void DemoController::Awake()
{
	GameObject * raymarchingCamera = GameObject::Instantiate("raymarchingCamera");
	this->cameraController = raymarchingCamera->AddComponent<DemoCameraController>();
	this->cameraController->GetTransform()->SetWorldPosition(glm::vec3(175, 25, 175));
	this->cameraController->GetTransform()->LookAt(glm::vec3(0.0, 20.0, 0.0));

	ShaderPassComposer * composer = new ShaderPassComposer();

	// Iterative raymarcher
	glm::vec2 screenSize = glm::vec2(Engine::GetScreenSize());
	RenderTexture * raymarchingTarget = new RenderTexture(screenSize.x, screenSize.y, true, 32, TextureParameters(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP));
	ShaderPass * raymarchingPass = new ShaderPass("terrain", raymarchingTarget);
	mainQuadMaterial = raymarchingPass->GetMaterial();

	composer->AddPass(raymarchingPass);

	// Copy buffer to feedback buffer
	RenderTexture * feedbackBuffer = new RenderTexture(screenSize.x, screenSize.y, true, 32, TextureParameters(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP));
	ShaderPass * copyPass = new ShaderPass("minKernel", feedbackBuffer);
	copyPass->SetIgnoreTarget(true);
	composer->AddPass(copyPass);

	Texture * heightmap = AssetDatabase::GetInstance()->LoadAsset<Texture>("resources/heightfield_1.png", TextureParameters(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP));
	mainQuadMaterial->SetTexture("Heightfield", heightmap);
	mainQuadMaterial->SetTexture("FeedbackBuffer", feedbackBuffer);
	
	// Shade
	ShaderPass * shadingPass = new ShaderPass("pass_passthrough");
	composer->AddPass(shadingPass);

	Engine::GetInstance()->AddShaderComposer(composer);
}

void DemoController::Update()
{
	this->mainQuadMaterial->SetVector("CameraPosition", glm::vec4(this->cameraController->GetTransform()->WorldPosition(), 1.0));
	glm::mat4 invProj = glm::inverse(this->cameraController->camera->GetViewProjectionMatrix());
	this->mainQuadMaterial->SetMatrix("InvViewProjection", invProj);
}