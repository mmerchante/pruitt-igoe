#version 450

#define MAX_ITERATIONS 10
#define SECONDARY_ITERATIONS 5
#define EPSILON .01
#define NORMAL_ESTIMATION_EPSILON .05

#include <Raymarching>

uniform float Time;

float scene(vec3 point)
{
	float d = sdCappedCylinder(point, vec2(.5, 1.0));
	return d;
}

vec3 shade(vec3 point, vec3 normal, vec3 rayOrigin, vec3 rayDirection, float t)
{
	vec3 light = vec3(35.0, 35.0, 50.0);
	vec3 l = light - point;

	float diffuse = clamp(dot(normal, normalize(l)), 0.0, 1.0) * .95 + .05;

	float falloff = 1500.0 / pow(length(l) + .001, 2.0);
	return vec3(diffuse * falloff);
}