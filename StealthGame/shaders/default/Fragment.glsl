#version 330 core

out vec4 fragColor;

in vec2 v_texCoord;

uniform sampler2D u_texture;

uniform float u_alpha;

uniform int u_useTexture;

vec4 MainGetColor();

void main()
{
	fragColor = MainGetColor();
	if (u_useTexture == 0)
	{
		if (fragColor.w > 0.5f)
			fragColor = vec4(1.0f, 0.0f, 1.0f, 1.0f);
	}
}

vec4 MainGetColor()
{
	vec4 texColor = texture(u_texture, v_texCoord);
	float depth = gl_FragCoord.z;
	if (u_alpha != -1.0f)
	{
		texColor.w = u_alpha;
		depth = u_alpha;
	}
	return texColor;
	//return vec4(vec3(depth), 1.0f);
}