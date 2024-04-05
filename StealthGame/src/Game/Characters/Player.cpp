#include "Player.h"

Player::Player()
{
}

void Player::Init(std::vector<QuadInitDesc>& descs)
{
	Scene* scene = GlobalData::Get().m_scene;

	m_uuids.reserve(descs.size());
	for (QuadInitDesc& desc : descs)
	{
		Quad quad(desc.m_pos, desc.m_scale);
		uint64_t uuid = quad.GetUUID().GetUUID();

		RenderQuadInitDesc renderDesc;
		renderDesc.m_depth = desc.m_depth;
		//renderDesc.m_depth = (float)rand() / (float)RAND_MAX;
		renderDesc.m_shaderUUID = desc.m_shaderUUID;
		renderDesc.m_textureUUID = desc.m_textureUUID;

		scene->AddQuad(quad, renderDesc);
		scene->GetRenderQuads()[uuid].UpdateRenderQuad(scene, uuid);
		scene->GetAABBs()[uuid].SetEnabled(false);

		m_uuids.emplace_back(uuid);
	}
	scene->GetAABBs()[GetUUID(0).GetUUID()].SetEnabled(true);
	scene->GetRenderQuads()[GetUUID(0).GetUUID()].SetVisibility(false);

	GlobalData::Get().m_collision->AddToLayer(1, GetUUID(0).GetUUID());

	m_animBP.Init();
}

void Player::ClearResources()
{
	m_inventory.ClearResources();
}

void Player::BindCamera(Camera* camera)
{
	m_bindedCamera = camera;
}

void Player::PlayerTick(GameTickDesc& desc)
{
	
	m_actionType = ActionType::NORMAL;
	if (!m_inputEnabled)
		return;

	MovePlayer(desc);

	if (!m_isCrouching)
		m_isDragging = false;

	if (KeyBoard::IsKeyPressDown(GLFW_KEY_SPACE))
	{
		if (m_isHidden)
			UnHidePlayer();
	}

	m_inventory.InventoryTick(desc);
	m_animBP.Tick(this);

	if (m_isDragging)
		m_actionType = ActionType::ILLEGAL;
}

void Player::HidePlayer(glm::vec2 pos)
{
	m_isHidden = true;
	m_posBeforeHidden = GetPos();
	SetPos(pos);
}

void Player::UnHidePlayer()
{
	SetPos(m_posBeforeHidden);
	m_isHidden = false;
	m_isCrouching = false;
}

void Player::EliminateNPC(NPC& victim)
{
	SetPos(victim.GetPos());
	m_actionType = ActionType::ILLEGAL;
}

void Player::MovePlayer(GameTickDesc& desc)
{
	GlobalData& gData = GlobalData::Get();

	float speed = (m_isCrouching ? m_crouchSpeed : m_normalSpeed) * desc.m_tickTimer->Second();
	glm::vec2 lastPos = GetPos();
	glm::vec2 add = {};
	if (KeyBoard::IsKeyDown(GLFW_KEY_W))
		add.y += speed;
	if (KeyBoard::IsKeyDown(GLFW_KEY_A))
		add.x -= speed;
	if (KeyBoard::IsKeyDown(GLFW_KEY_S))
		add.y -= speed;
	if (KeyBoard::IsKeyDown(GLFW_KEY_D))
		add.x += speed;

	Move(desc.m_collision, add.x, add.y);

	m_velocity = GetPos() - lastPos;

	desc.m_camera->SetPos(GetQuad(2)->GetPos());

	// crouch / unCrouch accoding to the input
	if (KeyBoard::IsKeyPressDown(GLFW_KEY_C))
		m_isCrouching = !m_isCrouching;

	GetQuad(0)->SetRadius(m_isCrouching ? m_crouchScale : m_normalScale);
	// check if in wall
	if (desc.m_collision->Collide(0, GetUUID(0)).m_hasHit)
		m_isCrouching = true;

	// synch hitbox and character
	GetQuad(2)->SetPos(GetQuad(0)->GetPos());
}