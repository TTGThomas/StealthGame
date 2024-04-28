#include "Guard.h"

#include "NPCConfig.h"

#include "../../GameScene.h"

void Guard::InitNodeGraph()
{
	// nodes
	{
		Node& node = m_nodes.emplace_back(Node());
		node.m_func = [this](GameTickDesc& desc, float timeFromEnter, int frameFromEnter)
			{
				m_stateOverview = State::NORMAL;
				m_speed = m_normalSpeed;
				m_isAttacking = false;
				glm::vec2 location = m_route[m_targetRouteIndex].m_pos;

				if (m_routeFinished)
				{
					m_routeFinished = false;
					StartMoveToLocation(location);
				}

				if (MoveToLocation(desc.m_tickTimer->Second()))
				{
					if (!m_isAtTarget)
					{
						m_timeAtTarget = (float)glfwGetTime();
						m_isAtTarget = true;
					}
					else if (1000.0f * ((float)glfwGetTime() - m_timeAtTarget) > m_route[m_targetRouteIndex].m_waitMs)
					{
						m_routeFinished = true;
						m_targetRouteIndex++;
						if (m_targetRouteIndex == m_route.size())
							m_targetRouteIndex = 0;
					}
				}
				else
				{
					m_isAtTarget = false;
					if (m_velocity != glm::vec2(0.0f))
						PointAtPoint(GetPos() + m_velocity);
				}
			};
	}
	int moveOnRouteIndex = m_nodes.size() - 1;
	{
		m_stateOverview = State::NORMAL;
		Node& node = m_nodes.emplace_back(Node());
		node.m_func = [this](GameTickDesc& desc, float timeFromEnter, int frameFromEnter)
			{
				m_speed = m_normalSpeed;
				m_isAttacking = false;
				Player& player = GlobalData::Get().m_gameScene->GetPlayer();
				PointAtPoint(player.GetPos());
			};
	}
	int lookAtPlayerIndex = m_nodes.size() - 1;
	{
		Node& node = m_nodes.emplace_back(Node());
		node.m_func = [this](GameTickDesc& desc, float timeFromEnter, int frameFromEnter)
			{
				m_stateOverview = State::ATTACK;
				m_isAttacking = true;
				GlobalData& gData = GlobalData::Get();

				Player& player = GlobalData::Get().m_gameScene->GetPlayer();
				glm::vec2 vec = glm::normalize(GetPos() - player.GetPos());
				MoveToTarget(desc.m_tickTimer->Second(), player.GetPos() + vec);
				PointAtPoint(player.GetPos());

				m_animBP.PointGunAt(this, player.GetPos());

				m_speed = m_runningSpeed;

				if (timeFromEnter > SHOOTDUR)
				{
					m_timeWhenEnter = (float)glfwGetTime();

					// shoot
					uint64_t t = gData.m_scene->GetAudio().AddSound(
					GameUUID(gData.m_audioGun2),
						GetPos(),
						5.0f, 7.0f,
						true,
						false
						).GetUUID();
					ProjectileInitDesc desc;
					desc.m_audioUUID = t;
					desc.m_colLayer = 4;
					desc.m_lifeSpan = 5.0f;
					desc.m_pos = GetPos();
					desc.m_rot = m_dir;
					desc.m_size = 0.05f;
					desc.m_speed = 10.0f;
					desc.m_texID = gData.m_texBullet;
					gData.m_gameScene->GetProjectiles().emplace_back(std::make_unique<Projectile>(desc));

				}
			};
	}
	int attackIndex = m_nodes.size() - 1;
	{
		Node& node = m_nodes.emplace_back(Node());
		node.m_func = [this](GameTickDesc& desc, float timeFromEnter, int frameFromEnter)
			{
				m_stateOverview = State::SEARCHING;
				m_speed = m_runningSpeed;
				m_isAttacking = false;
				Player* player = &GlobalData::Get().m_gameScene->GetPlayer();
				m_searchFinish = false;
				if (timeFromEnter < SEARCHTIME)
				{
					// phrase 1 - search gunshot source
					if (frameFromEnter == 0)
						StartMoveToLocation(m_searchPos);

					if (!m_isDynamicRouteCalculated)
						m_timeWhenEnter = (float)glfwGetTime();

					if (!MoveToLocation(desc.m_tickTimer->Second()))
					{
						m_timeWhenEnter = (float)glfwGetTime();
					}
					PointAtPoint(GetPos() + m_velocity);
				}
				else
				{
					m_searchFinish = true;
				}
			};
	}
	int searchGunShotIndex = m_nodes.size() - 1;
	{
		Node& node = m_nodes.emplace_back(Node());
		node.m_func = [this](GameTickDesc& desc, float timeFromEnter, int frameFromEnter)
			{
				m_stateOverview = State::SEARCHING;
				m_speed = m_runningSpeed;
				m_isAttacking = false;
				Player* player = &GlobalData::Get().m_gameScene->GetPlayer();
				NPC* body = (NPC*)m_searchParam;
				m_searchFinish = false;
				if (timeFromEnter < SEARCHTIME)
				{
					// phrase 1 - search body place
					if (frameFromEnter == 0)
						StartMoveToLocation(body->GetPos());

					if (!m_isDynamicRouteCalculated)
						m_timeWhenEnter = (float)glfwGetTime();

					if (!MoveToLocation(desc.m_tickTimer->Second()))
					{
						m_timeWhenEnter = (float)glfwGetTime();
					}
					PointAtPoint(GetPos() + m_velocity);
				}
				else
				{
					// teleport body to concentration location;
					body->SetPos(GlobalData::Get().m_bodyConcentrationPos);

					m_searchFinish = true;
				}
			};
	}
	int searchBodyIndex = m_nodes.size() - 1;
	{
		Node& node = m_nodes.emplace_back(Node());
		node.m_func = [this](GameTickDesc& desc, float timeFromEnter, int frameFromEnter)
			{
				m_stateOverview = State::SEARCHING;
				m_speed = m_runningSpeed;
				m_isAttacking = false;
				Player* player = &GlobalData::Get().m_gameScene->GetPlayer();
				m_searchFinish = false;
				if (timeFromEnter < SEARCHTIME)
				{
					// phrase 1 - search player last pos
					if (frameFromEnter == 0)
						StartMoveToLocation(player->GetPos());

					if (!m_isDynamicRouteCalculated)
						m_timeWhenEnter = (float)glfwGetTime();

					if (!MoveToLocation(desc.m_tickTimer->Second()))
					{
						m_timeWhenEnter = (float)glfwGetTime();
					}
					PointAtPoint(GetPos() + m_velocity);
				}
				else
				{
					m_searchFinish = true;
				}
			};
	}
	int searchPlayerIndex = m_nodes.size() - 1;
	{
		Node& node = m_nodes.emplace_back(Node());
		node.m_func = [this](GameTickDesc& desc, float timeFromEnter, int frameFromEnter)
			{
				m_stateOverview = State::SEARCHING;
				m_speed = m_runningSpeed;
				m_isAttacking = false;

				if (frameFromEnter == 0)
				{
					m_searchFinish = false;
					StartMoveToLocation(m_reportPos);
				}

				if (MoveToLocation(desc.m_tickTimer->Second()))
				{
					if (timeFromEnter > SEARCHTIME)
						m_searchFinish = true;
				}
				else
					m_timeWhenEnter = (float)glfwGetTime();
				PointAtPoint(GetPos() + m_velocity);
			};
	}
	int doReportIndex = m_nodes.size() - 1;

	// bridges
	{
		Bridge& bridge = m_bridges.emplace_back(Bridge());
		bridge.m_originIndexes.emplace_back(moveOnRouteIndex);
		bridge.m_destIndex = lookAtPlayerIndex;
		bridge.m_determineFunc = [this](float time, int frame) -> bool
			{
				Player& player = GlobalData::Get().m_gameScene->GetPlayer();
				float dist = glm::distance(player.GetPos(), GetPos());
				if (dist < LOOKINNER)
					return true;
				if (dist < LOOKOUTER && player.GetVelocity() != glm::vec2(0.0f, 0.0f))
					return true;
				return false;
			};
	}
	{
		Bridge& bridge = m_bridges.emplace_back(Bridge());
		bridge.m_originIndexes.emplace_back(lookAtPlayerIndex);
		bridge.m_destIndex = moveOnRouteIndex;
		bridge.m_determineFunc = [this](float time, int frame) -> bool
			{
				Player& player = GlobalData::Get().m_gameScene->GetPlayer();
				float dist = glm::distance(player.GetPos(), GetPos());
				if (dist < LOOKINNER)
					return false;
				if (dist < LOOKOUTER && player.GetVelocity() != glm::vec2(0.0f, 0.0f))
				{
					m_timeWhenEnter = (float)glfwGetTime();
					return false;
				}
				return time > 1.0f;
			};
	}
	{
		Bridge& bridge = m_bridges.emplace_back(Bridge());
		bridge.m_originIndexes.emplace_back(moveOnRouteIndex);
		bridge.m_originIndexes.emplace_back(lookAtPlayerIndex);
		bridge.m_destIndex = attackIndex;
		bridge.m_determineFunc = [this](float time, int frame) -> bool
			{
				Player& player = GlobalData::Get().m_gameScene->GetPlayer();
				if (!IsPlayerDetected())
					return false;
				if (m_disguiseStates[(int)player.GetDisguise()] == DisguiseState::COMPROMISED)
					return true;
				if (player.GetActionType() != Player::ActionType::ILLEGAL)
					return false;
				return true;
			};
	}
	{
		Bridge& bridge = m_bridges.emplace_back(Bridge());
		bridge.m_originIndexes.emplace_back(moveOnRouteIndex);
		bridge.m_originIndexes.emplace_back(lookAtPlayerIndex);
		bridge.m_destIndex = searchGunShotIndex;
		bridge.m_determineFunc = [this](float time, int frame) -> bool
			{
				GlobalData& gData = GlobalData::Get();
				AudioManager& audio = gData.m_scene->GetAudio();
				for (auto& [uuid, sound] : audio.GetSounds())
				{
					glm::vec2 soundPos = audio.GetSoundPos(uuid);
					if (glm::distance(soundPos, GetPos()) < GUNSOUNDRADIUS)
					{
						if (audio.GetSoundSource(uuid) == gData.m_audioGun1)
						{
							m_searchPos = soundPos;
							return true;
						}
					}

					if (glm::distance(soundPos, GetPos()) < NPCGUNSOUNDRADIUS)
					{
						if (audio.GetSoundSource(uuid) == gData.m_audioGun2)
						{
							m_searchPos = soundPos;
							return true;
						}
					}
				}
				return false;
			};
	}
	{
		Bridge& bridge = m_bridges.emplace_back(Bridge());
		bridge.m_originIndexes.emplace_back(searchGunShotIndex);
		bridge.m_destIndex = moveOnRouteIndex;
		bridge.m_determineFunc = [this](float time, int frame) -> bool
			{
				return m_searchFinish;
			};
	}
	{
		Bridge& bridge = m_bridges.emplace_back(Bridge());
		bridge.m_originIndexes.emplace_back(searchGunShotIndex);
		bridge.m_destIndex = attackIndex;
		bridge.m_determineFunc = [this](float time, int frame) -> bool
			{
				GlobalData& gData = GlobalData::Get();
				Player& player = gData.m_gameScene->GetPlayer();
				float dist = glm::distance(m_searchPos, player.GetPos());

				if (IsPlayerDetected() && dist < ATTACKRADIUS)
					return true;
				if (IsPlayerDetected() && player.IsGunShooting())
					return true;
				return false;
			};
	}
	{
		Bridge& bridge = m_bridges.emplace_back(Bridge());
		bridge.m_originIndexes.emplace_back(moveOnRouteIndex);
		bridge.m_originIndexes.emplace_back(lookAtPlayerIndex);
		bridge.m_originIndexes.emplace_back(doReportIndex);
		bridge.m_destIndex = searchBodyIndex;
		bridge.m_determineFunc = [this](float time, int frame) -> bool
			{
				GlobalData& gData = GlobalData::Get();
				for (uint64_t& uuid : m_detectedNPCs)
				{
					NPC& npc = *gData.m_gameScene->GetNPCs()[uuid];
					if (npc.GetHealth() <= 0)
					{
						if (m_detectedDeadNPCs.find(uuid) == m_detectedDeadNPCs.end())
						{
							m_detectedDeadNPCs.insert(uuid);
							m_searchParam = (void*)&npc;
							return true;
						}
					}
				}
				return false;
			};
	}
	{
		Bridge& bridge = m_bridges.emplace_back(Bridge());
		bridge.m_originIndexes.emplace_back(searchBodyIndex);
		bridge.m_destIndex = moveOnRouteIndex;
		bridge.m_determineFunc = [this](float time, int frame) -> bool
			{
				return m_searchFinish;
			};
	}
	{
		Bridge& bridge = m_bridges.emplace_back(Bridge());
		bridge.m_originIndexes.emplace_back(searchBodyIndex);
		bridge.m_destIndex = attackIndex;
		bridge.m_determineFunc = [this](float time, int frame) -> bool
			{
				GlobalData& gData = GlobalData::Get();
				Player& player = gData.m_gameScene->GetPlayer();
				NPC* body = (NPC*)m_searchParam;

				float dist = glm::distance(body->GetPos(), player.GetPos());
				if (IsPlayerDetected() && dist < ATTACKRADIUS && time < 0.1f)
					return true;
				return false;
			};
	}
	{
		Bridge& bridge = m_bridges.emplace_back(Bridge());
		bridge.m_originIndexes.emplace_back(searchGunShotIndex);
		bridge.m_originIndexes.emplace_back(searchBodyIndex);
		bridge.m_destIndex = attackIndex;
		bridge.m_determineFunc = [this](float time, int frame) -> bool
			{
				GlobalData& gData = GlobalData::Get();

				bool ret = false;
				for (uint64_t& uuid : m_detectedNPCs)
				{
					NPC& npc = *gData.m_gameScene->GetNPCs()[uuid];
					if (npc.GetHealth() > 0)
					{
						if (npc.GetIsAttacking())
						{
							ret = true;
							break;
						}
					}
				}
				return ret;
			};
	}
	{
		Bridge& bridge = m_bridges.emplace_back(Bridge());
		bridge.m_originIndexes.emplace_back(attackIndex);
		bridge.m_destIndex = searchPlayerIndex;
		bridge.m_determineFunc = [this](float time, int frame) -> bool
			{
				GlobalData& gData = GlobalData::Get();
				Player& player = gData.m_gameScene->GetPlayer();
				return !IsPlayerDetected();
			};
	}
	{
		Bridge& bridge = m_bridges.emplace_back(Bridge());
		bridge.m_originIndexes.emplace_back(searchPlayerIndex);
		bridge.m_destIndex = attackIndex;
		bridge.m_determineFunc = [this](float time, int frame) -> bool
			{
				GlobalData& gData = GlobalData::Get();
				Player& player = gData.m_gameScene->GetPlayer();
				return IsPlayerDetected();
			};
	}
	{
		Bridge& bridge = m_bridges.emplace_back(Bridge());
		bridge.m_originIndexes.emplace_back(searchPlayerIndex);
		bridge.m_destIndex = moveOnRouteIndex;
		bridge.m_determineFunc = [this](float time, int frame) -> bool
			{
				// returns if the player is lossed or not
				GlobalData& gData = GlobalData::Get();
				Player& player = gData.m_gameScene->GetPlayer();

				if (m_searchFinish)
				{
					m_disguiseStates[(int)player.GetDisguise()] = DisguiseState::COMPROMISED;
					return true;
				}
				return false;
			};
	}
	{
		Bridge& bridge = m_bridges.emplace_back(Bridge());
		bridge.m_originIndexes.emplace_back(moveOnRouteIndex);
		bridge.m_originIndexes.emplace_back(lookAtPlayerIndex);
		bridge.m_destIndex = doReportIndex;
		bridge.m_determineFunc = [this](float time, int frame) -> bool
			{
				bool ret = m_reported;
				m_reported = false;
				return ret;
			};
	}
	{
		Bridge& bridge = m_bridges.emplace_back(Bridge());
		bridge.m_originIndexes.emplace_back(doReportIndex);
		bridge.m_destIndex = moveOnRouteIndex;
		bridge.m_determineFunc = [this](float time, int frame) -> bool
			{
				return m_searchFinish;
			};
	}
	{
		Bridge& bridge = m_bridges.emplace_back(Bridge());
		bridge.m_originIndexes.emplace_back(doReportIndex);
		bridge.m_destIndex = attackIndex;
		bridge.m_determineFunc = [this](float time, int frame) -> bool
			{
				Player& player = GlobalData::Get().m_gameScene->GetPlayer();
				
				if (glm::distance(player.GetPos(), m_reportPos) < ATTACKRADIUS)
					return true;

				if (IsPlayerDetected() && m_disguiseStates[(int)player.GetDisguise()] == DisguiseState::COMPROMISED)
					return true;

				return false;
			};
	}
}