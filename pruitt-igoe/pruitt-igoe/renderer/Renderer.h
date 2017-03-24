#pragma once

class Renderer
{
public:
	virtual ~Renderer();

	Renderer();

	virtual void Render() = 0;
	virtual void Initialize() = 0;
};