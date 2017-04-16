#include "camera.h"
#include "../gameobject.h"

Camera::~Camera()
{
    Engine::GetInstance()->DeregisterCamera(this);

	if (this->renderTexture != nullptr)
		delete this->renderTexture;
}

void Camera::Awake()
{
    this->aspect = 1.f;
    this->viewProjectionMatrix = glm::mat4(1.f);
	this->width = Engine::GetInstance()->GetScreenSize().x;
	this->height = Engine::GetInstance()->GetScreenSize().y;
    this->nearClip = .1f;
    this->farClip = 1000.f;
    this->backgroundColor = glm::vec4(.2f, .2f, .2f, 0.f);
	this->mask = CullingMask::Default;
	this->clearColor = true;
	this->clearDepth = true;

    Engine::GetInstance()->RegisterCamera(this);
}

void Camera::PhysicsUpdate()
{
    this->aspect = width / (float)height;
	this->projectionMatrix = ComputeProjectionMatrix();
    this->viewProjectionMatrix = projectionMatrix * this->GetTransform()->WorldToLocalMatrix();
}

void Camera::UpdateScreenSize()
{
    this->width = Engine::GetInstance()->GetScreenSize().x;
	this->height = Engine::GetInstance()->GetScreenSize().y;

    PhysicsUpdate();
}

const glm::mat4 & Camera::GetProjectionMatrix()
{
	return projectionMatrix;
}

const glm::mat4 &Camera::GetViewProjectionMatrix()
{
    return viewProjectionMatrix;
}

const glm::vec3 Camera::GetViewVector()
{
    Transform * trans = this->gameObject->GetTransform();
    return  trans->Forward();
}

float Camera::GetFarClip()
{
	return farClip;
}

float Camera::GetNearClip()
{
	return nearClip;
}

void Camera::Render()
{
	if(renderTexture != nullptr)
		glBindFramebuffer(GL_FRAMEBUFFER, renderTexture->GetFramebufferID());
	else
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, width, height);

	int flags = 0;
	if (clearColor)
		flags |= GL_COLOR_BUFFER_BIT;

	if(clearDepth)
		flags |= GL_DEPTH_BUFFER_BIT;

	if (flags)
	{
		glClear(flags);
		glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.z);
	}
}

void Camera::FinishRender()
{
	if (this->renderTexture != nullptr)
	{
		this->renderTexture->GenerateMipmaps();
	}
}

bool Camera::Cull(int layer)
{
	return (layer & this->mask) == 0;
}

void Camera::SetRenderTexture(RenderTexture * rt)
{
	this->renderTexture = rt;
}

RenderTexture * Camera::GetRenderTexture()
{
	return renderTexture;
}

glm::mat4 PerspectiveCamera::ComputeProjectionMatrix()
{
    return glm::perspective(glm::radians(fieldOfView), aspect, nearClip, farClip);
}

void PerspectiveCamera::Awake()
{
    Camera::Awake();
    this->fieldOfView = 75.f;
}

void PerspectiveCamera::SetFieldOfView(float fieldOfView)
{
	this->fieldOfView = fieldOfView;
}

float PerspectiveCamera::GetFieldOfView()
{
	return fieldOfView;
}

glm::mat4 OrthographicCamera::ComputeProjectionMatrix()
{
    return glm::ortho(0.f, (float)width, 0.f, (float)height, nearClip, farClip);
}

void UICamera::Awake()
{
    this->aspect = 1.f;
    this->viewProjectionMatrix = glm::mat4(1.f);
    this->width = Engine::GetInstance()->GetScreenSize().x;
	this->height = Engine::GetInstance()->GetScreenSize().y;
    this->nearClip = .1f;
    this->farClip = 100.f;
	this->mask = CullingMask::All; //TODO: add UI later
    this->backgroundColor = glm::vec4(.2f, .2f, .2f, 1.f);
	this->clearColor = false;
	this->clearDepth = false; // UI draws over everything
}

UICamera::~UICamera()
{
}

glm::mat4 UICamera::ComputeProjectionMatrix()
{
    return glm::ortho(0.f, (float)width, 0.f, (float)height, nearClip, farClip);
}

void UICamera::PhysicsUpdate()
{
	this->aspect = width / (float)height;
	this->projectionMatrix = ComputeProjectionMatrix();
	this->viewProjectionMatrix = projectionMatrix * glm::translate(glm::mat4(1), glm::vec3(0, 0, -10)); // We need to offset Z a bit
}
