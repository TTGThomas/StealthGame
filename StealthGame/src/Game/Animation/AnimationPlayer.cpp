#include "AnimationPlayer.h"

std::unordered_map<std::string, uint64_t> AnimationPlayer::m_texUUIDs;

void AnimationPlayer::ClearResource()
{
	m_texUUIDs.clear();
}

void AnimationPlayer::ResetAnimationCursor(uint64_t uuid)
{
	GlobalData& gData = GlobalData::Get();
	RenderQuad& renderQuad = gData.m_scene->GetRenderQuads()[uuid];
	renderQuad.SetFrameIndex(0.0f);
}

void AnimationPlayer::SetAnimationAtlas(uint64_t uuid, const char* atlasPath)
{
	GlobalData& gData = GlobalData::Get();

	RenderQuad& renderQuad = gData.m_scene->GetRenderQuads()[uuid];
	uint64_t texUUID = 0;

	auto pair = m_texUUIDs.find(std::string(atlasPath));
	if (pair != m_texUUIDs.end())
	{
		texUUID = pair->second;
	}
	else
	{
		Texture texture(atlasPath);
		texUUID = texture.GetUUID().GetUUID();
		gData.m_renderer->AddTexture(texture);

		m_texUUIDs[std::string(atlasPath)] = texUUID;
	}

	renderQuad.SetTextureUUID(texUUID);
}

void AnimationPlayer::PlayAnimation(uint64_t uuid, float framesPerSecond, int sideFrames, int frames)
{
	GlobalData& gData = GlobalData::Get();
	RenderQuad& renderQuad = gData.m_scene->GetRenderQuads()[uuid];

	renderQuad.SetSideFrames(sideFrames);
	float frameIndex = renderQuad.GetFrameIndex();
	
	frameIndex += gData.m_deltaTime * framesPerSecond;
	if (frameIndex >= frames)
		frameIndex -= frames;

	renderQuad.SetFrameIndex(frameIndex);
}