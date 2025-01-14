#include "TaskBar.h"

TaskBar::~TaskBar()
{
	m_tasks.clear();
	m_taskOrder = {};
}

void TaskBar::Init(GameTickDesc& desc)
{
	// creates icon textures

	/*
	enum class TaskType
	{
		ELIMINATE, ESCAPE
	};
	*/

	m_iconTextures.resize(2);

	m_iconTextures[0] = desc.m_renderer->AddTexture("res/Icons/Eliminate.png");
	m_iconTextures[1] = desc.m_renderer->AddTexture("res/Icons/Escape.png");
	
}

void TaskBar::AddTask(Task task)
{
	m_changed = true;
	m_tasks[task.m_uuid.GetUUID()] = task;
	m_taskOrder.emplace_back(task.m_uuid.GetUUID());
}

void TaskBar::CompleteTask(uint64_t taskID)
{
	m_changed = true;
	int index = 0;
	for (uint64_t& id : m_taskOrder)
	{
		if (id == taskID)
			m_taskOrder.erase(m_taskOrder.begin() + index);
		index++;
	}
	m_tasks.erase(taskID);
}

void TaskBar::SetStartPos(glm::vec2 sPos)
{
	m_changed = true;
	m_startPos = sPos;
}

void TaskBar::UpdateTaskbar(GameTickDesc& desc)
{
	if (m_changed)
	{
		m_changed = false;
		ClearQuads(desc);
		int taskIndex = 0;
		glm::vec3 color = { 1.0f, 1.0f, 1.0f };
		for (uint64_t taskOrder : m_taskOrder)
		{
			Task& task = m_tasks[taskOrder];

			glm::vec2 pos = m_startPos;
			pos.y -= (float)taskIndex * m_fontSize * 2.0f;

			AddQuad(desc, m_iconTextures[(int)task.m_type], pos);
			pos.x += GetQuad((int)m_uuids.size() - 1)->GetRadius().x * 2.0f;

			for (int i = 0; i < task.m_text.size(); i++)
			{
				char letter = task.m_text[i];
				AddLetter(desc, letter, pos, color);
				pos.x += GetQuad((int)m_uuids.size() - 1)->GetRadius().x * 2.0f;
			}
			taskIndex++;
		}
	}
}

void TaskBar::ShowStatsWindow()
{
#ifndef IMGUI_DISABLE
	ImGui::Begin("TaskBarStats");
	ImGui::DragFloat("FontSize", &m_fontSize, 0.01f);
	ImGui::DragFloat2("StartPos", glm::value_ptr(m_startPos), 0.01f);
	if (ImGui::Button("Refresh"))
		m_changed = true;
	ImGui::End();
#endif
}

void TaskBar::AddLetter(GameTickDesc& desc, char letter, glm::vec2 pos, glm::vec3 color)
{
	Scene* scene = desc.m_scene;

	int letterIndex = letter - ' ';
	if (letterIndex < 0)
		letterIndex = 95;

	Quad quad;
	uint64_t uuid = quad.GetUUID().GetUUID();
	float widthRatio = desc.m_scene->GetLetterWidths()[letterIndex] / 24.0f;
	quad.SetRadius({ m_fontSize * widthRatio, m_fontSize });
	quad.SetPos(pos + glm::vec2(quad.GetRadius().x, -quad.GetRadius().y));
	m_uuids.emplace_back(GameUUID(uuid));
	RenderQuadInitDesc renderDesc;
	renderDesc.m_followCameraOffset = false;
	renderDesc.m_depth = 1.0f;
	renderDesc.m_shaderUUID = GlobalData::Get().m_defaultShader;
	renderDesc.m_textureUUID = desc.m_scene->GetTextureFromChar(desc.m_renderer, color, letter);

	scene->AddQuad(quad, renderDesc);
	scene->GetRenderQuads().at(uuid).UpdateRenderQuad(scene, uuid);
	scene->GetAABBs().at(uuid).SetEnabled(false);
}

void TaskBar::AddQuad(GameTickDesc& desc, uint64_t textureID, glm::vec2 pos)
{
	Scene* scene = desc.m_scene;

	Quad quad;
	uint64_t uuid = quad.GetUUID().GetUUID();
	quad.SetRadius({ m_fontSize, m_fontSize });
	quad.SetPos(pos + glm::vec2(quad.GetRadius().x, -quad.GetRadius().y));
	m_uuids.emplace_back(GameUUID(uuid));
	RenderQuadInitDesc renderDesc;
	renderDesc.m_followCameraOffset = false;
	renderDesc.m_depth = 1.0f;
	renderDesc.m_shaderUUID = GlobalData::Get().m_defaultShader;
	renderDesc.m_textureUUID = textureID;

	scene->AddQuad(quad, renderDesc);
	scene->GetRenderQuads().at(uuid).UpdateRenderQuad(scene, uuid);
	scene->GetAABBs().at(uuid).SetEnabled(false);
}

void TaskBar::SetQuadPos(Quad* quad, glm::vec2 pos)
{
	GlobalData& gData = GlobalData::Get();
	quad->SetPos(pos);
}

void TaskBar::ClearQuads(GameTickDesc& desc)
{
	for (GameUUID& uuid : m_uuids)
	{
		uint64_t id = uuid.GetUUID();
		desc.m_scene->DeleteQuad(id);
	}
	m_uuids = {};
}