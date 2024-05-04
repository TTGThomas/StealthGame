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
	Object(std::vector<QuadInitDesc>& descs, bool isSpecial, bool isDoor);

	virtual void Init(std::vector<QuadInitDesc>& descs, bool isSpecial, bool isDoor);

	Quad* GetQuad(int index) { return &GlobalData::Get().m_scene->GetQuads()[m_uuids[index].GetUUID()]; }
	const std::vector<GameUUID>& GetUUIDs() { return m_uuids; }
	GameUUID& GetUUID(int index) { return m_uuids[index]; }
protected:
	std::vector<GameUUID> m_uuids;
};