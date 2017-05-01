#include "TerrainGenerator.h"
#include <random>

TerrainGenerator::TerrainGenerator()
{
}

TerrainGenerator::~TerrainGenerator()
{
}

float * TerrainGenerator::Generate(int width, int height)
{
	int size = width * height;
	float * terrain = new float[size];
	float * buffer = new float[size];

	if (this->generator != nullptr)
	{
		this->generator->Generate(terrain, width, height);

		for (int i = 0; i < filters.size(); i++)
		{
			filters[i]->Evaluate(terrain, buffer, width, height);

			float * swp = buffer;
			buffer = terrain;
			terrain = swp;
		}
	}

	delete[] buffer;
	return terrain;
}

void TerrainGenerator::SetBaseGenerator(BaseGenerator * gen)
{
	this->generator = gen;
}

FractalGenerator::FractalGenerator(float frequency, float persistence, float amplitude)
	: BaseGenerator(), frequency(frequency), persistence(persistence), amplitude(amplitude)
{
}

inline
float fade1(float t)
{
	float t3 = t * t * t;
	return 6.0 * t3 * t * t - 15.0 * t3 * t + 10.0 * t3;
}

// Projected into 1D, multiplying PI by some prime and using as axis
inline
float hash2D(glm::vec2 x)
{
	float i = glm::dot(x, glm::vec2(123.4031, 46.5244876));
	return glm::fract(glm::sin(i * 7.13) * 268573.103291);
}

int p[512] = { 0 };

int permutation[] = { 151,160,137,91,90,15,
131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

float grad(int hash, float x, float y)
{
	int h = hash & 15;
	float u = h < 8 ? x : y;
	float v = h < 4 ? y : h == 12 || h == 14 ? x : y;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

float FractalGenerator::perlin2D(float x, float y)
{
	// Perlin's original approach
	int X = x;
	int Y = y;		
	x -= X;
	y -= Y;
	X = X % 255;
	Y = Y % 255;

	float u = fade1(x);
	float v = fade1(y);

	int A = p[X] + Y;
	int AA = p[A];
	int AB = p[A + 1];

	int B = p[X + 1] + Y;
	int BA = p[B];
	int BB = p[B + 1];

	return glm::mix(glm::mix(grad(p[AA], x, y), grad(p[BA], x - 1, y), u),
					glm::mix(grad(p[AB], x, y - 1), grad(p[BB], x - 1, y - 1), u), 
					v);
}

float FractalGenerator::fbm(float x, float y, float frequency, float persistence, float amplitude)
{
	int octaves = 12;
	float accum = 0.f;
	float freq = frequency;
	float ampl = amplitude;
	float result = 0.f;

	glm::vec2 p = glm::vec2(x, y);

	for (int i = 0; i < octaves; i++)
	{
		result += perlin2D(p.x * freq, p.y * freq) * ampl;
		accum += ampl;
		freq *= 2.f;
		ampl *= persistence;
	}

	if (accum == 0)
		accum = 1.f;

	return result;
}

void FractalGenerator::Generate(float * terrain, int width, int height)
{
	this->gradientMapSize = width;
	this->gradients = new glm::vec2[gradientMapSize * gradientMapSize];

	std::mt19937 random(14041956);
	float inv = 1.f / random.max();

	// Permutations
	for (int i = 0; i < 256; i++)
		p[256 + i] = p[i] = permutation[i];
	
	for (int y = 0; y < gradientMapSize; y++)
		for (int x = 0; x < gradientMapSize; x++)
		{
			float angle = random() * inv * glm::two_pi<float>();
			this->gradients[y * gradientMapSize + x] = glm::normalize(glm::vec2(glm::cos(angle), glm::sin(angle)));
		}

	float invW = 1.f / width;
	float invH = 1.f / height;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			terrain[y * width + x] = fbm(x * invW, y * invH, frequency, persistence, amplitude);
		}
	}
}
