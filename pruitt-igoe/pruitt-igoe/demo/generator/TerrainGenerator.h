#pragma once
#include "../../common.h"

class BaseGenerator
{
public:
	virtual void Generate(float * terrain, int width, int height) = 0;
};

class FractalGenerator : public BaseGenerator
{
public:
	FractalGenerator(int octaves, float frequency, float frequencyMultiplier, float amplitudeMultiplier, float amplitude);
	virtual void Generate(float * terrain, int width, int height);

	void SetNormalMap(glm::vec3 * normalMap, int size);

private:

	float fbm(float x, float y);
	float advFractal(float x, float y);
	float perlin2D(float x, float y);

	glm::vec3 * terrainNormalMap;
	int terrainNormalMapSize;

	int octaves;
	float frequency;
	float frequencyMultiplier;
	float amplitudeMultiplier;
	float amplitude;
	int gradientMapSize;
};

class Filter
{
public:
	virtual void Evaluate(float * terrain, float * output, int width, int height) = 0;
};


class TerrainGenerator
{
public:
	TerrainGenerator();
	virtual ~TerrainGenerator();

	float * Generate(int width, int height);

	void SetBaseGenerator(BaseGenerator * gen);

private:
	std::vector<Filter*> filters;
	BaseGenerator *generator;
};

