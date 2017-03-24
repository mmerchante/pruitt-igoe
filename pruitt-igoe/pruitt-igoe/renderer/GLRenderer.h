#pragma once
#include "Renderer.h"
#include <SFML/Graphics.hpp>

class GLRenderer : public Renderer
{
public:
	GLRenderer();
	~GLRenderer();

	virtual void Initialize();
	virtual void Render();
		
private:

	void PrepareQuad();

	sf::Window * window;
	bool focus;
};

