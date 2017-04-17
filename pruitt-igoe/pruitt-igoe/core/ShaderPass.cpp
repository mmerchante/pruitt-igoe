#include "ShaderPass.h"
#include "assets\texture.h"
#include "assets\mesh.h"
#include "assets\meshfactory.h"
#include "material.h"
#include "components\camera.h"
#include "engine.h"

ShaderPass::ShaderPass(const std::string & shaderName, RenderTexture * target, Camera * sourceCamera) : target(target), sourceCamera(sourceCamera)
{
	this->material = new Material(shaderName);
	this->material->SetFeature(GL_DEPTH_TEST, false);
}

ShaderPass::~ShaderPass()
{
	if (target != nullptr)
		delete target;

	if (material != nullptr)
		delete material;
}

void ShaderPass::Update()
{
}

RenderTexture * ShaderPass::Render(Mesh * quad, RenderTexture * source)
{
	if (target != nullptr)
		glBindFramebuffer(GL_FRAMEBUFFER, target->GetFramebufferID());
	else
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, target->GetWidth(), target->GetHeight());

	if(source != nullptr)
		this->material->SetTexture("SourceTexture", source);
	
	if(sourceCamera != nullptr)
		this->material->Render(quad, sourceCamera->GetViewProjectionMatrix(), glm::mat4(), glm::mat4(), glm::mat4(), Engine::Time());
	else
		this->material->Render(quad, glm::mat4(), glm::mat4(), glm::mat4(), glm::mat4(), Engine::Time());

	return this->target;
}

ShaderPassComposer::ShaderPassComposer()
{
	this->quad = MeshFactory::BuildQuad();
}

ShaderPassComposer::~ShaderPassComposer()
{
	delete this->quad;
}

void ShaderPassComposer::Render()
{
	RenderTexture * currentTarget = source;

	for (int i = 0; i < passes.size(); i++)
	{
		passes[i]->Update();
		currentTarget = passes[i]->Render(this->quad, currentTarget);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShaderPassComposer::AddPass(ShaderPass * pass)
{
	this->passes.push_back(pass);
}

void ShaderPassComposer::SetSourceTarget(RenderTexture * source)
{
	this->source = source;
}
