#include "DemoController.h"

void DemoController::Awake()
{
	GameObject * raymarchingCamera = GameObject::Instantiate("raymarchingCamera");
	this->cameraController = raymarchingCamera->AddComponent<DemoCameraController>();
	this->cameraController->GetTransform()->SetWorldPosition(glm::vec3(8, 8, 8));
	this->cameraController->GetTransform()->LookAt(glm::vec3());
	
	this->mainQuadMaterial = new Material("pennFlag");
	this->mainQuadMaterial->SetFeature(GL_DEPTH_TEST, false);

	this->mainQuad = GameObject::Instantiate("raymarchingQuad");
	ShaderReloader * reloader = mainQuad->AddComponent<ShaderReloader>();
	reloader->material = mainQuadMaterial;

	MeshRenderer * r = mainQuad->AddComponent<MeshRenderer>();
	r->SetMesh(MeshFactory::BuildQuad(true));
	r->SetMaterial(mainQuadMaterial);

	glm::vec2 screenSize = Engine::GetScreenSize();
	RenderTexture * raymarchingPass = new RenderTexture(screenSize.x, screenSize.y, true, 32, TextureParameters(GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT));
	raymarchingPass->Load();

	cameraController->camera->SetRenderTexture(raymarchingPass);

	// TODO: camera order sorting...
	GameObject * mainCameraGO = GameObject::Instantiate("mainCamera");
	Camera * mainCamera = mainCameraGO->AddComponent<PerspectiveCamera>();
	mainCamera->mask = CullingMask::Layer1;
	
	Material * shadingMaterial = new Material("shadingPass");
	shadingMaterial->SetFeature(GL_DEPTH_TEST, false);
	shadingMaterial->SetTexture("RenderTexture", raymarchingPass);

	GameObject * shadingQuad = GameObject::Instantiate("shadingQuad");
	shadingQuad->SetLayer(2);
	reloader = shadingQuad->AddComponent<ShaderReloader>();
	reloader->material = shadingMaterial;

	r = shadingQuad->AddComponent<MeshRenderer>();
	r->SetMesh(MeshFactory::BuildQuad(true));
	r->SetMaterial(shadingMaterial);
}

void DemoController::Update()
{
	this->mainQuadMaterial->SetVector("CameraPosition", glm::vec4(this->cameraController->GetTransform()->WorldPosition(), 1.0));
	glm::mat4 invProj = glm::inverse(this->cameraController->camera->GetViewProjectionMatrix());
	this->mainQuadMaterial->SetMatrix("InvViewProjection", invProj);
}