#pragma once

#include <vector>
#include <unordered_map>
#include <string>

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
		Task() = default;

		Task(TaskType type, std::string text, GameUUID taskUUID)
			: m_type(type), m_text(text), m_uuid(taskUUID) {}

		TaskType m_type = TaskType::ELIMINATE;
		std::string m_text;
		GameUUID m_uuid = 1;
	};
public:
	TaskBar() = default;
	~TaskBar();

	void Init(GameTickDesc& desc);

	void AddTask(Task task);

	void CompleteTask(uint64_t taskID);

	void SetStartPos(glm::vec2 sPos);

	void UpdateTaskbar(GameTickDesc& desc);

	void ShowStatsWindow();
	
	Quad* GetQuad(int index) { return &GlobalData::Get().m_scene->GetQuads()[m_uuids[index].GetUUID()]; }
	const std::vector<GameUUID>& GetUUIDs() { return m_uuids; }
	GameUUID& GetUUID(int index) { return m_uuids[index]; }
private:
	// pos is the top left point of the letter
	void AddLetter(GameTickDesc& desc, char letter, glm::vec2 pos, glm::vec3 color);
	void AddQuad(GameTickDesc& desc, uint64_t textureID, glm::vec2 pos);
	void SetQuadPos(Quad* quad, glm::vec2 pos);
private:
	void ClearQuads(GameTickDesc& desc);
private:
	bool m_changed = true;

	std::vector<GameUUID> m_uuids;
	std::vector<uint64_t> m_iconTextures;
	std::unordered_map<uint64_t, Task> m_tasks;
	std::vector<uint64_t> m_taskOrder;

	float m_fontSize = 0.05f;
	glm::vec2 m_startPos = { -1.0f, 1.0f };
};