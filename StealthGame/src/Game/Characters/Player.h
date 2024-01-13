#pragma once

#include <glm/glm.hpp>

#include "../../input/KeyBoard.h"
#include "../../input/Mouse.h"

#include "../Engine/Entity.h"

#include "NPC.h"

#include "../Desc.h"

// fist quad of the list acts as the hit box
// second quad of the list acts as the cursor
// third quad of the list acts as the character
// quads that are further down the list will be ignored
class Player : public Entity
{
public:
	Player() = default;

	void BindCamera(Camera* camera);
	void BindNPCs(std::vector<NPC>* npcs);

	void PlayerTick(GameTickDesc& desc);

	void SetPos(glm::vec2 newPos) { GetQuad(0)->SetPos(newPos); }
	void ChangePos(glm::vec2 pos) { GetQuad(0)->ChangePos(pos); }
	void SetInputEnabled(bool newInput) { m_inputEnabled = newInput; }
	void SetIsCrouching(bool newInput) { m_isCrouching = newInput; }
	
	const glm::vec2& GetPos() { return GetQuad(0)->GetPos(); }
	bool GetInputEnabled() { return m_inputEnabled; }
	bool GetIsCrouching() { return m_isCrouching; }
	void EliminateNPC(NPC& victim);
private:
	void MovePlayer(GameTickDesc& desc);
private:
	bool m_inputEnabled = true;
	Camera* m_bindedCamera = nullptr;
	std::vector<NPC>* m_npcs = nullptr;

	bool m_isCrouching = false;

	constexpr static glm::vec2 m_normalScale = { 0.2f, 0.2f };
	constexpr static glm::vec2 m_crouchScale = { 0.2f, 0.15f };

	constexpr static float m_normalSpeed = 1.5f;
	constexpr static float m_crouchSpeed = 0.9f;
};