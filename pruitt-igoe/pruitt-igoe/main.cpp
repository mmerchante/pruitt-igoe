#include <SFML/Graphics.hpp>

#include "core\assets\assetdatabase.h"
#include "core\assets\texture.h"
#include "core\engine.h"

#include <iostream>

int main()
{
	Engine * engine = Engine::GetInstance();

	sf::Window * window = new sf::Window(sf::VideoMode(640, 480, 32), "pruitt-igoe");
	window->setMouseCursorVisible(false);

	engine->Initialize(window);
	engine->Start();
	
	return 0;
}