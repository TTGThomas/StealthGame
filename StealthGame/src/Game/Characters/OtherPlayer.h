#pragma once

#include <glm/glm.hpp>
#include <imgui.h>

#include "../../input/KeyBoard.h"
#include "../../input/Mouse.h"

#include "../Engine/Entity.h"

#include "../Animation/AnimationPlayer.h"
#include "AnimBP/OtherPlayerAnimBP.h"

#include "../Inventory/Inventory.h"

#include "Player.h"
#include "NPC.h"

#include "../Desc.h"
#include "../PassData.h"

// fist quad of the list acts as the hit box
// second quad of the list acts as the cursor
// third quad of the list acts as the character
// quads that are further down the list will be ignored
class OtherPlayer : public Entity
{
public:
	OtherPlayer();

	virtual void Init(std::vector<QuadInitDesc>& descs) override;

	void ClearResources();

	void Tick(std::array<unsigned char, PATCHSIZE>& data);

	void SetPos(glm::vec2 newPos) { GetQuad(0)->SetPos(newPos); }
	void ChangePos(glm::vec2 pos) { GetQuad(0)->ChangePos(pos); }
	void SetIsCrouching(bool newInput) { m_isCrouching = newInput; }
	void SetIsDragging(bool newInput) { m_isDragging = newInput; }
	void SetDraggedNPCID(uint64_t input) { m_draggedNPC = input; }
	void SetHealth(int health) { m_health = health; }

	void HidePlayer(glm::vec2 pos);
	void UnHidePlayer();

	const glm::vec2& GetPos() { return GetQuad(0)->GetPos(); }
	bool GetIsCrouching() { return m_isCrouching; }
	bool GetIsDragging() { return m_isDragging; }
	bool GetIsHidden() { return m_isHidden; }
	float GetVelocity() { return m_velocity; }
	GameUUID GetDraggedNPCID() { return m_draggedNPC; }
	Identities GetDisguise() { return m_disguise; }
	int GetHealth() { return m_health; }
private:
	int m_health = 100;

	OtherPlayerAnimBP m_animBP;
	Identities m_disguise = Identities::STANDARD;

	Player::ActionType m_actionType = Player::ActionType::NORMAL;

	float m_velocity = {};
	float m_lastTime = 0.0f;

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