#include <SFML/Graphics.hpp>

#include "core\assets\assetdatabase.h"
#include "core\assets\texture.h"
#include "core\engine.h"
#include "core\material.h"
#include "core\gameobject.h"
#include "core\components\renderer.h"
#include "core\components\camera.h"
#include "core\assets\meshfactory.h"

#include <iostream>

int main()
{
	Engine * engine = Engine::GetInstance();

	sf::Window * window = new sf::Window(sf::VideoMode(640, 480, 32), "pruitt-igoe");
	window->setMouseCursorVisible(false);

	engine->Initialize(window);

	engine->GetLog()->SetLogLevel(Log::LogLevel::Verbose);

	GameObject * cameraGO = GameObject::Instantiate("mainCamera");
	PerspectiveCamera * mainCamera = cameraGO->AddComponent<PerspectiveCamera>();
		
	Material * material = new Material("pennFlag");
	material->SetFeature(GL_DEPTH_TEST, false);

	GameObject * quadGO = GameObject::Instantiate("quad");
	MeshRenderer * r = quadGO->AddComponent<MeshRenderer>();
	r->SetMesh(MeshFactory::BuildQuad(true));
	r->SetMaterial(material);
	
	engine->Start();
	
	return 0;
}