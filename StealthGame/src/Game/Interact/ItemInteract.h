#pragma once

#include "Interaction.h"

#include "../Items/Item.h"

class ItemInteract : public Interaction
{
public:
	ItemInteract(class GameScene* scene, Item* item)
		: m_scene(scene), m_item(item) {}

	virtual void OnInteract() override;

	// returns where you want the cursor to be at
	virtual glm::vec2 OnTick(bool* show) override;
private:
	Item* m_item = nullptr;
	GameScene* m_scene = nullptr;
};

