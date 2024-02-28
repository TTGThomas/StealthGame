#pragma once

#include <array>
#include <memory>

#include "../Desc.h"

#include "../../input/KeyBoard.h"
#include "../../input/Mouse.h"

#include "../GlobalData.h"

#include "InventoryItems/InventoryItem.h"
#include "InventoryItems/Fist.h"
#include "InventoryItems/FiberWire.h"
#include "InventoryItems/Gun.h"

class Inventory
{
public:
	enum class Type
	{
		FIST, FIBERWIRE, GUN
	};
public:
	Inventory();

	template <typename T>
	void AddItem(int slot)
	{
		m_items[slot] = std::make_shared<T>();
	}
	
	void ClearResources();
	
	void GiveEverything();

	void InventoryTick(GameTickDesc& desc);
	void OnResize(int x, int y);

	void EquipWithIndex(GameTickDesc& desc, int index);

	std::shared_ptr<InventoryItem> GetEquipped() { return m_items[m_equippedItem]; }
	Type GetEquippiedType() { return (Type)m_equippedItem; }
private:
	std::array<std::shared_ptr<InventoryItem>, 3> m_items{};
	int m_equippedItem = 0;

	std::pair<bool, glm::vec<2, int>> m_resize;
};