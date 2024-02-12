#version 330 core

out vec4 fragColor;

in vec2 v_texCoord;

uniform sampler2D u_texture;

uniform int u_selected;

void main()
{
	vec4 texColor = texture(u_texture, v_texCoord);
	fragColor = texColor;
	//fragColor = vec4(1.0f, 0.0f, 1.0f, 1.0f);
}