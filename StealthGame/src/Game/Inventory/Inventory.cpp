#include "Inventory.h"

Inventory::Inventory()
{
}

void Inventory::GiveEverything()
{
	AddItem<Fist>(0);
	AddItem<FiberWire>(1);
	AddItem<Gun>(2);
	GetEquipped()->OnEquip();
}

void Inventory::InventoryTick(GameTickDesc& desc)
{
	if (KeyBoard::IsKeyPressDown(GLFW_KEY_1))
		EquipWithIndex(desc, 0);
	if (KeyBoard::IsKeyPressDown(GLFW_KEY_2))
		EquipWithIndex(desc, 1);
	if (KeyBoard::IsKeyPressDown(GLFW_KEY_3))
		EquipWithIndex(desc, 2);

	GetEquipped()->OnEquipping();
}

void Inventory::EquipWithIndex(GameTickDesc& desc, int index)
{
	GetEquipped()->OnUnequip();
	m_equippedItem = index;
	GetEquipped()->OnEquip();
}
