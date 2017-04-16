#ifndef CAMERA_H
#define CAMERA_H

#include "../../common.h"
#include "../component.h"

class Camera : public Component
{
protected:
    virtual ~Camera();

    // Computed
    float aspect;
    glm::mat4 viewProjectionMatrix;
	glm::mat4 projectionMatrix;

    // Input
    int width;
    int height;
    float nearClip;
    float farClip;
    glm::vec4 backgroundColor;

    // Depends on each camera type!
    virtual glm::mat4 ComputeProjectionMatrix() = 0;

public:
    virtual void Awake();
    virtual void PhysicsUpdate();
    void UpdateScreenSize();

	const glm::mat4& GetProjectionMatrix();
    const glm::mat4& GetViewProjectionMatrix();
    const glm::vec3 GetViewVector();
	float GetFarClip();
	float GetNearClip();
};

class PerspectiveCamera : public Camera
{
public:
    virtual void Awake();
	void SetFieldOfView(float fieldOfView);
	float GetFieldOfView();

protected:
	float fieldOfView;
	virtual glm::mat4 ComputeProjectionMatrix();
};

class OrthographicCamera: public Camera
{
    virtual glm::mat4 ComputeProjectionMatrix();
};

class UICamera: public Camera
{
public:
    virtual void Awake();
    virtual ~UICamera();
    virtual glm::mat4 ComputeProjectionMatrix();
	virtual void PhysicsUpdate();
};

#endif // CAMERA_H
