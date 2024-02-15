#pragma once

#include "../Scene.h"

#include "../quadRenderer/RenderQuad.h"
#include "../quadRenderer/QuadRenderer.h"
#include "../quadRenderer/Texture.h"

#include "GlobalData.h"

#include "Desc.h"

class VisualLayers
{
public:
	VisualLayers() = default;

	void Init(GameTickDesc& desc, uint64_t backgroundTexID, uint64_t foregroundTexID);
private:
	uint64_t AddQuad(GameTickDesc& desc, float depth, uint64_t texID);
private:
	UUID m_foreground;
	UUID m_background;
};