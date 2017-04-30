#version 450

#define MAX_ITERATIONS 400
#define SECONDARY_ITERATIONS 5
#define EPSILON .05
#define NORMAL_ESTIMATION_EPSILON 2.0

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
	float steepness = pow(abs(normal.y), 10.0);


	vec3 lightDirection = normalize(vec3(-1.0, 1.0, -1.0));
	float cosTheta = dot(normal, lightDirection);

	vec3 refl = reflect(normal, rayDirection);

	float rim = pow(1.0 - dot(rayDirection, -normal), 5.0);

	float diffuse = saturate(cosTheta) * .7;
	float ambient = .05f + rim * .05;
	float specular = pow(saturate(dot(refl, lightDirection)), 2.0) * (1.0 - steepness) * .25 * diffuse;

	vec3 amb = vec3(.4, .5, .8);
	vec3 terrainColor = vec3(.25, .3, .08) * .35;
	terrainColor = mix(vec3(.125, .05, 0) * .1, terrainColor, steepness);	

	return terrainColor * diffuse * 8.0 + amb * specular + (amb * ambient * ((t / 250)));// vec3(diffuse);
}