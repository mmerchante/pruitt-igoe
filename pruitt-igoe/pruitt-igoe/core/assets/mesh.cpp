#include "mesh.h"

Mesh::Mesh() : Asset(), interleaved(false), vertexCount(0), indicesCount(0),
    indices(nullptr), interleavedData(nullptr), vertices(nullptr), normals(nullptr), colors(nullptr), UVs(nullptr),
    bufIdx(0), bufInterleaved(0),  bufPos(0), bufNor(0), bufCol(0),  bufUV(0),
    idxBound(false), interleavedBound(false), posBound(false), norBound(false), colBound(false), UVBound(false)
{
}

Mesh::~Mesh()
{
	Destroy();
	DeleteInternal();
}

void Mesh::Interleave()
{
    if(interleaved && vertexCount != 0 && vertices != nullptr)
    {
        interleavedData = new Vertex[vertexCount];

        for(int i = 0; i < vertexCount; i++)
        {
            interleavedData[i].position = vertices[i];
            interleavedData[i].normal = (normals != nullptr) ? normals[i] : glm::vec4(1.f);
            interleavedData[i].color = (colors != nullptr) ? colors[i] : glm::vec4(1.f);
            interleavedData[i].uv = (UVs != nullptr) ? UVs[i] : glm::vec2(1.f);
        }
    }
}

void Mesh::SetInterleaved(bool interleaved)
{
    this->interleaved = interleaved;
}

bool Mesh::IsInterleaved()
{
    return interleaved;
}

int Mesh::GetIndicesCount()
{
    return indicesCount;
}

int Mesh::GetVertexCount()
{
    return vertexCount;
}

void Mesh::SetIndices(glm::uint *indices, int triangleCount, bool copy)
{
    this->indicesCount = triangleCount;

    if(copy)
    {
        this->indices = new glm::uint[indicesCount];
        std::memcpy(this->indices, indices, this->indicesCount * sizeof(glm::uint));
    }
    else
    {
        this->indices = indices;
    }
}

void Mesh::SetInterleavedData(Vertex *data, int vertexCount, bool copy)
{
    this->vertexCount = vertexCount;

    if(copy)
    {
        this->interleavedData = new Vertex[vertexCount];
        std::memcpy(this->interleavedData, data, vertexCount * sizeof(Vertex));
    }
    else
    {
        this->interleavedData = data;
    }
}

void Mesh::SetVertices(glm::vec4 *vertices, int vertexCount, bool copy)
{
    this->vertexCount = vertexCount;

	if (copy)
	{
		this->vertices = new glm::vec4[vertexCount];
		std::memcpy(this->vertices, vertices, vertexCount * sizeof(glm::vec4));
	}
	else
	{
		this->vertices = vertices;
	}
}

void Mesh::SetNormals(glm::vec4 *normals, int vertexCount, bool copy)
{
	if (this->vertexCount == vertexCount)
	{
		if (copy)
		{
			this->normals = new glm::vec4[vertexCount];
			std::memcpy(this->normals, normals, vertexCount * sizeof(glm::vec4));
		}
		else
		{
			this->normals = normals;
		}
	}
    /*else
        Engine::LogError("Normals and vertices dont match!");*/
}

void Mesh::SetColors(glm::vec4 *colors, int vertexCount, bool copy)
{
	if (this->vertexCount == vertexCount)
	{
		if (copy)
		{
			this->colors = new glm::vec4[vertexCount];
			std::memcpy(this->colors, colors, vertexCount * sizeof(glm::vec4));
		}
		else
		{
			this->colors = colors;
		}
	}
    /*else
        Engine::LogError("Colors and vertices dont match!");*/
}

void Mesh::SetUVs(glm::vec2 *UVs, int vertexCount, bool copy)
{
	if (this->vertexCount == vertexCount)
	{
		if (copy)
		{
			this->UVs = new glm::vec2[vertexCount];
			std::memcpy(this->UVs, UVs, vertexCount * sizeof(glm::vec2));
		}
		else
		{
			this->UVs = UVs;
		}
	}
    /*else
        Engine::LogError("Uvs and vertices dont match!");*/
}

//glm::vec4 Mesh::CalculateVertexforScreenQuad(float X, float Y, glm::mat4 InvViewProj)
//{
//    glm::vec4 vertex = InvViewProj* glm::vec4(X, Y, 0.9f, 1.0f);
//
//    vertex.x = vertex.x / vertex.w;
//    vertex.y = vertex.y / vertex.w;
//    vertex.z = vertex.z / vertex.w;
//    vertex.w = 1.0f;
//    return vertex;
//}
//
//void Mesh::UpdateForScreenQaud(glm::mat4 invVP, int w, int h)
//{
//    float left = -1.0f;
//    float right = 1.0f;
//    float top = 1.0f;
//    float bottom = -1.0f;
//
//    // Load the vertex array with data.
//    glm::vec4 vertex = CalculateVertexforScreenQuad(left, top, invVP);
//    vertices[1] = glm::vec4(vertex.x, vertex.y, vertex.z, 1.0f);  // Top left.
//
//    vertex = CalculateVertexforScreenQuad(right, bottom, invVP);
//    vertices[3] = glm::vec4(vertex.x, vertex.y, vertex.z, 1.0f); // Bottom right.
//
//    vertex = CalculateVertexforScreenQuad(left, bottom, invVP);
//    vertices[0] = glm::vec4(vertex.x, vertex.y, vertex.z, 1.0f); // Bottom left.
//
//    vertex = CalculateVertexforScreenQuad(right, top, invVP);
//    vertices[2] = glm::vec4(vertex.x, vertex.y, vertex.z, 1.0f);  // Top right.
//
//    context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
//    context->glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * sizeof(glm::vec4), vertices);
//}
//

void Mesh::Upload(bool deleteInternal)
{
    // Create a VBO on our GPU and store its handle in bufIdx
    if(indices != nullptr)
    {
        if(!BindIndices()) {
            GenerateIndices();
			BindIndices();
        }

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof(GLuint), indices, GL_STATIC_DRAW);
    }

	// The next few sets of function calls are basically the same as above, except bufPos and bufNor are
	// array buffers rather than element array buffers, as they store vertex attributes like position.
    if(interleaved)
    {
        if(interleavedData != nullptr)
        {
            if(!BindInterleaved()) {
                GenerateInterleaved();
				BindInterleaved();
            }

            glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), interleavedData, GL_STATIC_DRAW);
        }
    }
    else
    {
        if(vertices != nullptr)
        {
            GenerateVertices();
			BindVertices();
            
			glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(glm::vec4), vertices, GL_STATIC_DRAW);
        }

        if(normals != nullptr)
        {
            GenerateNormals();
			BindNormals();

			glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(glm::vec4), normals, GL_STATIC_DRAW);
        }
		
        if(colors != nullptr)
        {
            GenerateColors();
			BindColors();

			glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(glm::vec4), colors, GL_STATIC_DRAW);
        }

        if(UVs != nullptr)
        {
            GenerateUVs();
			BindUVs();
			glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(glm::vec2), UVs, GL_STATIC_DRAW);
        }
    }

    // If we only want to write/upload, delete all data after it was uploaded
    if(deleteInternal)
    {
		DeleteInternal();
    }
}

void Mesh::Destroy()
{
	if(bufIdx)
		glDeleteBuffers(1, &bufIdx);

	if(bufPos)
		glDeleteBuffers(1, &bufPos);

	if(bufNor)
		glDeleteBuffers(1, &bufNor);

	if(bufCol)
		glDeleteBuffers(1, &bufCol);

	if(bufUV)
		glDeleteBuffers(1, &bufUV);

	if(bufInterleaved)
		glDeleteBuffers(1, &bufInterleaved);

	// Just in case...
	bufIdx = 0;
	bufPos = 0;
	bufNor = 0;
	bufCol = 0;
	bufUV = 0;
	bufInterleaved = 0;
}

void Mesh::DeleteInternal()
{
	if (indices != nullptr)
		delete[] indices;

	if (interleavedData != nullptr)
		delete[] interleavedData;

	if (vertices != nullptr)
		delete[] vertices;

	if (normals != nullptr)
		delete[] normals;

	if (colors != nullptr)
		delete[] colors;

	if (UVs != nullptr)
		delete[] UVs;

	indices = nullptr;
	interleavedData = nullptr;
	vertices = nullptr;
	normals = nullptr;
	colors = nullptr;
	UVs = nullptr;
}

void Mesh::Validate()
{
    bool valid = true;

    if(indices != nullptr)
    {
        if(vertices == nullptr && interleavedData == nullptr)
        {
            valid = false;
        }
        else
        {
            for(int i = 0; i < indicesCount; i++)
                if(indices[i] >= (glm::uint) vertexCount)
                    valid = false;
        }
    }

    //Engine::LogDebug("VALID? " + std::to_string(valid));
}

void Mesh::GenerateIndices()
{
	if (!idxBound)
	{
		idxBound = true;
		glGenBuffers(1, &bufIdx);
	}
}

void Mesh::GenerateInterleaved()
{
	if (!interleavedBound)
	{
		interleavedBound = true;
		glGenBuffers(1, &bufInterleaved);
	}
}

void Mesh::GenerateVertices()
{
	if (!posBound)
	{
		posBound = true;
		glGenBuffers(1, &bufPos);
	}
}

void Mesh::GenerateNormals()
{
	if (!norBound)
	{
		norBound = true;
		glGenBuffers(1, &bufNor);
	}
}

void Mesh::GenerateColors()
{
	if (!colBound)
	{
		colBound = true;
		glGenBuffers(1, &bufCol);
	}
}

void Mesh::GenerateUVs()
{
	if (!UVBound)
	{
		UVBound = true;
		glGenBuffers(1, &bufUV);
	}
}

bool Mesh::BindIndices()
{
    if(idxBound)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);

    return idxBound;
}

bool Mesh::BindInterleaved()
{
    if(interleavedBound)
        glBindBuffer(GL_ARRAY_BUFFER, bufInterleaved);

    return interleavedBound;
}

bool Mesh::BindVertices()
{
    if(posBound)
        glBindBuffer(GL_ARRAY_BUFFER, bufPos);

    return posBound;
}

bool Mesh::BindNormals()
{
    if(norBound)
        glBindBuffer(GL_ARRAY_BUFFER, bufNor);

    return norBound;
}

bool Mesh::BindColors()
{
    if(colBound)
        glBindBuffer(GL_ARRAY_BUFFER, bufCol);

    return colBound;
}

bool Mesh::BindUVs()
{
   if(UVBound)
       glBindBuffer(GL_ARRAY_BUFFER, bufUV);

   return UVBound;
}