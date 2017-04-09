#include <SFML/Graphics.hpp>

#include "renderer\GLRenderer.h"
#include "core\assets\assetdatabase.h"
#include "core\assets\texture.h"

#include <iostream>

int main()
{
	GLRenderer renderer;
	renderer.Initialize();
	
	AssetDatabase * db = AssetDatabase::GetInstance();
	Texture * tx = db->LoadAsset<Texture>("test.png");
	std::cout << tx->GetWidth() << std::endl;
	
	renderer.Render();
		
	return 0;
}