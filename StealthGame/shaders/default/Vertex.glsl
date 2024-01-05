#version 330 core

layout(location = 0)in vec2 a_pos;
layout(location = 1)in vec2 a_texCoord;

out vec2 v_texCoord;

uniform vec2 u_pos;
uniform vec2 u_scale;

void main()
{
	v_texCoord = a_texCoord;
	gl_Position = vec4((a_pos * u_scale) + u_pos, 0.0f, 1.0f);
}