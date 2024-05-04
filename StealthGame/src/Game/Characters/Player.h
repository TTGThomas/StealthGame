#pragma once

#include <glm/glm.hpp>
#include <imgui.h>

#include "../../input/KeyBoard.h"
#include "../../input/Mouse.h"

#include "../Engine/Entity.h"

#include "../Animation/AnimationPlayer.h"
#include "AnimBP/PlayerAnimBP.h"

#include "../Inventory/Inventory.h"

#include "NPC.h"

#include "../Desc.h"
#include "../PassData.h"

// fist quad of the list acts as the hit box
// second quad of the list acts as the cursor
// third quad of the list acts as the character
// quads that are further down the list will be ignored
class Player : public Entity
{
public:
	enum class ActionType
	{
		NORMAL, ILLEGAL
	};
public:
	Player();

	virtual void Init(std::vector<QuadInitDesc>& descs, bool isSpecial = false, bool isDoor = false) override;

	void ClearResources();

	void BindCamera(Camera* camera);

	void PlayerTick(GameTickDesc& desc);

	PassData GetPassData();

	void ShowWindow();

	void SetPos(glm::vec2 newPos) { GetQuad(0)->SetPos(newPos); }
	void ChangePos(glm::vec2 pos) { GetQuad(0)->ChangePos(pos); }
	void SetInputEnabled(bool newInput) { m_inputEnabled = newInput; }
	void SetIsCrouching(bool newInput) { m_isCrouching = newInput; }
	void SetIsDragging(bool newInput) { m_isDragging = newInput; }
	void SetDraggedNPCID(uint64_t input) { m_draggedNPC = input; }
	void SetActionType(ActionType type) { m_actionType = type; }
	void SetHealth(int health) { m_health = health; }

	void ChangeDisguise(Identities type);

	void HidePlayer(glm::vec2 pos);
	void UnHidePlayer();
	
	bool OnTrespassZone();
	bool OnHostileZone();

	bool IsGunShooting();

	const glm::vec2& GetPos() { return GetQuad(0)->GetPos(); }
	const glm::vec2& GetVelocity() { return m_velocity; }
	bool GetInputEnabled() { return m_inputEnabled; }
	bool GetIsCrouching() { return m_isCrouching; }
	bool GetIsDragging() { return m_isDragging; }
	bool GetIsHidden() { return m_isHidden; }
	GameUUID GetDraggedNPCID() { return m_draggedNPC; }
	void EliminateNPC(NPC& victim);
	ActionType GetActionType() { return m_actionType; }
	Inventory& GetInventory() { return m_inventory; }
	Identities GetDisguise() { return m_disguise; }
	int GetHealth() { return m_health; }
private:
	void MovePlayer(GameTickDesc& desc);
	void ApplyDamage();
private:
	int m_health = 100;

	PlayerAnimBP m_animBP;
	Inventory m_inventory;
	glm::vec2 m_velocity{};
	Identities m_disguise = Identities::STANDARD;

	ActionType m_actionType = ActionType::NORMAL;
	bool m_inputEnabled = true;
	Camera* m_bindedCamera = nullptr;

	bool m_isCrouching = false;
	bool m_isDragging = false;
	bool m_isHidden = false;
	glm::vec2 m_posBeforeHidden = {};
	GameUUID m_draggedNPC = 0;

	constexpr static glm::vec2 m_normalScale = { 0.1f, 0.2f };
	constexpr static glm::vec2 m_crouchScale = { 0.1f, 0.15f };

	constexpr static float m_normalSpeed = 1.5f;
	constexpr static float m_crouchSpeed = 0.9f;
};