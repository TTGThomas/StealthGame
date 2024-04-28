#pragma once

#include "vector"

#include <glm/glm.hpp>

#include "../../../quadRenderer/Texture.h"

#include "../../Desc.h"
#include "../../GlobalData.h"

#include "../../../UUID.h"

class InventoryItem
{
public:
	virtual void OnEquip() {}
	virtual void OnUnequip() {}
	virtual void OnEquipping() {}
	virtual void OnResize(int x, int y) {}
	virtual bool IsIllegal() { return false; }

	Quad& GetQuad(int index) { return GlobalData::Get().m_scene->GetQuads()[GetUUID(index)]; }
	uint64_t GetUUID(int index) { return m_uuids[index].GetUUID(); }
	std::vector<GameUUID>& GetUUIDs() { return m_uuids; }
protected:
	uint64_t CreateTexture(const char* path)
	{
		GlobalData gData = GlobalData::Get();

		Texture texture;
		texture.Init(path);
		uint64_t ret = texture.GetUUID().GetUUID();
		gData.m_renderer->AddTexture(texture);
		return ret;
	}

	void AddQuad(glm::vec2 pos, glm::vec2 radius, bool followCameraOffset, uint64_t texture)
	{
		GlobalData gData = GlobalData::Get();

		Quad quad;
		quad.SetPos(pos);
		quad.SetRadius(radius);
		uint64_t uuid = quad.GetUUID().GetUUID();

		RenderQuadInitDesc renderDesc;
		renderDesc.m_followCameraOffset = followCameraOffset;
		renderDesc.m_depth = 0.91f;
		renderDesc.m_shaderUUID = gData.m_defaultShader;
		renderDesc.m_textureUUID = texture;

		gData.m_scene->AddQuad(quad, renderDesc);
		gData.m_scene->GetRenderQuads()[uuid].UpdateRenderQuad(gData.m_scene, uuid);
		gData.m_scene->GetAABBs()[uuid].SetEnabled(false);

		m_uuids.emplace_back(uuid);
	}
protected:
	std::vector<GameUUID> m_uuids;
};