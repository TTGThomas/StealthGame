#pragma once

#include "../../../input/KeyBoard.h"
#include "../../../input/Mouse.h"

#include "../../Engine/Projectile.h"

#include "InventoryItem.h"

class Gun : public InventoryItem
{
public:
	virtual void OnEquip() override;
	virtual void OnUnequip() override;
	virtual void OnEquipping() override;
	virtual void OnResize(int x, int y) override;

	static void ClearResources();
	
	bool IsShooting() { return m_isShooting; }

	static uint64_t GetTexHUD() { return m_texHUD; }
	static uint64_t GetTexInGame() { return m_texInGame; }
private:
	float AngleFromPoint(glm::vec2 start, glm::vec2 end);
private:
	static uint64_t m_texHUD;
	static uint64_t m_texInGame;
	bool m_isShooting = false;

	glm::vec2 m_HUDpos;

	glm::vec2 m_windowScale;
};

