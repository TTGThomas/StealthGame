#pragma once

#include <vector>

#include "../quadRenderer/RenderQuad.h"

#include "../UUID.h"
#include "../Scene.h"

#include "AABB.h"

struct CollisionPayload
{
	bool m_hasHit;
	GameUUID m_uuid;
};

// collision layer 0: all object collision
// collision layer 1: entity collision
// Collision layer 2: damager collision
// collision layer 3: map object collision(excludes special blocks which include door, exits, ...)
// collision layer 4: player damager collision
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
	CollisionPayload Collide(int layer, GameUUID& uuid, bool ignoreDisabled = true);
private:
	Scene* m_parent = nullptr;
	std::vector<std::vector<uint64_t>> m_layers;
};