#pragma once

#include <vector>

#include "../quadRenderer/RenderQuad.h"

#include "../UUID.h"
#include "../Scene.h"

#include "AABB.h"

struct CollisionPayload
{
	bool m_hasHit;
	UUID m_uuid;
};

class CollisionDetector
{
public:
	CollisionDetector(class Scene* parent)
		: m_parent(parent) {}

	CollisionPayload Collide(glm::vec2 point);
	CollisionPayload Collide(glm::vec2 l0, glm::vec2 l1);
	CollisionPayload Collide(AABB& aabb);
	CollisionPayload Collide(UUID& uuid);
private:
	Scene* m_parent = nullptr;
};