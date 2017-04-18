#include "DemoCameraController.h"

void DemoCameraController::Awake()
{
	this->camera = this->gameObject->AddComponent<PerspectiveCamera>();
	this->speed = 15.f;
	this->horizontalAngle = 0.f;
	this->verticalAngle = 0.f;

	// Hacky, but SFML is not very flexible with this...
	glm::vec2 screenCenter = glm::vec2(Engine::GetScreenSize()) * .5f;
	sf::Mouse::setPosition(sf::Vector2i(screenCenter.x, screenCenter.y), *Engine::GetInstance()->GetWindow());
}

void DemoCameraController::Update()
{
	Transform * t = this->gameObject->GetTransform();

	glm::vec3 forward = t->Forward();
	glm::vec3 right = t->Right();
	glm::vec3 displacement = right * velocity.x + forward * velocity.z;

	t->SetLocalPosition(t->LocalPosition() + displacement * Engine::DeltaTime() * speed);

	if (Engine::GetInstance()->GetWindow()->hasFocus() && Engine::MouseLocked())
	{
		glm::vec2 mousePos = Engine::GetInstance()->GetCurrentMousePosition();
		glm::vec2 screenCenter = glm::vec2(Engine::GetScreenSize()) * .5f;

		glm::vec2 delta = mousePos - screenCenter;

		if (std::fabs(delta.x) < 0.1f && std::fabs(delta.y) < .1f)
			return;

		float sensibility = .25f;

		this->verticalAngle -= delta.y * sensibility * Engine::DeltaTime();
		this->horizontalAngle -= delta.x * sensibility * Engine::DeltaTime();

		Engine::CenterMousePosition();
	}
	
	// Clamp vertical angle
	this->verticalAngle = glm::clamp(verticalAngle, -.49f * glm::pi<float>(), .49f * glm::pi<float>());
	t->SetLocalRotation(glm::vec3(verticalAngle, horizontalAngle, 0.f));
}

void DemoCameraController::OnKeyPressEvent(sf::Event::KeyEvent * e)
{
	if (e->code == sf::Keyboard::Key::W)
		this->velocity.z += 1;

	if (e->code == sf::Keyboard::Key::D)
		this->velocity.x += 1;

	if (e->code == sf::Keyboard::Key::S)
		this->velocity.z += -1;

	if (e->code == sf::Keyboard::Key::A)
		this->velocity.x += -1;
}

void DemoCameraController::OnKeyReleaseEvent(sf::Event::KeyEvent * e)
{
	if (e->code == sf::Keyboard::Key::W)
		this->velocity.z -= 1;

	if (e->code == sf::Keyboard::Key::D)
		this->velocity.x -= 1;

	if (e->code == sf::Keyboard::Key::S)
		this->velocity.z -= -1;

	if (e->code == sf::Keyboard::Key::A)
		this->velocity.x -= -1;
}