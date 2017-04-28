#include "Terrain.h"

void Terrain::Awake()
{
	ReadableTexture * heightmap = AssetDatabase::GetInstance()->LoadAsset<ReadableTexture>("resources/heightfield_1.png", TextureParameters(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP));

	int size = heightmap->GetWidth() * heightmap->GetHeight();
	float * hpHeightmap = new float[size];

	for (int i = 0; i < size; i++)
		//hpHeightmap[i] = glm::sin(i * .0001f) * 70.f;
		hpHeightmap[i] = (float)heightmap->GetRawPixels()[i*4];

	Mesh * terrainMesh = GenerateMesh(hpHeightmap, heightmap->GetWidth(), heightmap->GetHeight(), 0.25f);
	this->renderer = this->gameObject->AddComponent<MeshRenderer>();
	this->renderer->SetMesh(terrainMesh);

	this->material = new Material("diffuse");
	//this->material->SetOverrideDrawingMode(GL_LINES);
	this->renderer->SetMaterial(material);

	delete[] hpHeightmap;
}

Mesh * Terrain::GenerateMesh(float * heightmap, int width, int height, float scale)
{
	std::vector<GLuint> indices;
	std::vector<glm::vec4> vertices;
	std::vector<glm::vec2> uvs;

	glm::vec2 pixelSize = glm::vec2(1.f / width, 1.f / height);

	// For now, I only need vertices
	int index = 0;
	for (int y = 0; y < height - 1; y++)
	{
		for (int x = 0; x < width - 1; x++)
		{
			float h1 = heightmap[y * width + x];
			float h2 = heightmap[y * width + (x+1)];
			float h3 = heightmap[(y + 1) * width + (x+1)];
			float h4 = heightmap[(y + 1) * width + x];

			glm::vec4 v1 = glm::vec4(x    , h1 * scale, y    , 1.f);
			glm::vec4 v2 = glm::vec4(x + 1, h2 * scale, y    , 1.f);
			glm::vec4 v3 = glm::vec4(x + 1, h3 * scale, y + 1, 1.f);
			glm::vec4 v4 = glm::vec4(x    , h4 * scale, y + 1, 1.f);

			glm::vec2 uv1 = glm::vec2(x, y + 1) * pixelSize;
			glm::vec2 uv2 = glm::vec2(x + 1, y + 1) * pixelSize;
			glm::vec2 uv3 = glm::vec2(x + 1, y) * pixelSize;
			glm::vec2 uv4 = glm::vec2(x, y) * pixelSize;

			vertices.push_back(v1);
			vertices.push_back(v2);
			vertices.push_back(v3);
			vertices.push_back(v4);

			uvs.push_back(uv1);
			uvs.push_back(uv2);
			uvs.push_back(uv3);
			uvs.push_back(uv4);

			indices.push_back(index + 2);
			indices.push_back(index + 1);
			indices.push_back(index + 0);

			indices.push_back(index + 0);
			indices.push_back(index + 3);
			indices.push_back(index + 2);

			index += 4;
		}
	}
	
	Mesh * output = new Mesh();
	output->SetIndices(indices.data(), indices.size(), true);
	output->SetVertices(vertices.data(), vertices.size(), true);
	//output->SetUVs(uvs.data(), uvs.size(), true);
	output->GenerateNormals();
	output->Upload(true);
	return output;
}
