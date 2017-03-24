#include "GLRenderer.h"
#include <SFML\OpenGL.hpp>
//#include <gl\glew.h>
#define GLEW_STATIC

GLRenderer::GLRenderer() : Renderer()
{
	this->window = new sf::Window(sf::VideoMode(640, 480, 32), "pruitt-igoe");
	this->window->setMouseCursorVisible(false);
}

GLRenderer::~GLRenderer()
{
}

void GLRenderer::Initialize()
{
	// Set color and depth clear value
	glClearDepth(1.f);
	glClearColor(1.f, 0.f, 0.f, 0.f);

	glEnable(GL_TEXTURE_2D);
}

void GLRenderer::Render()
{
	while (window->isOpen())
	{
		sf::Event event;
		while (window->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window->close();
		}

		// Now render...
		glClear(GL_COLOR_BUFFER_BIT);
		window->display();
	}
}

void GLRenderer::PrepareQuad()
{
}
