#version 450

#define MAX_ITERATIONS 400
#define SECONDARY_ITERATIONS 5
#define EPSILON .05
#define NORMAL_ESTIMATION_EPSILON 3.0

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
	float steepness = saturate(pow(abs(normal.y), 10.0));
	float snow = saturate(smoothstep(200.0 * (1.0 - steepness), 300.0, point.y));

	vec3 lightDirection = normalize(vec3(-1.0, 1.0, -1.0));
	float cosTheta = dot(normal, lightDirection);

	vec3 refl = reflect(normal, rayDirection);
	float rim = pow(1.0 - dot(rayDirection, -normal), 5.0);

	float diffuse = saturate(cosTheta) * .7;
	float ambient = .04f + rim * .075 + saturate(-cosTheta) * .03;
	float specular = pow(saturate(dot(refl, lightDirection)), 2.0) * (1.0 - steepness) * .1 * diffuse;

	vec3 amb = vec3(.2, .5, .9);
	vec3 terrainColor = vec3(.25, .3, .08) * .05;
	terrainColor = mix(vec3(.1, .05, 0) * .03, terrainColor, steepness);	
	terrainColor = mix(terrainColor, vec3(.8, .8, 1.0) * .1, snow);

	
	vec3 outColor = terrainColor * diffuse * 20.0 + amb * specular + amb * ambient;

	return mix(outColor, amb, saturate(-.2 + t / 2000.0));
}