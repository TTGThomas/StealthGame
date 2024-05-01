#include "ZonePopUp.h"

static ZonePopUp::State state = ZonePopUp::State::NORMAL;
static bool stateChanged = true;

static glm::vec2 startPos;

static float fontSize = 0.08f;

static uint64_t yellowTexture = 0;

static void CreateTrespass(GameTickDesc& desc, PopUp* popUp)
{
	GlobalData& gData = GlobalData::Get();

	if (yellowTexture == 0)
	{
		unsigned char* data = new unsigned char[4];
		data[0] = 0xbb;
		data[1] = 0xbb;
		data[2] = 0x00;
		data[3] = 0xff;
		yellowTexture = desc.m_renderer->AddTexture(data, 1, 1);
		delete[] data;
	}
	popUp->AddQuad(desc, yellowTexture, { fontSize * 4.7f, fontSize }, startPos);

	const char* word = "TRESPASSING";

	glm::vec2 pos = startPos;

	for (int i = 0; i < strlen(word); i++)
	{
		char letter = word[i];
		popUp->AddLetter(desc, letter, fontSize, pos, { 1.0f, 1.0f, 1.0 });
		pos.x += popUp->GetQuad((int)popUp->GetUUIDs().size() - 1)->GetRadius().x * 2.0f;
	}
}

static void CreateHostile(GameTickDesc& desc, PopUp* popUp)
{
	GlobalData& gData = GlobalData::Get();

	if (yellowTexture == 0)
	{
		unsigned char* data = new unsigned char[4];
		data[0] = 0xbb;
		data[1] = 0xbb;
		data[2] = 0x00;
		data[3] = 0xff;
		yellowTexture = desc.m_renderer->AddTexture(data, 1, 1);
		delete[] data;
	}
	popUp->AddQuad(desc, yellowTexture, { fontSize * 2.7f, fontSize }, startPos);

	const char* word = "HOSTILE";

	glm::vec2 pos = startPos;

	for (int i = 0; i < strlen(word); i++)
	{
		char letter = word[i];
		popUp->AddLetter(desc, letter, fontSize, pos, { 1.0f, 1.0f, 1.0 });
		pos.x += popUp->GetQuad((int)popUp->GetUUIDs().size() - 1)->GetRadius().x * 2.0f;
	}
}

static void OnCreate(GameTickDesc& desc, PopUp* popUp)
{
}

static void OnTick(GameTickDesc& desc, PopUp* popUp, float timeFromCreate)
{
	if (stateChanged)
	{
		stateChanged = false;
		popUp->ClearQuads(desc);
		if (state == ZonePopUp::State::TRESPASSING)
			CreateTrespass(desc, popUp);
		else if (state == ZonePopUp::State::HOSTILE)
			CreateHostile(desc, popUp);
	}
}

static void OnDelete(GameTickDesc& desc, PopUp* popUp)
{
}

void ZonePopUp::Init(GameTickDesc& desc, PopUpManager* manager)
{
	m_manager = manager;
	m_popUp.Init(OnCreate, OnTick, OnDelete);

	yellowTexture = 0;
	m_manager->AddPopUp(desc, m_popUp);
}

void ZonePopUp::ClearResources()
{
	m_manager = nullptr;
	m_popUp = {};
}

void ZonePopUp::SetStartPos(glm::vec2 pos)
{
	stateChanged = true;
	startPos = pos;
}

void ZonePopUp::OnTrespass()
{
	if (state != State::TRESPASSING)
		stateChanged = true;
	state = State::TRESPASSING;
}

void ZonePopUp::OnHostile()
{
	if (state != State::HOSTILE)
		stateChanged = true;
	state = State::HOSTILE;
}

void ZonePopUp::OnExit(GameTickDesc& desc)
{
	if (state != State::NORMAL)
		stateChanged = true;
	state = State::NORMAL;
}

float ZonePopUp::GetFontSize()
{
	return fontSize;
}
