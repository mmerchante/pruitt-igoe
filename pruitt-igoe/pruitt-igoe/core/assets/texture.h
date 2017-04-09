#ifndef TEXTURE_H
#define TEXTURE_H

#include "../../common.h"
#include "asset.h"

struct TextureParameters
{
	GLint minFilter;
	GLint magFilter;
	GLint wrapS;
	GLint wrapT;

	TextureParameters() : minFilter(GL_NEAREST), magFilter(GL_NEAREST), wrapS(GL_REPEAT), wrapT(GL_REPEAT) {}
};

class Texture : public Asset
{
public:
    Texture();
    virtual ~Texture();

    int GetWidth();
    int GetHeight();
	bool IsValid();

    GLuint GetTextureID();

	void LoadFromRaw(const uint8_t * pixels, int width, int height, const TextureParameters& p);

    virtual void LoadFromFilename(const std::string& filename);
	virtual void LoadFromFilename(const std::string& filename, const TextureParameters& p);

protected:
	int width;
	int height;
	GLuint textureID;
	TextureParameters parameters;
};

#endif // TEXTURE_H
