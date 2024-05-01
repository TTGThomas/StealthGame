#version 440 core

out vec4 fragColor;

in vec2 v_texCoord;
in float v_texIndex;
in float v_alpha;

layout(binding = 0)uniform sampler2DArray u_textureArray;
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
	vec4 texColor = texture(u_textureArray, vec3(v_texCoord, v_texIndex));
	//vec4 texColor = vec4(v_texCoord, 0.0f, 1.0f);
	float depth = gl_FragCoord.z;
	if (v_alpha != -1.0f)
	{
		texColor.w = v_alpha;
		depth = v_alpha;
	}
	return texColor;
	//return vec4(vec3(depth), 1.0f);
}