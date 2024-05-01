#version 440 core

layout(location = 0)in vec3 a_pos;
layout(location = 1)in vec2 a_texCoord;
layout(location = 2)in mat4 a_matrix;

out vec2 v_texCoord;

uniform float u_screenRatio;

uniform mat4 u_matrix;

uniform float u_depth;

uniform int u_sideFrames;
uniform int u_frameIndex;

void main()
{
	// u_frameIndex = yIndex * u_sideFrames + xIndex;
	int xIndex = u_frameIndex % u_sideFrames;
	int yIndex = (u_frameIndex - xIndex) / u_sideFrames;

	float invN = 1.0f / float(u_sideFrames);
	v_texCoord = a_texCoord * vec2(invN, invN) + vec2(float(xIndex) * invN, float(yIndex) * invN);


	vec3 pos = (u_matrix * vec4(a_pos.xy, u_depth, 1.0f)).xyz;
	pos.x *= u_screenRatio;
	gl_Position = vec4(pos, 1.0f);
}