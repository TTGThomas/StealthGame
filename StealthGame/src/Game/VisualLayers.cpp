#include "VisualLayers.h"

void VisualLayers::Init(GameTickDesc& desc)
{
	// background = -0.1f
	// foreground = 0.99f
	AddQuad(desc, 1.0f, 0.99f, 0);
	AddQuad(desc, 2.0f, -0.1f, 0);
}

uint64_t VisualLayers::AddQuad(GameTickDesc& desc, float ground, float depth, uint64_t texID)
{
	Scene* scene = desc.m_scene;
	QuadRenderer* renderer = desc.m_renderer;

	float rate = MAP_SCALE / 32.0f;
	float size = 1024.0f;
	
	// 32 px for 0.45
	// 1 px for 0.45 / 32
	Quad quad;
	uint64_t uuid = quad.GetUUID().GetUUID();
	glm::vec2 radius = { size * rate, size * rate };
	quad.SetRadius(radius);
	quad.SetPos({ radius.x, -radius.y });
	uint64_t ret = uuid;
	RenderQuadInitDesc renderDesc;
	renderDesc.m_followCameraOffset = true;
	renderDesc.m_depth = depth;
	renderDesc.m_shaderUUID = GlobalData::Get().m_defaultShader;
	renderDesc.m_textureUUID = texID;

	scene->AddQuad(quad, renderDesc);
	scene->GetRenderQuads()[uuid].UpdateRenderQuad(scene, uuid);
	scene->GetRenderQuads()[uuid].SetIsGround(ground);
	scene->GetAABBs()[uuid].SetEnabled(false);
	return ret;
}
