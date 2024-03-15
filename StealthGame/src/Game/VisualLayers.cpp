#include "VisualLayers.h"

void VisualLayers::Init(GameTickDesc& desc, uint64_t backgroundTexID, uint64_t foregroundTexID)
{
	// background = -0.1f
	// foreground = 0.99f
	if (foregroundTexID != 0)
		m_foreground = UUID(AddQuad(desc, 0.99f, foregroundTexID));
	if (backgroundTexID != 0)
		m_background = UUID(AddQuad(desc, -0.1f, backgroundTexID));
}

uint64_t VisualLayers::AddQuad(GameTickDesc& desc, float depth, uint64_t texID)
{
	Scene* scene = desc.m_scene;
	Texture& texture = desc.m_renderer->GetTextures()[texID];

	float rate = 0.015f;
	
	// 20 px for 0.6
	// 1 px for 0.6 / 20
	// rate = 0.015
	Quad quad;
	uint64_t uuid = quad.GetUUID().GetUUID();
	glm::vec2 radius = { (float)texture.GetWidth() * rate, (float)texture.GetHeight() * rate };
	quad.SetRadius(radius);
	quad.SetPos({ radius.x, -radius.y });
	uint64_t ret = uuid;
	RenderQuadInitDesc renderDesc;
	renderDesc.m_followCameraOffset = true;
	renderDesc.m_depth = depth;
	renderDesc.m_shaderUUID = GlobalData::Get().m_defaultShader;
	renderDesc.m_textureUUID = texID;

	scene->AddQuad(quad, renderDesc);
	scene->GetRenderQuads().at(uuid).UpdateRenderQuad(scene, uuid);
	scene->GetAABBs().at(uuid).SetEnabled(false);
	return ret;
}
