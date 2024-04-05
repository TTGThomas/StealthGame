#pragma once

#include <unordered_map>
#include <string>

#include "../../quadRenderer/RenderQuad.h"
#include "../../quadRenderer/QuadRenderer.h"
#include "../../quadRenderer/Texture.h"

#include "../GlobalData.h"

class AnimationPlayer
{
public:
	static void ClearResource();

	static void ResetAnimationCursor(uint64_t uuid);
	static void SetAnimationAtlas(uint64_t uuid, const char* atlasPath);
	static void PlayAnimation(uint64_t uuid, float framesPerSecond, int sideFrames, int frames);
private:
	// TODO: deprecate std::string
	static std::unordered_map<std::string, uint64_t> m_texUUIDs;
};