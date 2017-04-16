#version 150

uniform sampler2D RenderTexture;

in vec2 uv;

out vec4 out_Col;

void main()
{
    vec4 tex = textureLod(RenderTexture, uv, 6);
    out_Col = tex;
}
