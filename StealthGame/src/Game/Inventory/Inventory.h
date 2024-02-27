#pragma once

#include <array>
#include <memory>

#include "../Desc.h"

#include "../../input/KeyBoard.h"
#include "../../input/Mouse.h"

#include "InventoryItems/InventoryItem.h"
#include "InventoryItems/Fist.h"
#include "InventoryItems/FiberWire.h"
#include "InventoryItems/Gun.h"

class Inventory
{
public:
	Inventory();

	template <typename T>
	void AddItem(int slot)
	{
		m_items[slot] = std::make_shared<T>();
	}

	void GiveEverything();

	void InventoryTick(GameTickDesc& desc);

	void EquipWithIndex(GameTickDesc& desc, int index);

	std::shared_ptr<InventoryItem> GetEquipped() { return m_items[m_equippedItem]; }
private:
	std::array<std::shared_ptr<InventoryItem>, 3> m_items{};
	int m_equippedItem = 0;
};