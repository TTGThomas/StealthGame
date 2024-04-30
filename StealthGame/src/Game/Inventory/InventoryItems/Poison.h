#pragma once

#include "../../../input/KeyBoard.h"
#include "../../../input/Mouse.h"

#include "../../Projectiles/CoinProj.h"

#include "InventoryItem.h"

class Poison : public InventoryItem
{
public:
	virtual void OnEquip() override;
	virtual void OnUnequip() override;
	virtual void OnEquipping() override;
	virtual void OnResize(int x, int y) override;
	virtual bool IsIllegal() override;

	static void ClearResources();
private:
	static uint64_t m_texHUD;
	static uint64_t m_texInGame;

	glm::vec2 m_windowScale;
	glm::vec2 m_HUDpos;
};

