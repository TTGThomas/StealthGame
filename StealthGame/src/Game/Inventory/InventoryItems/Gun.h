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

	static void ProjectileTick(GameTickDesc& desc);

	static void ClearResources();
private:
	float AngleFromPoint(glm::vec2 start, glm::vec2 end);
private:
	static uint64_t m_texHUD;
	static uint64_t m_texInGame;

	static std::vector<Projectile> m_projectiles;

	glm::vec2 m_HUDpos;

	glm::vec2 m_windowScale;
};

