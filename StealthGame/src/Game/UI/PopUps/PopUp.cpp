#include "PopUp.h"

void PopUp::Init(
	std::function<void(GameTickDesc&, PopUp*)> onCreate,
	std::function<void(GameTickDesc&, PopUp*, float)> onTick,
	std::function<void(GameTickDesc&, PopUp*)> onDelete
)
{
	m_onCreate = onCreate;
	m_onTick = onTick;
	m_onDelete = onDelete;
}

void PopUp::OnCreate(GameTickDesc& desc)
{
	m_onCreate(desc, this);
	m_createTime = (float)glfwGetTime();
}

void PopUp::OnTick(GameTickDesc& desc)
{
	m_onTick(desc, this, (float)glfwGetTime() - m_createTime);
}

void PopUp::OnDelete(GameTickDesc& desc)
{
	m_onDelete(desc, this);
	ClearQuads(desc);
}

void PopUp::AddLetter(GameTickDesc& desc, char letter, float fontSize, glm::vec2 pos, glm::vec3 color)
{
	Scene* scene = desc.m_scene;

	int letterIndex = letter - ' ';
	if (letterIndex < 0)
		letterIndex = 95;

	Quad quad;
	uint64_t uuid = quad.GetUUID().GetUUID();
	float widthRatio = desc.m_scene->GetLetterWidths()[letterIndex] / 24.0f;
	quad.SetRadius({ fontSize * widthRatio, fontSize });
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

void PopUp::AddQuad(GameTickDesc& desc, uint64_t textureID, glm::vec2 radius, glm::vec2 pos)
{
	Scene* scene = desc.m_scene;

	Quad quad;
	uint64_t uuid = quad.GetUUID().GetUUID();
	quad.SetRadius(radius);
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

void PopUp::SetQuadPos(Quad* quad, glm::vec2 pos)
{
	GlobalData& gData = GlobalData::Get();
	quad->SetPos(pos);
}

void PopUp::ClearQuads(GameTickDesc& desc)
{
	for (GameUUID& uuid : m_uuids)
	{
		uint64_t id = uuid.GetUUID();
		desc.m_scene->DeleteQuad(id);
	}
	m_uuids = {};
}