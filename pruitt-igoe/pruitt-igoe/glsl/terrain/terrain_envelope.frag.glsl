#version 450

#define MAX_ITERATIONS 100
#define SECONDARY_ITERATIONS 5
#define EPSILON .01
#define NORMAL_ESTIMATION_EPSILON 1.5

//#define DEBUG

#include <Raymarching>

uniform sampler2D Heightfield;
uniform vec4 TerrainScale; // Scaling in the sdf to prevent transformation errors

float scene(vec3 point)
{
	float h = texture2D(Heightfield, point.xz * TerrainScale.xz).r * TerrainScale.y;
	float d = (point.y - h);
	return d;
}

vec3 shade(vec3 point, vec3 normal, vec3 rayOrigin, vec3 rayDirection, float t)
{
	vec3 light = vec3(35.0, 100.0, 50.0);
	vec3 l = light - point;

	float diffuse = clamp(dot(normal, normalize(l)), 0.0, 1.0) * .75 + .25;

	float falloff = 1500.0 / pow(length(l) + .001, 2.0);
	return vec3(diffuse);
}