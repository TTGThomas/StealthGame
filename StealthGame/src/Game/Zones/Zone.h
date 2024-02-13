#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "../../Collisions/AABB.h"

class Zone
{
public:
	Zone() = default;
	Zone(std::vector<AABB>& aabbs)
		: m_zones(aabbs) {}

	void AddAABB(AABB aabb) { m_zones.emplace_back(aabb); }

	bool IsPointInZone(glm::vec2 point);
private:
	std::vector<AABB> m_zones;
};

