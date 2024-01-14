#pragma once

#include <vector>

#include "../quadRenderer/Quad.h"

struct CollisionPayload
{
	bool m_hasHit;
	int m_hitIndex;
};

class CollisionDetector
{
public:
	void BindMap(std::vector<Quad>* map);

	void DisableAABBs(std::vector<int> indices);

	CollisionPayload Collide(glm::vec2 point);
	CollisionPayload Collide(glm::vec2 l0, glm::vec2 l1);
	CollisionPayload Collide(AABB& aabb);
	CollisionPayload Collide(int index);
private:
	std::vector<Quad>* m_map = nullptr;
};