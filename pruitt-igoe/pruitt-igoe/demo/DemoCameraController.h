#include "../core/engine_common.h"

#pragma once
class DemoCameraController : public Component, public InputListener
{
public:
	void Awake();
	void Update();

	void OnKeyPressEvent(sf::Event::KeyEvent * e);
	void OnKeyReleaseEvent(sf::Event::KeyEvent * e);

	PerspectiveCamera * camera;

private:
	glm::vec3 velocity;
	float speed;
	float verticalAngle;
	float horizontalAngle;
};

