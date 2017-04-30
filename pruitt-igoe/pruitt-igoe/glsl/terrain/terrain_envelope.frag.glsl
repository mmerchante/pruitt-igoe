#version 450

#define MAX_ITERATIONS 200
#define SECONDARY_ITERATIONS 5
#define EPSILON .05
#define NORMAL_ESTIMATION_EPSILON 2.0

// SHADOW PARAMETERS
#define SHADOW_ITERATIONS 100
#define SHADOW_SOFT_FACTOR 20.0
#define SHADOW_EPSILON .5
#define SHADOW_OFFSET 10.0

#define SHADOWS
  
//#define DEBUG

#include <Raymarching>

uniform sampler2D Heightfield;
uniform vec4 TerrainScale; // Scaling in the sdf to prevent transformation errors
uniform float Time;
 
float scene(vec3 point)
{
	float h = texture2D(Heightfield, point.xz * TerrainScale.xz).r * TerrainScale.y;
	float d = (point.y - h);
	return d;
} 

vec3 shade(vec3 point, vec3 normal, vec3 rayOrigin, vec3 rayDirection, float t)
{ 
	vec3 lightPosition = vec3(1024.0, 155.0, 2024.0);

	float steepness = saturate(pow(abs(normal.y), 10.0));
	float snow = saturate(smoothstep(150.0 * (1.0 - steepness), 250.0, point.y));

	vec3 lightDirection = normalize(lightPosition - point);
	float cosTheta = dot(normal, lightDirection);
 
	vec3 refl = reflect(normal, rayDirection);
	float rim = pow(1.0 - dot(rayDirection, -normal), 5.0);

	float diffuse = saturate(cosTheta) * .7;
	float ambient = .04f + rim * .075 + saturate(-cosTheta) * .03;
	float specular = pow(saturate(dot(refl, lightDirection)), 1.5) * (1.0 - steepness) * .3 * diffuse;

	vec3 amb = vec3(.2, .5, .9);
	vec3 terrainColor = vec3(.9, .8, .08) * .065;
	terrainColor = mix(vec3(.5, .4, 0.05) * .002, terrainColor, steepness);	
	terrainColor = mix(terrainColor, vec3(.8, .8, 1.0) * .1, snow);

	vec3 lightColor = vec3(1.25, 1.2, .9);

	vec3 shadow = mix(vec3(.1, .35, .9), vec3(1.0), 1.0 - shadows(point + normal, lightPosition));

	vec3 outColor = terrainColor * diffuse * 30.0 + amb * specular + amb * ambient;

	outColor = mix(outColor, amb, saturate(-.175 + t / 2000.0)) * shadow + amb * saturate(-cosTheta) * .05;

	vec3 desaturated = vec3(pow(dot(outColor, vec3(0.299, 0.587, 0.114)), 1.5));

	return mix(outColor, desaturated, .8);
}