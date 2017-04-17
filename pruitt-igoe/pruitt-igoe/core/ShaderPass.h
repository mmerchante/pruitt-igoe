#pragma once

#include <vector>

class RenderTexture;
class Material;
class Mesh;
class Camera;

class ShaderPass
{
public:
	ShaderPass(const std::string& shaderName, RenderTexture * target, Camera * sourceCamera = nullptr);
	virtual ~ShaderPass();

	// If specific passes need some information updated before render
	virtual void Update();
	RenderTexture * Render(Mesh * quad, RenderTexture * source);

private:
	RenderTexture * target; // The target buffer of this pass
	Material * material;
	Camera * sourceCamera; // Which projection matrices to send
};

class ShaderPassComposer
{
public:
	ShaderPassComposer();
	~ShaderPassComposer();
	void Render();
	void AddPass(ShaderPass * pass);
	void SetSourceTarget(RenderTexture * source);

private:
	std::vector<ShaderPass*> passes;
	RenderTexture * source;
	Mesh * quad;
};