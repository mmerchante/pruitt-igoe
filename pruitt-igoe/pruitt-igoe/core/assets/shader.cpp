#include "shader.h"
#include "mesh.h"
#include "texture.h"
#include <iostream>
#include <fstream>
#include "../engine.h"

const std::string SHADER_BASE_DIRECTORY = "./glsl/";

Shader::Shader() : Asset(), shaderName(""),
    vertShader(-1), fragShader(-1), prog(-1),
    attrPos(-1), attrNor(-1), attrCol(-1), attrUV(-1), loaded(false)
{
    vertexFilename = "";
    fragmentFilename = "";
}

Shader::~Shader()
{
}

void Shader::LoadFromFilename(std::string filename)
{
    shaderName = filename;
    vertexFilename = SHADER_BASE_DIRECTORY + shaderName + ".vert.glsl";
    fragmentFilename = SHADER_BASE_DIRECTORY + shaderName + ".frag.glsl";
}

void Shader::Upload()
{    
    if(loaded)
        return;

    Engine::LogInfo("Loading " + vertexFilename);
    Engine::LogInfo("Loading " + fragmentFilename);

    vertShader = glCreateShader(GL_VERTEX_SHADER);
    fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    prog = glCreateProgram();

	std::string vertexSource = ReadFile(vertexFilename);
	std::string fragmentSource = ReadFile(fragmentFilename);
	
	//Engine::LogVerbose("VERTEX SHADER: \n" + vertexSource);
	//Engine::LogVerbose("FRAGMENT SHADER: \n" + fragmentSource);

	const char * vertSource = vertexSource.c_str();
	const char * fragSource = fragmentSource.c_str();

    // Send the shader text to OpenGL and store it in the shaders specified by the handles vertShader and fragShader
    glShaderSource(vertShader, 1, &vertSource, 0);
    glShaderSource(fragShader, 1, &fragSource, 0);

    // Tell OpenGL to compile the shader text stored above
    glCompileShader(vertShader);
    glCompileShader(fragShader);

    // Check if everything compiled OK
    GLint compiled;

    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
		PrintShaderInfoLog(vertShader);

    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
		PrintShaderInfoLog(fragShader);

    // Tell prog that it manages these particular vertex and fragment shaders
    glAttachShader(prog, vertShader);
    glAttachShader(prog, fragShader);
    glLinkProgram(prog);

    // Check for linking success
    GLint linked;
    glGetProgramiv(prog, GL_LINK_STATUS, &linked);

    if (!linked)
		PrintShaderInfoLog(prog);

    attrPos = glGetAttribLocation(prog, "vertexPosition");
    attrNor = glGetAttribLocation(prog, "vertexNormal");
    attrCol = glGetAttribLocation(prog, "vertexColor");
    attrUV = glGetAttribLocation(prog, "vertexUV");

    loaded = true;
}

void Shader::Bind()
{
	glUseProgram(prog);
}

//This function, as its name implies, uses the passed in GL widget
void Shader::Render(Mesh *mesh, GLenum drawMode)
{
    Bind();
	
    if(mesh->IsInterleaved())
    {
        if(mesh->BindInterleaved())
        {
            if (attrPos != -1)
            {
                glEnableVertexAttribArray(attrPos);
                glVertexAttribPointer(attrPos, 4, GL_FLOAT, false, sizeof(Vertex), 0);
            }

            if (attrNor != -1)
            {
                glEnableVertexAttribArray(attrNor);
                glVertexAttribPointer(attrNor, 4, GL_FLOAT, false, sizeof(Vertex), (void*)(sizeof(glm::vec4)));
            }

            if (attrCol != -1)
            {
                glEnableVertexAttribArray(attrCol);
                glVertexAttribPointer(attrCol, 4, GL_FLOAT, false, sizeof(Vertex), (void*)(2 * sizeof(glm::vec4)));
            }

            if (attrUV != -1)
            {
                glEnableVertexAttribArray(attrUV);
                glVertexAttribPointer(attrUV, 2, GL_FLOAT, false, sizeof(Vertex), (void*)(3 * sizeof(glm::vec4)));
            }
        }
    }
    else
    {
        if (attrPos != -1 && mesh->BindVertices()) {
            glEnableVertexAttribArray(attrPos);
            glVertexAttribPointer(attrPos, 4, GL_FLOAT, false, 0, NULL);
        }

        if (attrNor != -1 && mesh->BindNormals()) {
            glEnableVertexAttribArray(attrNor);
            glVertexAttribPointer(attrNor, 4, GL_FLOAT, false, 0, NULL);
        }

        if (attrCol != -1 && mesh->BindColors()) {
            glEnableVertexAttribArray(attrCol);
            glVertexAttribPointer(attrCol, 4, GL_FLOAT, false, 0, NULL);
        }

        if (attrUV != -1 && mesh->BindUVs()) {
            glEnableVertexAttribArray(attrUV);
            glVertexAttribPointer(attrUV, 2, GL_FLOAT, false, 0, NULL);
        }
    }

    // Bind the index buffer and then draw shapes from it.
    // This invokes the shader program, which accesses the vertex buffers.
    mesh->BindIndices();
    glDrawElements(drawMode, mesh->GetIndicesCount(), GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(attrPos);
    if (attrNor != -1) glDisableVertexAttribArray(attrNor);
    if (attrCol != -1) glDisableVertexAttribArray(attrCol);
    if (attrUV != -1) glDisableVertexAttribArray(attrUV);
}

std::string Shader::ReadFile(const std::string& filename)
{
	std::ifstream in(filename, std::ios::in);

	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}

	return "";
}

std::string Shader::GetName()
{
    return shaderName;
}

void Shader::PrintShaderInfoLog(int shader)
{
    int infoLogLen = 0;
    int charsWritten = 0;
    GLchar * infoLog;

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);

    if (infoLogLen > 0)
    {
        infoLog = new GLchar[infoLogLen + 1];
        glGetShaderInfoLog(shader, infoLogLen, &charsWritten, infoLog);
        std::cout << "ShaderInfoLog:" << std::endl << infoLog << std::endl;
        delete [] infoLog;
    }
}

int Shader::GetUniformLocation(const char *uniform)
{
    Bind();

    return glGetUniformLocation(prog, uniform);
}

void Shader::SetIntUniform(int uniform, int value)
{
    Bind();

    if(uniform != -1)
        glUniform1i(uniform, value);
}

void Shader::SetFloatUniform(int uniform, float value)
{
    Bind();

    if(uniform != -1)
        glUniform1f(uniform, value);
}

void Shader::SetTextureUniform(int uniform, GLuint value)
{
    Bind();

    if(uniform != -1)
    {
        glUniform1i(uniform, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, value);
    }
}

void Shader::SetMatrixUniform(int uniform, const glm::mat4 &matrix)
{
        Bind();

        if(uniform != -1)
            glUniformMatrix4fv(uniform, 1, GL_FALSE, &matrix[0][0]);
}

void Shader::SetMatrixArrayUniform(int uniform, glm::mat4 *matrix, int size)
{
    Bind();

    if(uniform != -1)
        glUniformMatrix4fv(uniform, size, GL_FALSE, &(matrix[0])[0][0]);
}

void Shader::SetVectorUniform(int uniform, const glm::vec4 &v)
{
    Bind();

    if(uniform != -1)
        glUniform4fv(uniform, 1, &v[0]);
}

void Shader::SetIVectorUniform(int uniform, const glm::ivec4 &v)
{
    Bind();

    if(uniform != -1)
        glUniform4iv(uniform, 1, &v[0]);
}