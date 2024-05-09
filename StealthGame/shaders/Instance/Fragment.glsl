#version 440 core

out vec4 fragColor;

in vec2 v_texCoord;
in float v_texIndex;
in float v_alpha;
in float v_isGround;

layout(binding = 0)uniform sampler2DArray u_textureArray;
layout(binding = 1)uniform sampler2D u_foreTex;
layout(binding = 2)uniform sampler2D u_backTex;

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
	vec4 texColor = vec4(1.0f, 0.0f, 1.0f, 1.0f);
	if (v_isGround == 0.0f)
		texColor = texture(u_textureArray, vec3(v_texCoord, v_texIndex));
	else if (v_isGround == 1.0f)
		texColor = texture(u_foreTex, v_texCoord);
	else if (v_isGround == 2.0f)
		texColor = texture(u_backTex, v_texCoord);

	float depth = gl_FragCoord.z;
	if (v_alpha != -1.0f)
	{
		texColor.w = v_alpha;
		depth = v_alpha;
	}
	return texColor;
}