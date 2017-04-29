#pragma once

#include "../core/engine_common.h"
#include "DemoCameraController.h"
#include "Terrain.h"

class DemoController : public Component, public InputListener
{
public:
	void Awake();
	void Update();

private:
	DemoCameraController * cameraController;
	Material * mainQuadMaterial;
	GameObject * mainQuad;
	Terrain * terrain;
};

