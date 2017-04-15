#ifndef SHADER_H
#define SHADER_H

#include "../../common.h"
#include "asset.h"
#include <vector>

class Mesh;

class Shader: public Asset
{
public:
    Shader();
    ~Shader();

	void LoadFromFilename(std::string filename);
	std::string GetName();

    void Upload();
    void Bind();
    void Render(Mesh * mesh, GLenum drawMode);

    void SetIntUniform(int uniform, int value);
    void SetFloatUniform(int uniform, float value);
    void SetMatrixUniform(int uniform, const glm::mat4& matrix);
    void SetMatrixArrayUniform(int uniform, glm::mat4* matrix, int size);
    void SetVectorUniform(int uniform, const glm::vec4& v);
    void SetIVectorUniform(int uniform, const glm::ivec4 &v);
    void SetTextureUniform(int uniform, GLuint value);   

protected:

	std::string ReadFile(const std::string& filename);
	
	void PrintShaderInfoLog(int shader);
	int GetUniformLocation(const char *uniform);
	
private:
	std::string shaderName;
	std::string vertexFilename;
	std::string fragmentFilename;

	GLuint vertShader;
	GLuint fragShader;
	GLuint prog;

	int attrPos;
	int attrNor;
	int attrCol;
	int attrUV;

	bool loaded;
};

#endif // SHADER_H
