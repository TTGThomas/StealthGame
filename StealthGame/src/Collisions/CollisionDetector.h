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

// collision layer 0: map collision
// collision layer 1: NPC collision
// Collision layer 2: damager collision
class CollisionDetector
{
public:
	CollisionDetector(class Scene* parent)
		: m_parent(parent) {}

	void ClearResources();

	void SetLayers(int numLayers);
	void AddToLayer(int layer, uint64_t aabb);

	CollisionPayload Collide(int layer, glm::vec2 point, bool ignoreDisabled = true);
	CollisionPayload Collide(int layer, glm::vec2 l0, glm::vec2 l1, bool ignoreDisabled = true);
	CollisionPayload Collide(int layer, AABB& aabb, bool ignoreDisabled = true);
	CollisionPayload Collide(int layer, UUID& uuid, bool ignoreDisabled = true);
private:
	Scene* m_parent = nullptr;
	std::vector<std::vector<uint64_t>> m_layers;
};