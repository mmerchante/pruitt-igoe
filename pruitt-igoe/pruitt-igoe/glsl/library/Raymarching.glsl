// This file includes the main raymarching code, reused in all shaders.
// The shader assumes a constant material.
// It has the main function, and assumes the existence of certain functions, such as 

// - shade:  the output color
vec3 shade(vec3 point, vec3 normal, vec3 rayOrigin, vec3 rayDirection, float t);

// - scene: the raymarched sdf.
float scene(vec3 point);

#define saturate(x) clamp(x, 0.0, 1.0)

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

float sdCappedCylinder( vec3 p, vec2 h)
{
  vec2 d = abs(vec2(length(p.xz),p.y)) - h;
  return min(max(d.x,d.y),0.0) + length(max(d,0.0));
}

vec3 debugIterations(float factor)
{
	vec3 a = vec3(0.478, 0.500, 0.500);
	vec3 b = vec3(0.500);
	vec3 c = vec3(0.688, 0.748, 0.748);
	vec3 d = vec3(0.318, 0.588, 0.908);

	return palette(factor, a, b, c, d);
}

vec3 estimateSceneGradient(vec3 point, float epsilon)
{
	vec3 e = vec3(epsilon, -epsilon, 0);

	float x = scene(vec3(point.x + epsilon, point.y, point.z));
	x -= scene(vec3(point.x - epsilon, point.y, point.z));

	float y = scene(vec3(point.x, point.y + epsilon, point.z));
	y -= scene(vec3(point.x, point.y - epsilon, point.z));

	float z = scene(vec3(point.x, point.y, point.z + epsilon));
	z -= scene(vec3(point.x, point.y, point.z - epsilon));

	return normalize(vec3(x,y,z));
}

// TODO: Reintroduce
#ifdef TEMPORAL_REPROJECTION
uniform sampler2D FeedbackBuffer;

// Temporal reprojection
float SamplePreviousFrame(vec2 uv)
{
	float d = min(texture2D(FeedbackBuffer, uv).r, textureLod(FeedbackBuffer, uv, 3).r);	
	return d;
}
#endif

uniform mat4 Model;
uniform mat4 ModelInv;
uniform mat4 ModelInvTr;

uniform vec4 CameraPosition;
uniform vec4 CameraParameters;

struct VertexData
{
	vec3 localVertexPosition;
	vec3 localCameraPosition;
};

in VertexData vertexData;

layout(location = 0) out vec4 out_Col;

void main()
{
	vec3 rayOrigin = vertexData.localCameraPosition;
	vec3 toCamera = vertexData.localVertexPosition - rayOrigin;
	vec3 rayDir = normalize(toCamera);
	float depth = length(toCamera);
		
    vec3 color = vec3(0.0);
    vec3 current = rayOrigin + rayDir * depth;

    float t = depth;
	float d = CameraParameters.y;

	bool hit = false;

#ifdef DEBUG
    float iterationCount = 0.0;
#endif

	for(int j = 0; j < MAX_ITERATIONS; j++)
	{
		d = scene(current);

		if(d < EPSILON)
		{
			hit = true;
			break;
		}

		t += d;
		current += rayDir * d;

#ifdef DEBUG
		iterationCount += 1.0;
#endif

		// Far clip
		if(t >= CameraParameters.y)
			break;
	}

	if(hit)
	{
		// More details in intersections (similar to a discontinuity reduction)
		// This GREATLY improves, for example, the gradient estimation for 
		// big discontinuities such as box edges
		for(int k = 0; k < SECONDARY_ITERATIONS; k++)
		{
			d = scene(current);
	
			if(d <= 0)
				break;

			t += d;
			current += rayDir * d;
		}

#ifdef DEBUG
		vec3 debugColor = debugIterations(iterationCount / float(MAX_ITERATIONS));
		out_Col = vec4(debugColor, 1.0);
#else
		// Local normal
		vec3 normal = estimateSceneGradient(current, NORMAL_ESTIMATION_EPSILON);

		// Transform into world space again
		current = (Model * vec4(current, 1.0)).xyz;
		t = length(CameraPosition.xyz - current);
		normal = normalize((ModelInvTr * vec4(normal, 0.0)).xyz);

		color = shade(current, normal, rayOrigin, rayDir, t);
		out_Col = vec4(color, 1.0);
#endif
	}
	else
	{
		// Make fragment far clip
		t = CameraParameters.y;
	}

	gl_FragDepth = (t - CameraParameters.x) / (CameraParameters.y - CameraParameters.x);
}
