#include "texture.h"
#include "../engine.h"

Texture::Texture() : Asset(), textureID(-1), width(0), height(0)
{
}

Texture::~Texture()
{
	if(IsValid())
		glDeleteTextures(1, &textureID);
}

int Texture::GetWidth()
{
    return width;
}

int Texture::GetHeight()
{
    return height;
}

bool Texture::IsValid()
{
	return textureID != -1;
}

GLuint Texture::GetTextureID()
{
    return textureID;
}

void Texture::LoadFromRaw(const uint8_t * pixels, int width, int height, const TextureParameters & p)
{
	if (this->IsValid())
		return;

	this->width = width;
	this->height = height;

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, p.minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, p.magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, p.wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, p.wrapT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, pixels);

	if (p.minFilter == GL_LINEAR_MIPMAP_LINEAR)
		glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::LoadFromFilename(const std::string& filename)
{
	LoadFromFilename(filename, TextureParameters());
}

void Texture::LoadFromFilename(const std::string & filename, const TextureParameters & p)
{
	sf::Image image;

	if (!image.loadFromFile(filename))
	{
		Engine::LogError("Could not load " + filename);
		return;
	}
		
	const uint8_t * pixels = image.getPixelsPtr();

    if (!pixels)
        return;

	LoadFromRaw(pixels, image.getSize().x, image.getSize().y, p);
}