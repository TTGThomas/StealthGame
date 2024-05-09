#pragma once

#include "../Scene.h"

#include "../quadRenderer/RenderQuad.h"
#include "../quadRenderer/QuadRenderer.h"
#include "../quadRenderer/Texture.h"

#include "SceneLoader/SceneConfig.h"

#include "GlobalData.h"

#include "Desc.h"

class VisualLayers
{
public:
	VisualLayers() = default;

	void Init(GameTickDesc& desc);
private:
	uint64_t AddQuad(GameTickDesc& desc, float ground, float depth, uint64_t texID);
private:
	GameUUID m_foreground;
	GameUUID m_background;
};