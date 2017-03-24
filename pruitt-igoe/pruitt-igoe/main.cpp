#include <SFML/Graphics.hpp>

#include "renderer\GLRenderer.h"

int main()
{
	GLRenderer renderer;
	renderer.Initialize();
	renderer.Render();
	return 0;
}