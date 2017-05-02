#version 450

#define MAX_ITERATIONS 150
#define SECONDARY_ITERATIONS 5
#define EPSILON .01
#define NORMAL_ESTIMATION_EPSILON 1.0

// SHADOW PARAMETERS
#define SHADOW_ITERATIONS 200
#define SHADOW_SOFT_FACTOR 10.0
#define SHADOW_EPSILON .85
#define SHADOW_OFFSET 1.0
#define SHADOW_BIAS 2.0

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
	//vec3 n = texture2D(HeightfieldNormal, uv).rgb * 2.0 - vec3(1.0);

	//float angle = atan(n.y, n.x) ;
	//float deriv = (1.0 - smoothstep(n.z - .1, n.z, .8)) * sin((angle + uv.x + uv.y) * .5) * .5 + .5;

	float h = texture2D(Heightfield, uv).r;

	//deriv *= 1.5;

	//h = h - deriv;
	
	float d = (point.y - h);
	return d;
} 

vec3 shade(vec3 point, vec3 normal, vec3 rayOrigin, vec3 rayDirection, float t)
{ 
	vec2 uv = point.xz * TerrainScale.xz;
	float tex = texture2D(Heightfield, uv).r;

	vec3 lightPosition = vec3(2048.0, 255.0, 0.0);

	float steepness = saturate(pow(abs(normal.y), 15.0));
	float snow = saturate((point.y + steepness * 10.0 + 100.f)/100.f) * steepness;

	vec3 lightDirection = normalize(lightPosition - point);
	float cosTheta = dot(normal, lightDirection);
 
	vec3 refl = reflect(normal, rayDirection);
	float rim = pow(1.0 - dot(rayDirection, -normal), 5.0);

	float diffuse = saturate(cosTheta) * .7;
	float ambient = .04f + rim * .0075 + saturate(-cosTheta) * .013;
	float specular = pow(saturate(dot(refl, lightDirection)), 1.5) * (1.0 - steepness) * .1 * diffuse;

	vec3 amb = vec3(.2, .5, .9);
	vec3 terrainColor = vec3(.9, .8, .08) * .065;
	terrainColor = mix(vec3(.5, .4, 0.05) * .002, terrainColor, steepness);	
	terrainColor = mix(terrainColor, vec3(.8, .8, 1.0) * .1, snow);

	vec3 lightColor = vec3(1.25, 1.2, .9);

	vec3 shadow = vec3(1.0);// mix(vec3(.1, .35, .9), vec3(1.0), 1.0 - shadows(point + normal * 3.0, lightPosition));

	vec3 outColor = terrainColor * diffuse * 30.0 + amb * specular + amb * ambient;
	outColor = mix(outColor, amb, saturate(-.1 + t / 1000.0)) * shadow + amb * saturate(-cosTheta) * .15;

	vec3 desaturated = vec3(pow(dot(outColor, vec3(0.299, 0.587, 0.114)), 1.5));

	return desaturated;// + vec3(1.0, .25, 0.0) * smoothstep(abs(tex), abs(tex) + 2.0, 5.0) * 1.5;//mix(outColor, desaturated, .35);

	//return vec3(step(length(texture2D(HeightfieldNormal, point.xz * TerrainScale.xz).rgb), 1.15));
}