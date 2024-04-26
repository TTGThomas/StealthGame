#include "Guard.h"

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

				if (frameFromEnter == 0)
					StartMoveToLocation(location);

				if (MoveToLocation(desc.m_tickTimer->Second()))
				{
					if (!m_isAtTarget)
					{
						m_timeAtTarget = (float)glfwGetTime();
						m_isAtTarget = true;
					}
					else if (1000.0f * ((float)glfwGetTime() - m_timeAtTarget) > m_route[m_targetRouteIndex].m_waitMs)
					{
						m_frameFromEnter = -1;
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

				if (timeFromEnter > 0.5f)
				{
					m_timeWhenEnter = (float)glfwGetTime();

					// shoot
					gData.m_gameScene->GetProjectiles().emplace_back(GetPos(), 4, m_dir, gData.m_texBullet);
					uint64_t t = gData.m_scene->GetAudio().AddSound(
						GameUUID(gData.m_audioGun1),
						GetPos(),
						5.0f, 7.0f,
						true,
						false
					).GetUUID();
					gData.m_scene->GetAudio().StartSound(t);
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
				if (timeFromEnter < 3.0f)
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
					memset(m_disguiseStates, 0, sizeof(DisguiseState));
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
				if (timeFromEnter < 3.0f)
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
					memset(m_disguiseStates, 0, sizeof(DisguiseState));
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
				if (timeFromEnter < 3.0f)
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

	// bridges
	{
		Bridge& bridge = m_bridges.emplace_back(Bridge());
		bridge.m_originIndexes.emplace_back(moveOnRouteIndex);
		bridge.m_destIndex = lookAtPlayerIndex;
		bridge.m_determineFunc = [this](float time, int frame) -> bool
			{
				Player& player = GlobalData::Get().m_gameScene->GetPlayer();
				float dist = glm::distance(player.GetPos(), GetPos());
				if (dist < 0.2f)
					return true;
				if (dist < 0.5f && player.GetVelocity() != glm::vec2(0.0f, 0.0f))
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
				if (dist < 0.2f)
					return false;
				if (dist < 0.5f && player.GetVelocity() != glm::vec2(0.0f, 0.0f))
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
					if (glm::distance(soundPos, GetPos()) < 10.0f)
					{
						if (audio.GetSoundSource(uuid) == gData.m_audioGun1)
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

				if (IsPlayerDetected() && dist < 1.0f && time < 0.1f)
					return true;
				if (player.IsGunShooting())
					return true;
				return false;
			};
	}
	{
		Bridge& bridge = m_bridges.emplace_back(Bridge());
		bridge.m_originIndexes.emplace_back(moveOnRouteIndex);
		bridge.m_originIndexes.emplace_back(lookAtPlayerIndex);
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
				if (IsPlayerDetected() && dist < 1.0f && time < 0.1f)
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
}