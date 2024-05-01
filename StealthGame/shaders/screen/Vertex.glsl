#version 440 core

layout(location = 0)in vec3 a_pos;
layout(location = 1)in vec2 a_texCoord;

out vec2 v_texCoord;

void main()
{
	vec2 scale = vec2(2.0f, 2.0f);
	vec2 offset = vec2(0.0f, 0.0f);

	v_texCoord = vec2(a_texCoord.x, 1.0f - a_texCoord.y);
	//gl_Position = vec4(a_pos * vec3(scale * vec2(0.5f), 1.0f) + vec3(offset, 0.0f), 1.0f);
	gl_Position = vec4(a_pos, 1.0f);
}