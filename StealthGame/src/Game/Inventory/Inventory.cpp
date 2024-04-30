#include "Inventory.h"

#include "../GameScene.h"

Inventory::Inventory()
{
}

void Inventory::ClearResources()
{
	Fist::ClearResources();
	FiberWire::ClearResources();
	Coin::ClearResources();
	Gun::ClearResources();
}

void Inventory::GiveEverything()
{
	AddItem<Fist>(0);
	AddItem<FiberWire>(1);
	AddItem<Coin>(2);
	AddItem<Gun>(3);
	AddItem<Poison>(4);
	GetEquipped()->OnEquip();
}

void Inventory::InventoryTick(GameTickDesc& desc)
{
	if (GlobalData::Get().m_gameScene->GetPlayer().GetIsHidden())
	{
		EquipWithIndex(desc, 0);
	}
	else
	{
		if (KeyBoard::IsKeyPressDown(GLFW_KEY_1))
			EquipWithIndex(desc, 0);
		if (KeyBoard::IsKeyPressDown(GLFW_KEY_2))
			EquipWithIndex(desc, 1);
		if (KeyBoard::IsKeyPressDown(GLFW_KEY_3))
			EquipWithIndex(desc, 2);
		if (KeyBoard::IsKeyPressDown(GLFW_KEY_4))
			EquipWithIndex(desc, 3);
		if (KeyBoard::IsKeyPressDown(GLFW_KEY_5))
			EquipWithIndex(desc, 4);
	}

	GetEquipped()->OnEquipping();

	if (m_resize.first)
	{
		m_resize.first = false;
		m_items[0]->OnResize(m_resize.second.x, m_resize.second.y);
		m_items[1]->OnResize(m_resize.second.x, m_resize.second.y);
		m_items[2]->OnResize(m_resize.second.x, m_resize.second.y);
		m_items[3]->OnResize(m_resize.second.x, m_resize.second.y);
		m_items[4]->OnResize(m_resize.second.x, m_resize.second.y);
	}

	if (GetEquipped()->IsIllegal())
		GlobalData::Get().m_gameScene->GetPlayer().SetActionType(Player::ActionType::ILLEGAL);
}

void Inventory::OnResize(int x, int y)
{
	m_resize = { true, {x, y} };
}

void Inventory::EquipWithIndex(GameTickDesc& desc, int index)
{
	GetEquipped()->OnUnequip();
	m_equippedItem = index;
	GetEquipped()->OnEquip();
}
