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

#include <iostream>


class ShaderReloader : public Component, public InputListener
{
public:

	Material * material;

	virtual void OnKeyPressEvent(sf::Event::KeyEvent * e)
	{
		if (e->code == sf::Keyboard::Key::R)
		{
			if (material != nullptr)
			{
				material->Reload();
				Engine::LogInfo("Reloading shader!");
			}
		}
	}
};

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
	ShaderReloader * reloader = quadGO->AddComponent<ShaderReloader>();
	reloader->material = material;

	MeshRenderer * r = quadGO->AddComponent<MeshRenderer>();
	r->SetMesh(MeshFactory::BuildQuad(true));
	r->SetMaterial(material);

	GameObject * titleTest = GameObject::Instantiate("Titles");
	UIImage * image = titleTest->AddComponent<UIImage>();
	image->SetTexture("./resources/title.png");	
	image->GetTransform()->UIScale(.1f);
	
	GameObject * fpsCounter = GameObject::Instantiate("fps");
	fpsCounter->AddComponent<FPSCounter>();
	
	engine->Start();
	
	return 0;
}