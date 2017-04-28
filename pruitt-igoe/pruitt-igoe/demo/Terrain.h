#pragma once

#include "../core/engine_common.h"

class Terrain : public Component
{
public:
	
	void Awake();

protected:

	Mesh * GenerateMesh(float * heightmap, int width, int height, float scale);
	Material * material;
	MeshRenderer * renderer;
};

