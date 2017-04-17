#include "DemoController.h"

void DemoController::Awake()
{
	GameObject * raymarchingCamera = GameObject::Instantiate("raymarchingCamera");
	this->cameraController = raymarchingCamera->AddComponent<DemoCameraController>();
	this->cameraController->GetTransform()->SetWorldPosition(glm::vec3(8, 8, 8));
	this->cameraController->GetTransform()->LookAt(glm::vec3());

	ShaderPassComposer * composer = new ShaderPassComposer();

	glm::vec2 screenSize = Engine::GetScreenSize();
	RenderTexture * raymarchingTarget = new RenderTexture(screenSize.x, screenSize.y, true, 32, TextureParameters(GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT));
	ShaderPass * raymarchingPass = new ShaderPass("minion", raymarchingTarget);
	mainQuadMaterial = raymarchingPass->GetMaterial();
	composer->AddPass(raymarchingPass);

	ShaderPass * shadingPass = new ShaderPass("shadingPass");
	composer->AddPass(shadingPass);

	Engine::GetInstance()->AddShaderComposer(composer);
}

void DemoController::Update()
{
	this->mainQuadMaterial->SetVector("CameraPosition", glm::vec4(this->cameraController->GetTransform()->WorldPosition(), 1.0));
	glm::mat4 invProj = glm::inverse(this->cameraController->camera->GetViewProjectionMatrix());
	this->mainQuadMaterial->SetMatrix("InvViewProjection", invProj);
}