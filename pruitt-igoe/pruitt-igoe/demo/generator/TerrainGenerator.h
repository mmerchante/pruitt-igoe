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
	FractalGenerator(float frequency, float persistence, float amplitude);
	virtual void Generate(float * terrain, int width, int height);

private:

	float fbm(float x, float y, float frequency, float persistence, float amplitude);
	float perlin2D(float x, float y);

	glm::vec2 * gradients;

	float frequency;
	float persistence;
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

