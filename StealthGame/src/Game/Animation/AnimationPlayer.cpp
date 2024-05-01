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
		texUUID = pair->second;
	else
		m_texUUIDs[std::string(atlasPath)] = gData.m_renderer->AddTexture(atlasPath);

	renderQuad.SetTextureUUID(texUUID);
}

void AnimationPlayer::PlayAnimation(uint64_t uuid, float framesPerSecond, int sideFrames, int startFrame, int endFrame)
{
	GlobalData& gData = GlobalData::Get();
	RenderQuad& renderQuad = gData.m_scene->GetRenderQuads()[uuid];

	renderQuad.SetSideFrames(sideFrames);
	float frameIndex = renderQuad.GetFrameIndex();

	if ((int)frameIndex < startFrame)
		frameIndex = startFrame;
	
	frameIndex += gData.m_deltaTime * framesPerSecond;
	if ((int)frameIndex > endFrame)
		frameIndex = startFrame;

	renderQuad.SetFrameIndex(frameIndex);
}