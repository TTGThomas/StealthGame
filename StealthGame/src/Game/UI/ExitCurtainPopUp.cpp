#include "ExitCurtainPopUp.h"

static uint64_t blackTexture = 0;
static PopUpManager* popUpManager = nullptr;

static void OnCreate(GameTickDesc& desc, PopUp* popUp)
{
	if (blackTexture == 0)
	{
		unsigned char* data = new unsigned char[4];
		data[0] = 0x00;
		data[1] = 0x00;
		data[2] = 0x00;
		data[3] = 0xff;
		blackTexture = desc.m_renderer->AddTexture(data, 1, 1);
		delete[] data;
	}
	popUp->AddQuad(desc, blackTexture, { 100.0f, 100.0f }, { -50.0f, 50.0f });
}

static void OnTick(GameTickDesc& desc, PopUp* popUp, float timeFromCreate)
{
	desc.m_scene->GetRenderQuads()[popUp->GetUUID(0).GetUUID()].SetAlpha(std::max(timeFromCreate / 0.5f, 0.0f));
	if (timeFromCreate > 0.5f)
		popUpManager->AddDelete(popUp->GetPopUUID().GetUUID());
}

static void OnStartTick(GameTickDesc& desc, PopUp* popUp, float timeFromCreate)
{
	desc.m_scene->GetRenderQuads()[popUp->GetUUID(0).GetUUID()].SetAlpha(std::max(1.2f - timeFromCreate / 1.0f, 0.0f));
	if (timeFromCreate > 1.2f)
		popUpManager->AddDelete(popUp->GetPopUUID().GetUUID());
}

static void OnDelete(GameTickDesc& desc, PopUp* popUp)
{
}

void ExitCurtainPopUp::ClearResources()
{
	m_manager = nullptr;
	m_startPopUp = {};
	m_endPopUp = {};

	m_endStarted = false;
	m_startStarted = false;
}

void ExitCurtainPopUp::StartEnd(GameTickDesc& desc, PopUpManager* manager)
{
	if (m_endStarted)
		return;

	m_endStarted = true;

	m_manager = manager;
	popUpManager = manager;
	m_endPopUp.Init(OnCreate, OnTick, OnDelete);

	m_manager->AddPopUp(desc, m_endPopUp);
}

void ExitCurtainPopUp::StartStart(GameTickDesc& desc, PopUpManager* manager)
{
	if (m_startStarted)
		return;

	m_startStarted = true;

	m_manager = manager;
	popUpManager = manager;
	m_startPopUp.Init(OnCreate, OnStartTick, OnDelete);

	m_manager->AddPopUp(desc, m_startPopUp);
}
