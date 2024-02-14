#pragma once

#include <vector>
#include <functional>

#include <glm/glm.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../../../quadRenderer/RenderQuad.h"

#include "../../GlobalData.h"

#include "../../Desc.h"

class PopUp
{
public:
	PopUp() = default;

	void Init(
		std::function<void(GameTickDesc&, PopUp*)> onCreate, 
	    std::function<void(GameTickDesc&, PopUp*, float)> onTick,
		std::function<void(GameTickDesc&, PopUp*)> onDelete
	);

	void OnCreate(GameTickDesc& desc);
	void OnTick(GameTickDesc& desc);
	void OnDelete(GameTickDesc& desc);

	Quad* GetQuad(int index) { return &GlobalData::Get().m_scene->GetQuads()[m_uuids[index].GetUUID()]; }
	const std::vector<UUID>& GetUUIDs() { return m_uuids; }
	UUID& GetUUID(int index) { return m_uuids[index]; }
	UUID GetPopUUID() { return m_popUUID; }
public:
	// pos is the top left point of the letter
	void ClearQuads(GameTickDesc& desc);
	void AddLetter(GameTickDesc& desc, char letter, float fontSize, glm::vec2 pos, glm::vec3 color);
	void AddQuad(GameTickDesc& desc, uint64_t textureID, glm::vec2 radius, glm::vec2 pos);
	void SetQuadPos(Quad* quad, glm::vec2 pos);
private:
	UUID m_popUUID;
	std::vector<UUID> m_uuids;

	float m_createTime = 0.0f;

	std::function<void(GameTickDesc&, PopUp*)> m_onCreate;
	std::function<void(GameTickDesc&, PopUp*, float)> m_onTick;
	std::function<void(GameTickDesc&, PopUp*)> m_onDelete;
};