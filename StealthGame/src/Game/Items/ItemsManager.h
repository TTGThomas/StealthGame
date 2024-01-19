#pragma once

#include <vector>
#include <unordered_map>
#include <memory>

#include <glm/glm.hpp>

#include "Item.h"

#include "../../quadRenderer/QuadRenderer.h"

#include "../GlobalData.h"

#include "../../UUID.h"

class ItemsManager
{
public:
	void AddItem(std::shared_ptr<Item> item);
	void AddItem(std::vector<std::shared_ptr<Item>>& items);

	void DeleteItem(uint64_t uuid);

	UUID GetNearestUUID(glm::vec2 pos);
	std::shared_ptr<Item> GetNearestItem(glm::vec2 pos);

	std::shared_ptr<Item> GetItem(uint64_t key) { return m_items[key]; }
	std::unordered_map<uint64_t, std::shared_ptr<Item>>& GetItems() { return m_items; }
private:
	std::unordered_map<uint64_t, std::shared_ptr<Item>> m_items;
};