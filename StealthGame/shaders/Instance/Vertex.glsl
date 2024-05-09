#version 440 core

layout(location = 0)in vec3 a_pos;
layout(location = 1)in vec2 a_texCoord;
layout(location = 2)in mat4 a_matrix;
layout(location = 6)in vec2 a_texDimension;
layout(location = 7)in float a_texIndex;
layout(location = 8)in float a_alpha;
layout(location = 9)in float a_depth;
layout(location = 10)in float a_sideFrames;
layout(location = 11)in float a_frameIndex;
layout(location = 12)in float a_isGround;

out vec2 v_texCoord;
out float v_texIndex;
out float v_alpha;
out float v_isGround;

uniform float u_screenRatio;

void main()
{
	if (a_isGround == 0.0f)
	{
		// frameIndex = yIndex * sideFrames + xIndex;
		int sideFrames = int(a_sideFrames);
		int frameIndex = int(a_frameIndex);

		int xIndex = frameIndex % sideFrames;
		int yIndex = (frameIndex - xIndex) / sideFrames;
	
		float invN = 1.0f / float(sideFrames);
		v_texCoord = a_texCoord * vec2(invN, invN) + vec2(float(xIndex) * invN, float(yIndex) * invN);
		v_texCoord *= a_texDimension / vec2(256.0f);
	}
	else
	{
		v_texCoord = a_texCoord;
	}

	v_texIndex = a_texIndex;
	v_alpha = a_alpha;
	v_isGround = a_isGround;

	vec4 pos = a_matrix * vec4(a_pos.xy, a_depth, 1.0f);
	//pos.x += float(gl_InstanceID) / 10.0f;
	
	pos.x *= u_screenRatio;
	gl_Position = pos;
}