#version 330 core

out vec4 fragColor;

in vec2 v_texCoord;

uniform sampler2D u_texture;

uniform int u_selected;

uniform float u_alpha;

uniform int u_useTexture;

void main()
{
	vec4 texColor = texture(u_texture, v_texCoord);
	if (u_alpha != -1.0f)
		texColor.w = u_alpha;
	if (u_useTexture == 0)
	{
		if (texColor.w > 0.5f)
			fragColor = vec4(1.0f, 0.0f, 1.0f, 1.0f);
	}
	else
	{
		fragColor = texColor;
	}
}