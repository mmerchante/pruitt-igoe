#version 330

#define FAR_CLIP 1000.0
#define MAX_ITERATIONS 5
#define SECONDARY_ITERATIONS 3
#define EPSILON 0.005

#define saturate(x) clamp(x, 0.0, 1.0)

#define COHERENCE

struct Ray {
	vec3 position;
	vec3 direction;
};

struct VertexData
{
	vec2 uv;
	vec4 ndc;
	vec3 rayDir;
	vec3 rayPos;
};

uniform float u_debug;
uniform float Time;

uniform sampler2D Heightfield;
uniform sampler2D FeedbackBuffer;

//uniform vec4 Heightfield_Size;

in VertexData vertexData;

layout(location = 0) out vec4 out_Col;

// Reference: https://github.com/stackgl/glsl-smooth-min
float smin(float a, float b, float k) 
{
  float res = exp(-k * a) + exp(-k * b);
  return -log(res) / k;
}

// Reference: http://www.iquilezles.org/www/articles/palettes/palettes.htm
vec3 palette( float t, vec3 a, vec3 b, vec3 c, vec3 d)
{
    return saturate(a + b * cos(6.28318 * ( c * t + d)));
}

vec3 debugIterations(float factor)
{
	vec3 a = vec3(0.478, 0.500, 0.500);
	vec3 b = vec3(0.500);
	vec3 c = vec3(0.688, 0.748, 0.748);
	vec3 d = vec3(0.318, 0.588, 0.908);

	return palette(factor, a, b, c, d);
}

Ray getRay(vec3 origin, vec3 dir)
{
	Ray ray;
	ray.position = origin;
	ray.direction = dir;
	return ray;
}

float terrainSDF(vec3 point)
{
	float h = textureLod(Heightfield, point.xz * .01, 0).r * 20.0;

	return (point.y - h);
}

// Assumes minDistance was initialized to FAR_CLIP
// TODO: add material types
void evaluateSceneSDF(vec3 point, out float minDistance, out float hitMaterial)
{	
	hitMaterial = 0.0;
	minDistance = FAR_CLIP;

	minDistance = min(minDistance, terrainSDF(point));

	//float farBackground = farBackgroundSDF(point);
	//minDistance = min(minDistance, farBackground);
	//hitMaterial = mix(hitMaterial, 0.0, step(abs(farBackground - minDistance), .001));
}

// This method is useful for just the distance
float evaluateSceneSDFSimple(vec3 point)
{
	float minDistance = FAR_CLIP;
	minDistance = min(minDistance, terrainSDF(point));
	return minDistance;	
}

vec3 estimateSceneGradient(vec3 point, float epsilon)
{
	vec3 e = vec3(epsilon, -epsilon, 0);
	float x = evaluateSceneSDFSimple(vec3(point.x + epsilon, point.y, point.z));
	x -= evaluateSceneSDFSimple(vec3(point.x - epsilon, point.y, point.z));

	float y = evaluateSceneSDFSimple(vec3(point.x, point.y + epsilon, point.z));
	y -= evaluateSceneSDFSimple(vec3(point.x, point.y - epsilon, point.z));

	float z = evaluateSceneSDFSimple(vec3(point.x, point.y, point.z + epsilon));
	z -= evaluateSceneSDFSimple(vec3(point.x, point.y, point.z - epsilon));

	return normalize(vec3(x,y,z));
}

void main()
{
	// Renormalize due to interpolation
	Ray ray = getRay(vertexData.rayPos, normalize(vertexData.rayDir));

    vec3 color;
    vec3 current = ray.position;

    float t = 0.0;
	float d = FAR_CLIP;
    float iterationCount = 0.0;
    float hitMaterial = 0.0;
	float bias = 1.f;

#ifdef COHERENCE
	vec4 previousFrame = textureLod(FeedbackBuffer, vertexData.uv, 0);
	t = previousFrame.r * 100.f * .95;
	current += ray.direction * t;
	bias += smoothstep(0.0, 1.0, previousFrame.b) * .1;
#endif

	for(int j = 0; j < MAX_ITERATIONS; j++)
	{
		evaluateSceneSDF(current, d, hitMaterial);

		if(d < EPSILON)
			break;

		d *= bias;

		t += d;
		current += ray.direction * d;
		iterationCount += 1.0;

		if(t >= FAR_CLIP)
			break;
	}

	//// More details in intersections (similar to a discontinuity reduction)
	//// This GREATLY improves, for example, the gradient estimation for 
	//// big discontinuities such as box edges
	//for(int k = 0; k < SECONDARY_ITERATIONS; k++)
	//{
	//	if(t >= FAR_CLIP)
	//		break;

	//	d = evaluateSceneSDFSimple(current);
	
	//	if(d <= 0.0)
	//		break;

	//	t += d;
	//	current += ray.direction * d;
	//	//iterationCount += 1.0;
	//}

	//color = shade(current, ray, t, hitMaterial);

	//// Gamma correction
	//color = pow(color, vec3(.45454));

	float normalizedIterations = iterationCount / float(MAX_ITERATIONS);
	vec3 debugColor = debugIterations(iterationCount / float(MAX_ITERATIONS + SECONDARY_ITERATIONS));
	//color = mix(color, debugColor, u_debug);

	t /= 100.f;

	//out_Col = vec4(t,abs(previousFrame.r - t),normalizedIterations, 1.0);
	out_Col = vec4(t,0,normalizedIterations, 1.0);

}