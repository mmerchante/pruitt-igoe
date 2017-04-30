#include "Terrain.h"

#define SHOW_WIREFRAME false

void Terrain::Awake()
{
	ReadableTexture * heightmap = AssetDatabase::GetInstance()->LoadAsset<ReadableTexture>("resources/heightfield_3.png", TextureParameters(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP));

	int resolution = 64;
	int width = heightmap->GetWidth() / resolution;
	int height = heightmap->GetHeight() / resolution;

	int size = width * height;
	float * hpHeightmap = new float[size];

	for (int i = 0; i < size; i++)
	{
		int dstX = i % width;
		int dstY = i / width;

		int srcX = dstX * resolution;
		int srcY = dstY * resolution;

		int srcIndex = srcY * heightmap->GetWidth() + srcX;

		// For testing purposes, uncomment
		//hpHeightmap[i] = glm::sin(i * .01f) * .5f + .5f;
		hpHeightmap[i] = (float)heightmap->GetRawPixels()[srcIndex * 4];
	}

	float verticalScale = 300.f;
	float scale = .5f;
	
	Mesh * terrainMesh = GenerateMesh(hpHeightmap, width, height, verticalScale, resolution);
	this->renderer = this->gameObject->AddComponent<MeshRenderer>();
	this->renderer->SetMesh(terrainMesh);
	this->material = new Material("terrain/terrain_envelope");
	this->renderer->SetMaterial(material);

	// Match the raymarched terrain with the mesh
	this->GetTransform()->SetLocalScale(glm::vec3(scale, 1.f, scale));
	//this->GetTransform()->SetLocalScale(glm::vec3(heightmap->GetWidth(), 1, heightmap->GetHeight()));
	this->material->SetVector("TerrainScale", glm::vec4(1.f / heightmap->GetWidth(), verticalScale, 1.f / heightmap->GetHeight(), 0.f));

	if (SHOW_WIREFRAME)
	{
		GameObject * wireframeGO = this->gameObject;// GameObject::Instantiate("wireframeTerrain");
		MeshRenderer * wireframeRenderer = wireframeGO->AddComponent<MeshRenderer>();
		wireframeRenderer->SetMesh(terrainMesh);

		Material * wireframeMat = new Material("terrain/wireframe");
		wireframeMat->SetColor("Color", glm::vec4(.1, .1, .1, 1.0f));
		wireframeMat->SetOverrideDrawingMode(GL_LINES);
		wireframeMat->SetFeature(GL_DEPTH_TEST, false);
		wireframeRenderer->SetMaterial(wireframeMat);
	}

	delete[] hpHeightmap;
}

Mesh * Terrain::GenerateMesh(float * heightmap, int width, int height, float scale, float resolution)
{
	std::vector<GLuint> indices;
	std::vector<glm::vec4> vertices;
	std::vector<glm::vec2> uvs;

	glm::vec2 pixelSize = glm::vec2(1.f / width, 1.f / height);
	glm::vec4 resolutionMultiplier = glm::vec4(resolution, scale / 255.f, resolution, 1.f);

	// Vertex data
	int index = 0;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			float h = heightmap[y * width + x];

			glm::vec4 v = glm::vec4(x, h, y, 1.f);
			glm::vec2 uv = glm::vec2(x, y + 1) * pixelSize;

			vertices.push_back(v * resolutionMultiplier);
			uvs.push_back(uv);

			if (x < width - 1 && y < height - 1)
			{
				int i1 = y * width + x;
				int i2 = y * width + x + 1;
				int i3 = (y + 1) * width + x;
				int i4 = (y + 1) * width + x + 1;

				indices.push_back(i3);
				indices.push_back(i2);
				indices.push_back(i1);

				indices.push_back(i4);
				indices.push_back(i2);
				indices.push_back(i3);
			}
		}
	}

	Engine::LogInfo("Terrain mesh vertex count: " + std::to_string(vertices.size()));
	
	Mesh * output = new Mesh();
	output->SetIndices(indices.data(), indices.size(), true);
	output->SetVertices(vertices.data(), vertices.size(), true);
	output->SetUVs(uvs.data(), uvs.size(), true);
	output->GenerateNormals();
	output->Upload(true);
	return output;
}
