#version 330 core

layout(location = 0)in vec3 a_pos;
layout(location = 1)in vec2 a_texCoord;

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

	//vec3 pos = vec3(((a_pos * u_scale) + u_pos) * vec2(u_cameraZoom), 0.0f);
	vec3 pos = (u_matrix * vec4(a_pos.xy, u_depth, 1.0f)).xyz;
	pos.x *= u_screenRatio;
	gl_Position = vec4(pos, 1.0f);
}