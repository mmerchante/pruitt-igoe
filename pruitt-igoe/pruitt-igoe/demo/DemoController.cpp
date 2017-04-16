#include "DemoController.h"

void DemoController::Awake()
{
	GameObject * cameraGO = GameObject::Instantiate("mainCamera");
	this->cameraController = cameraGO->AddComponent<DemoCameraController>();
	this->cameraController->GetTransform()->SetWorldPosition(glm::vec3(8, 8, 8));
	this->cameraController->GetTransform()->LookAt(glm::vec3());
	
	this->mainQuadMaterial = new Material("minion");
	this->mainQuadMaterial->SetFeature(GL_DEPTH_TEST, false);

	this->mainQuad = GameObject::Instantiate("quad");
	ShaderReloader * reloader = mainQuad->AddComponent<ShaderReloader>();
	reloader->material = mainQuadMaterial;

	MeshRenderer * r = mainQuad->AddComponent<MeshRenderer>();
	r->SetMesh(MeshFactory::BuildQuad(true));
	r->SetMaterial(mainQuadMaterial);
}

void DemoController::Update()
{
	this->mainQuadMaterial->SetVector("CameraPosition", glm::vec4(this->cameraController->GetTransform()->WorldPosition(), 1.0));
	glm::mat4 invProj = glm::inverse(this->cameraController->camera->GetViewProjectionMatrix());
	this->mainQuadMaterial->SetMatrix("InvViewProjection", invProj);
}