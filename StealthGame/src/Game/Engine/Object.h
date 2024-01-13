#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "../../quadRenderer/QuadRenderer.h"
#include "../../Collisions/CollisionDetector.h"

struct QuadInitDesc
{
	glm::vec2 m_pos{};
	glm::vec2 m_scale{};
	int m_shaderIndex = 0;
	int m_textuerIndex = 0;
};

class Object
{
public:
	Object() = default;
	Object(QuadRenderer* map, std::vector<QuadInitDesc>& descs);

	inline void Init(QuadRenderer* map, std::vector<QuadInitDesc>& descs);

	Quad* GetQuad(int index) { return &m_map->GetQuads()[GetIndex(index)]; }
	const std::vector<int>& GetIndices() { return m_indices; }
	int GetIndex(int index) { return m_indices[index]; }
protected:
	QuadRenderer* m_map = nullptr;
	std::vector<int> m_indices;
};