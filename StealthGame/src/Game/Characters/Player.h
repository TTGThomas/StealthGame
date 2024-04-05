#pragma once

#include <glm/glm.hpp>

#include "../../input/KeyBoard.h"
#include "../../input/Mouse.h"

#include "../Engine/Entity.h"

#include "../Animation/AnimationPlayer.h"
#include "AnimBP/PlayerAnimBP.h"

#include "../Inventory/Inventory.h"

#include "NPC.h"

#include "../Desc.h"

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

	virtual void Init(std::vector<QuadInitDesc>& descs) override;

	void ClearResources();

	void BindCamera(Camera* camera);

	void PlayerTick(GameTickDesc& desc);

	void SetPos(glm::vec2 newPos) { GetQuad(0)->SetPos(newPos); }
	void ChangePos(glm::vec2 pos) { GetQuad(0)->ChangePos(pos); }
	void SetInputEnabled(bool newInput) { m_inputEnabled = newInput; }
	void SetIsCrouching(bool newInput) { m_isCrouching = newInput; }
	void SetIsDragging(bool newInput) { m_isDragging = newInput; }
	void SetDraggedNPCID(uint64_t input) { m_draggedNPC = input; }
	void SetActionType(ActionType type) { m_actionType = type; }

	void HidePlayer(glm::vec2 pos);
	void UnHidePlayer();
	
	void OnTrespassZone() { m_actionType = ActionType::ILLEGAL; }
	void OnHostileZone() { m_actionType = ActionType::ILLEGAL; }

	const glm::vec2& GetPos() { return GetQuad(0)->GetPos(); }
	const glm::vec2& GetVelocity() { return m_velocity; }
	bool GetInputEnabled() { return m_inputEnabled; }
	bool GetIsCrouching() { return m_isCrouching; }
	bool GetIsDragging() { return m_isDragging; }
	bool GetIsHidden() { return m_isHidden; }
	UUID GetDraggedNPCID() { return m_draggedNPC; }
	void EliminateNPC(NPC& victim);
	ActionType GetActionType() { return m_actionType; }
	Inventory& GetInventory() { return m_inventory; }
private:
	void MovePlayer(GameTickDesc& desc);
private:
	PlayerAnimBP m_animBP;
	Inventory m_inventory;
	glm::vec2 m_velocity{};

	ActionType m_actionType = ActionType::NORMAL;
	bool m_inputEnabled = true;
	Camera* m_bindedCamera = nullptr;

	bool m_isCrouching = false;
	bool m_isDragging = false;
	bool m_isHidden = false;
	glm::vec2 m_posBeforeHidden = {};
	UUID m_draggedNPC = 0;

	constexpr static glm::vec2 m_normalScale = { 0.1f, 0.2f };
	constexpr static glm::vec2 m_crouchScale = { 0.1f, 0.15f };

	constexpr static float m_normalSpeed = 1.5f;
	constexpr static float m_crouchSpeed = 0.9f;
};