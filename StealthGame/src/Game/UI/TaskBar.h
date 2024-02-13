#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>

#include "../Desc.h"

#include "../GlobalData.h"

#include "../../Scene.h"

class TaskBar
{
public:
	enum class TaskType
	{
		ELIMINATE, ESCAPE
	};

	struct Task
	{
		TaskType m_type;
		const char* m_text;
	};
public:
	void Init(GameTickDesc& desc);

	void AddTask(Task task);

	void CompleteTask(int index);

	void SetStartPos(glm::vec2 sPos);

	void UpdateTaskbar(GameTickDesc& desc);

	void ShowStatsWindow();
	
	Quad* GetQuad(int index) { return &GlobalData::Get().m_scene->GetQuads()[m_uuids[index].GetUUID()]; }
	const std::vector<UUID>& GetUUIDs() { return m_uuids; }
	UUID& GetUUID(int index) { return m_uuids[index]; }
private:
	// pos is the top left point of the letter
	void AddLetter(GameTickDesc& desc, char letter, glm::vec2 pos, glm::vec3 color);
	void AddQuad(GameTickDesc& desc, uint64_t textureID, glm::vec2 pos);
	void SetQuadPos(Quad* quad, glm::vec2 pos);
private:
	void ClearQuads(GameTickDesc& desc);
private:
	bool m_changed = true;

	std::vector<UUID> m_uuids;
	std::vector<uint64_t> m_textures;
	std::vector<uint64_t> m_iconTextures;
	std::vector<Task> m_tasks;

	float m_fontSize = 0.05f;
	glm::vec2 m_startPos = { -1.0f, 1.0f };
};