#pragma once

#include "../core/engine_common.h"

class Terrain : public Component
{
public:
	void Awake();
	Material * material;

protected:
	Mesh * GenerateMesh(float * heightmap, int width, int height, float scale, float resolution);
	MeshRenderer * renderer;
};

