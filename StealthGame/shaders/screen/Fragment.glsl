#version 440 core

out vec4 fragColor;

in vec2 v_texCoord;

uniform sampler2D u_texture;

vec3 SamplePoint(vec2 texCoord);


float kernel[9] = float[]
(
// normal
	0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f
// Edge detection
//	1.0f,  1.0f, 1.0f,
//	1.0f, -8.0f, 1.0f,
//	1.0f,  1.0f, 1.0f
// gaussian blur
//	1.0f, 2.0f, 1.0f,
//	2.0f, 4.0f, 2.0f,
//	1.0f, 2.0f, 1.0f
);
float invDiv = 1.0f / 1.0f;

float offset = 1.0f / 300.0f;

vec2 sampleOffset[9] = vec2[]
(
	vec2(-offset,  offset), vec2(0.0f,  offset), vec2( offset,  offset),
	vec2(-offset,    0.0f), vec2(0.0f,    0.0f), vec2( offset,    0.0f),
	vec2(-offset, -offset), vec2(0.0f, -offset), vec2(-offset, -offset)
);

void main()
{
	vec3 color = vec3(0.0f);
	for (int i = 0; i < 9; i++)
		color += SamplePoint(v_texCoord + sampleOffset[i]) * kernel[i] * invDiv;

	fragColor = vec4(color, 1.0f);
}

vec3 SamplePoint(vec2 texCoord)
{
	float t = 0.001f;
	texCoord = clamp(texCoord, 0.0f + t, 1.0f - t);
	
	return texture(u_texture, texCoord).rgb;
}