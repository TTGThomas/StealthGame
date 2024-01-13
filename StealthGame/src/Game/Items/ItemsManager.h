#pragma once

#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include "Item.h"

#include "../../quadRenderer/QuadRenderer.h"

class ItemsManager
{
public:
	void BindMap(QuadRenderer* map) { m_map = map; }

	void AddItem(std::shared_ptr<Item> item);
	void AddItem(std::vector<std::shared_ptr<Item>>& items);

	void DeleteItem(int index);

	int GetNearestIndex(glm::vec2 pos);
	std::shared_ptr<Item> GetNearestItem(glm::vec2 pos);

	std::shared_ptr<Item> GetItem(int index) { return m_items[index]; }
	std::vector<std::shared_ptr<Item>>& GetItems() { return m_items; }
private:
	std::vector<std::shared_ptr<Item>> m_items;
	QuadRenderer* m_map;
};

