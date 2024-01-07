#version 330 core

out vec4 fragColor;

in vec2 v_texCoord;

uniform sampler2D u_texture;

uniform int u_selected;

void main()
{
	vec4 texColor = texture(u_texture, v_texCoord);
	if (u_selected == 0)
		fragColor = texColor;
	else
		fragColor = texColor + vec4(-0.2f);
}