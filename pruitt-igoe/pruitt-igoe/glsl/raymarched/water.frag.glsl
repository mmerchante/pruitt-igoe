#version 450

#define MAX_ITERATIONS 1
#define SECONDARY_ITERATIONS 0
#define EPSILON .01
#define NORMAL_ESTIMATION_EPSILON .05

#define REFLECTION_ITERATIONS 20
#define REFLECTION_EPSILON .5
//#define DEBUG_REFLECTIONS
#define REFLECTIONS
#define ENABLE_ALPHA

#include <Raymarching>

uniform sampler2D RandomTexture;
uniform sampler2D ReflectedHeightfield;
uniform vec4 TerrainScale; // Scaling in the sdf to prevent transformation errors
uniform float Time;  

float noise( in vec3 x )
{
    vec3 p = floor(x);
    vec3 f = fract(x);
	f = f*f*(3.0-2.0*f);
	
	vec2 uv = x.xz;
	vec3 rg = texture2D(RandomTexture, (uv + 0.5) / 512.0).rgb;
	return mix(rg.x, rg.y, rg.z );
}

// Both noise and fractal from iq's hell shadertoy: https://www.shadertoy.com/view/MdfGRX
float fractal(vec3 p)
{
	float f;
	vec3 q = p                          - vec3(0.0,1.0,0.0)*Time;
    f  = 0.50000*noise( q ); q = q*2.02 - vec3(0.0,1.0,0.0)*Time;
    f += 0.25000*noise( q ); q = q*2.03 - vec3(0.0,1.0,0.0)*Time;
    f += 0.12500*noise( q ); q = q*2.01 - vec3(0.0,1.0,0.0)*Time;
    f += 0.06250*noise( q ); q = q*2.02 - vec3(0.0,1.0,0.0)*Time;
    f += 0.03125*noise( q );

	return f;
}

float sceneReflections(vec3 point)
{ 
	vec2 uv = point.xz * TerrainScale.xz / .75;
	float h = textureLod(ReflectedHeightfield, uv, 1).r;	
	float d = (point.y - h);
	return d;
} 

float shadeAlpha(vec3 point)
{
	vec2 uv = point.xz * TerrainScale.xz / .75;
	float h = textureLod(ReflectedHeightfield, uv, 1).r;
	h += 90.0;
	return saturate(abs(h) * .2) * .85;
}

vec3 shadeReflections(vec3 point, vec3 normal, vec3 rayOrigin, vec3 rayDirection, float t)
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
	vec3 shadow = vec3(1.0);// mix(vec3(.1, .35, .9), vec3(1.0), shadows(point - normal * 1.0, lightPosition));

	vec3 outColor = terrainColor * diffuse + amb * specular + amb * ambient;
	outColor = mix(outColor, amb * 2.0, saturate(-.15 + t / 400.0)) * shadow;// + amb * saturate(-cosTheta) * .15;

	return outColor * falloff;// + vec3(1.0, .25, 0.0) * smoothstep(abs(tex), abs(tex) + 2.0, 5.0) * 1.5;
}

float scene(vec3 point)
{
	return point.y;
}

vec3 shade(vec3 point, vec3 normal, vec3 rayOrigin, vec3 rayDirection, float t)
{

	vec2 uv = point.xz * TerrainScale.xz / .75;
	float h = textureLod(ReflectedHeightfield, uv, 1).r;
	h += 90.0;
	float shoreline = sin(h * 2.0 + Time) * (1.0 - saturate(abs(h) * .2));

	vec3 l = normalize(vec3(1.0));
	float diffuse = clamp(dot(-normal, l), 0.0, 1.0);

	normal = normalize(normal + vec3(sin(point.x + point.z) *.001 + fractal(point * .25 + Time) * .1) + shoreline);
	//normal = vec3(sin((point.x + point.z) * .85 + Time * 4.0) * .015 + .1, 1.0, 0.0);

	normal += rayDirection * .2;

	float waveCenter = length(point.xz - vec2(700.0, 512));
	normal.xz += (sin(fractal(point * .2) * 2.4 + waveCenter - Time * 10.0) * .3 - .3) * (1.0 - saturate(waveCenter * .05));

	vec3 refl = reflect(rayDirection, normalize(normal));
	refl.y = abs(refl.y) * .85;
 
	return reflections(point, normalize(refl), 100.f);
}