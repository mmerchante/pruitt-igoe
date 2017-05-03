#version 450

#define MAX_ITERATIONS 200
#define SECONDARY_ITERATIONS 5
#define EPSILON .01
#define NORMAL_ESTIMATION_EPSILON 1.0

// SHADOW PARAMETERS
#define SHADOW_ITERATIONS 200
#define SHADOW_SOFT_FACTOR 20.0
#define SHADOW_EPSILON 3.85
#define SHADOW_OFFSET 5.0
#define SHADOW_BIAS 1.5

#define SHADOWS
  
//#define DEBUG

#include <Raymarching>
 
uniform sampler2D Heightfield;
uniform sampler2D HeightfieldNormal;

uniform vec4 TerrainScale; // Scaling in the sdf to prevent transformation errors
uniform float Time;
 
float scene(vec3 point)
{
	vec2 uv = point.xz * TerrainScale.xz;
	float h = texture2D(Heightfield, uv).r;
	float d = (point.y - h);
	return d;
} 

vec3 shade(vec3 point, vec3 normal, vec3 rayOrigin, vec3 rayDirection, float t)
{ 
	vec2 uv = point.xz * TerrainScale.xz;
	float snow = saturate(smoothstep(.6, .65, normal.y) - step(-35.0, point.y));

	vec3 lightPosition = vec3(512.0, 50, 512.0);
	float falloff = 5000.0 / pow(length(lightPosition - point) + .001, 2.0);

	vec3 lightDirection = normalize(lightPosition - point);
	float cosTheta = dot(normal, lightDirection);
 
	vec3 refl = reflect(normal, rayDirection);
	float rim = pow(1.0 - dot(rayDirection, -normal), 3.0);

	float diffuse = pow(smoothstep(0.0, 1.0, cosTheta * .5 + .5), 3.0) * .25 + .25 + rim * .5;
	float ambient = .04f + rim * .0075 - saturate(-cosTheta) * .13;
	float specular = pow(saturate(dot(refl, lightDirection)), 1.5) * (1.0 - snow) * .15 * diffuse;

	vec3 amb = vec3(.2, .5, .9);

	vec3 terrainColor = mix(vec3(.1, .1, .3), vec3(1.0), snow);

	//vec3 terrainColor = vec3(.9, .8, .08) * .5;
	//terrainColor *= steepness * 2.0;
	//terrainColor = mix(terrainColor, vec3(.8, .8, 1.0) * .3, snow);
	//terrainColor *= .25 + pow(saturate((point.y + 75.0) / 1500.f), 3.0) * 3.0; 

	vec3 shadow = mix(vec3(.1, .35, .9), vec3(1.0), shadows(point - normal * 1.0, lightPosition));

	vec3 outColor = terrainColor * diffuse + amb * specular + amb * ambient;
	outColor = mix(outColor, amb * 2.0, saturate(-.15 + t / 400.0)) * shadow;// + amb * saturate(-cosTheta) * .15;

	vec3 desaturated = vec3(pow(dot(outColor, vec3(0.299, 0.587, 0.114)), 1.5));


	return outColor * falloff;// + vec3(1.0, .25, 0.0) * smoothstep(abs(tex), abs(tex) + 2.0, 5.0) * 1.5;//mix(outColor, desaturated, .35);

	//return vec3(step(length(texture2D(HeightfieldNormal, point.xz * TerrainScale.xz).rgb), 1.15));
}