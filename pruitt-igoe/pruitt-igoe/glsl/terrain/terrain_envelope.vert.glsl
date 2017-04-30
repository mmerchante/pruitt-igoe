#version 330
#define FAR_CLIP 1000.0

struct VertexData
{
	vec4 ssPos;
	vec3 wsPos;
};

uniform mat4 ViewProj;
uniform mat4 Model;

in vec4 vertexPosition;
in vec4 vertexNormal;
in vec2 vertexUV;

out VertexData vertexData;

void main() 
{
	// Inflate the mesh
	vec4 p = Model * vertexPosition;
	p.xyz += vertexNormal.xyz * 4.0f;

	vertexData.wsPos = p.xyz;
	vertexData.ssPos = ViewProj * p;
    gl_Position = vertexData.ssPos;
}