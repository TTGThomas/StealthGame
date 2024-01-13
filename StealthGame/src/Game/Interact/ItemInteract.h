#pragma once

#include "Interaction.h"

#include "../Items/Item.h"

class ItemInteract : public Interaction
{
public:
	ItemInteract(class Scene* scene, Item* item)
		: m_scene(scene), m_item(item) {}

	virtual void OnInteract() override;
	virtual glm::vec2 OnTick() override;
private:
	Item* m_item = nullptr;
	Scene* m_scene = nullptr;
};

