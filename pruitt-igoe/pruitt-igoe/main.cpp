#include <SFML/Graphics.hpp>

#include "core\assets\assetdatabase.h"
#include "core\assets\texture.h"
#include "core\assets\shader.h"
#include "core\engine.h"
#include "core\material.h"
#include "core\gameobject.h"
#include "core\components\renderer.h"
#include "core\components\FPSCounter.h"
#include "core\components\camera.h"
#include "core\assets\meshfactory.h"
#include "core\ui\uitext.h"
#include "core\ui\uiimage.h"
#include "demo\DemoController.h"

#include <iostream>

#define DEBUG true

int main()
{
	Engine * engine = Engine::GetInstance();

	sf::Window * window = new sf::Window(sf::VideoMode(sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height, 32), "pruitt-igoe", sf::Style::Fullscreen);
	window->setMouseCursorVisible(false);
	
	engine->Initialize(window);
	engine->GetLog()->SetLogLevel(Log::LogLevel::Verbose);

	GameObject * demo = GameObject::Instantiate("demo");
	demo->AddComponent<DemoController>();
	
	if (DEBUG)
	{
		GameObject * fpsCounter = GameObject::Instantiate("fps");
		fpsCounter->AddComponent<FPSCounter>();
	}
	
	engine->Start();
	
	return 0;
}