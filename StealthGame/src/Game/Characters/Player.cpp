#include "Player.h"

Player::Player()
{
}

void Player::Init(std::vector<QuadInitDesc>& descs)
{
	Entity::Init(descs);
	m_animBP.Init();
}

void Player::ClearResources()
{
	m_inventory.ClearResources();
	NPCAnimBP::ClearResources();
}

void Player::BindCamera(Camera* camera)
{
	m_bindedCamera = camera;
}

void Player::PlayerTick(GameTickDesc& desc)
{
	ApplyDamage();

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

void Player::ShowWindow()
{
	ImGui::Begin("Player");

	ImGui::Text("Action Illegal: %i", m_actionType == ActionType::ILLEGAL ? 1 : 0);
	
	float invDt = 1.0f / GlobalData::Get().m_deltaTime;
	ImGui::Text("Speed: %f", glm::length(invDt * glm::vec2(m_velocity.x, m_velocity.y)));
	
	ImGui::Text("Health: %i", m_health);

	const char* names[] =
	{
		"Standard",
		"Guest",
		"Vip Guest",
		"Guard",
		"Vip Guard",
	};
	ImGui::Text("Identity: %s", names[(int)m_disguise]);

	ImGui::End();
}

void Player::ChangeDisguise(Identities type)
{
	m_disguise = type;
	m_animBP.ChangeDisguise(this, type);
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

bool Player::OnTrespassZone()
{
	bool ret = true;
	if (m_disguise == Identities::GUARD)
		ret = false;
	else if (m_disguise == Identities::VIPGUARD)
		ret = false;

	if (ret)
		m_actionType = ActionType::ILLEGAL;
	return ret;
}

bool Player::OnHostileZone()
{
	bool ret = true;
	if (m_disguise == Identities::VIPGUARD)
		ret = false;

	if (ret)
		m_actionType = ActionType::ILLEGAL;
	return ret;
}

bool Player::IsGunShooting()
{
	if (m_inventory.GetEquippiedType() != Inventory::Type::GUN)
		return false;

	return ((Gun*)m_inventory.GetEquipped().get())->IsShooting();
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

void Player::ApplyDamage()
{
	return;
	if (m_health > 200)// not a valid range
		return;

	GlobalData& gData = GlobalData::Get();

	CollisionPayload payload = gData.m_collision->Collide(4, GetUUID(0));
	if (payload.m_hasHit)
	{
		m_health = 0;
	}
}