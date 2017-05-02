#version 450

#define MAX_ITERATIONS 1
#define SECONDARY_ITERATIONS 0
#define EPSILON .01
#define NORMAL_ESTIMATION_EPSILON .05

#define REFLECTION_ITERATIONS 20
#define REFLECTION_EPSILON .5
//#define DEBUG_REFLECTIONS
#define REFLECTIONS

#include <Raymarching>

uniform sampler2D ReflectedHeightfield;
uniform vec4 TerrainScale; // Scaling in the sdf to prevent transformation errors
uniform float Time;
  
float sceneReflections(vec3 point)
{ 
	vec2 uv = point.xz * TerrainScale.xz / .75;
	float h = textureLod(ReflectedHeightfield, uv, 3).r;	
	float d = (point.y - h);
	return d;
} 

vec3 shadeReflections(vec3 point, vec3 normal, vec3 rayOrigin, vec3 rayDirection, float t)
{ 
	vec2 uv = point.xz * TerrainScale.xz;

	vec3 lightPosition = vec3(512.0, 0, 0.0);

	float steepness = saturate(pow(smoothstep(.5, 1.0, abs(normal.y)), 3.0));
	float snow = saturate((point.y + steepness * 10.0 + 35.f)/10.f) * abs(normal.y * normal.y * normal.y);

	vec3 lightDirection = normalize(vec3(1.8, .15, .6));// normalize(lightPosition - point);
	float cosTheta = dot(normal, lightDirection);
 
	vec3 refl = reflect(normal, rayDirection);
	float rim = pow(1.0 - dot(rayDirection, -normal), 5.0);

	float diffuse = pow(smoothstep(0.0, 1.0, cosTheta * .5 + .5), 3.0);
	float ambient = .04f + rim * .0075 - saturate(-cosTheta) * .13;
	float specular = pow(saturate(dot(refl, lightDirection)), 1.5) * (1.0 - steepness) * .15 * diffuse;

	vec3 amb = vec3(.2, .5, .9);
	vec3 terrainColor = vec3(.9, .8, .08) * .5;
	terrainColor *= steepness * 2.0;
	terrainColor = mix(terrainColor, vec3(.8, .8, 1.0) * .3, snow);
	terrainColor *= .25 + pow(saturate((point.y + 75.0) / 1500.f), 3.0) * 3.0; 

	vec3 outColor = terrainColor * diffuse + amb * specular + amb * ambient;
	outColor = mix(outColor, amb, saturate(-.15 + t / 900.0)) + amb * saturate(-cosTheta) * .15;

	return outColor;
}

float scene(vec3 point)
{
	return point.y;
}

//vec3 shadeReflections(vec3 point, vec3 normal, vec3 rayOrigin, vec3 rayDirection, float t)
//{
//	vec3 l = normalize(vec3(1.0));
//	float diffuse = clamp(dot(normal, l), 0.0, 1.0);
//	return vec3(diffuse);
//}

vec3 shade(vec3 point, vec3 normal, vec3 rayOrigin, vec3 rayDirection, float t)
{
	vec3 l = normalize(vec3(1.0));
	float diffuse = clamp(dot(-normal, l), 0.0, 1.0);

	normal = vec3(sin((point.x + point.z) * .85 + Time * 4.0) * .015 + .1, 1.0, 0.0);
	//normal.xz += sin(length(point.xz - vec2(412.0, 400))) * .2 - .2;

	vec3 refl = reflect(rayDirection, normalize(normal));
	refl.y = abs(refl.y) * .5;
 
	return reflections(point, normalize(refl), 100.f);
}