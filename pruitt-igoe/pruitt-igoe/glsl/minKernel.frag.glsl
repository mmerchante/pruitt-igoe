#version 150

#define COHERENCE

uniform sampler2D SourceTexture;
uniform float Time;
uniform vec4 SourceTextureSize;

in vec2 uv;

out vec4 out_Col;

void main()
{

#ifdef COHERENCE
    vec4 tex = textureLod(SourceTexture, uv, 0);

	float d = tex.r;
	for(int i = 0; i < 1; i++)
	{ 
		int lod = 0 + i;
		vec3 pixelSize = vec3(1, -1, 0) * vec3(SourceTextureSize.z, SourceTextureSize.w, 0);

		// Cross
		d = min(d, textureLod(SourceTexture, uv + pixelSize.xz, lod).r);
		d = min(d, textureLod(SourceTexture, uv + pixelSize.yz, lod).r);
		d = min(d, textureLod(SourceTexture, uv + pixelSize.zx, lod).r);
		d = min(d, textureLod(SourceTexture, uv + pixelSize.zy, lod).r);

		// Corners
		d = min(d, textureLod(SourceTexture, uv + pixelSize.xx, lod).r);
		d = min(d, textureLod(SourceTexture, uv + pixelSize.xy, lod).r);
		d = min(d, textureLod(SourceTexture, uv + pixelSize.yy, lod).r);
		d = min(d, textureLod(SourceTexture, uv + pixelSize.yx, lod).r);
	}
#else
	float d = 0.f;
#endif

    out_Col = vec4(d);
}
