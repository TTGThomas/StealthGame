#version 330 core

layout(location = 0)in vec2 a_pos;
layout(location = 1)in vec2 a_texCoord;

out vec2 v_texCoord;

uniform float u_screenRatio;

uniform vec2 u_pos;
uniform vec2 u_scale;
uniform float u_cameraZoom;

void main()
{
	v_texCoord = a_texCoord;
	vec2 pos = ((a_pos * u_scale) + u_pos) * vec2(u_cameraZoom);
	pos.x *= u_screenRatio;
	gl_Position = vec4(pos, 0.0f, 1.0f);
}