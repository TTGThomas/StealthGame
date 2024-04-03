#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "../../quadRenderer/RenderQuad.h"
#include "../../quadRenderer/QuadRenderer.h"
#include "../../Collisions/CollisionDetector.h"

#include "../GlobalData.h"

#include "../../UUID.h"

struct QuadInitDesc
{
	glm::vec2 m_pos{};
	glm::vec2 m_scale{};
	float m_depth = 0.0f;
	uint64_t m_shaderUUID = 0;
	uint64_t m_textureUUID = 0;
};

class Object
{
public:
	Object() = default;
	Object(std::vector<QuadInitDesc>& descs);

	virtual void Init(std::vector<QuadInitDesc>& descs);

	Quad* GetQuad(int index) { return &GlobalData::Get().m_scene->GetQuads()[m_uuids[index].GetUUID()]; }
	const std::vector<UUID>& GetUUIDs() { return m_uuids; }
	UUID& GetUUID(int index) { return m_uuids[index]; }
protected:
	std::vector<UUID> m_uuids;
};