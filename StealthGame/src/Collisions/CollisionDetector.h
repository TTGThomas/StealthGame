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

	CollisionPayload Collide(glm::vec2 point, bool ignoreDisabled = true);
	CollisionPayload Collide(glm::vec2 l0, glm::vec2 l1, bool ignoreDisabled = true);
	CollisionPayload Collide(AABB& aabb, bool ignoreDisabled = true);
	CollisionPayload Collide(UUID& uuid, bool ignoreDisabled = true);
private:
	Scene* m_parent = nullptr;
};