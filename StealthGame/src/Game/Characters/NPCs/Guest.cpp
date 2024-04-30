#include "Guest.h"

#include "Guard.h"
#include "NPCConfig.h"

#include "../../GameScene.h"

void Guest::InitNodeGraph()
{	
	// nodes
	{
		Node& node = m_nodes.emplace_back(Node());
		node.m_func = [this](GameTickDesc& desc, float timeFromEnter, int frameFromEnter)
			{
				m_stateOverview = NPC::State::NORMAL;
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
		m_stateOverview = NPC::State::NORMAL;
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
				m_stateOverview = NPC::State::PANIC;
				m_speed = m_runningSpeed;
				m_isAttacking = false;
				GlobalData& gData = GlobalData::Get();

				if (frameFromEnter == 0)
				{
					m_searchFinish = false;

					uint64_t matchUUID = 0;
					float matchDist = FLT_MAX;
					for (auto& [npcUUID, npc] : gData.m_gameScene->GetNPCs())
					{
						if (npc->GetType() != Identities::GUARD && npc->GetType() != Identities::VIPGUARD)
							continue;

						float dist = glm::distance(GetPos(), npc->GetPos());
						if (dist > matchDist)
							continue;

						if (npc->GetState() != State::NORMAL)
							continue;

						if (npc->GetHealth() <= 0)
							continue;

						matchUUID = npcUUID;
						matchDist = dist;
					}
					m_reportNPC = matchUUID;
				}

				if (m_reportNPC == 0)
				{
					if (timeFromEnter > PANICTIME)
						m_searchFinish = true;
					return;
				}

				NPC& guard = *gData.m_gameScene->GetNPCs()[m_reportNPC];

				if (frameFromEnter == 0)
					StartMoveToLocation(guard.GetPos());

				if (MoveToLocation(gData.m_deltaTime))
				{
					if (glm::distance(GetPos(), guard.GetPos()) < MAP_SCALE)
					{
						m_searchFinish = true;

						// Report guard
						((Guard*)&guard)->Report(m_reportPos, m_reportIdentity, m_reportIdentityVal);
						m_reportIdentityVal = (int)DisguiseState::NORMAL;
					}
					else
						m_frameFromEnter = -1;
				}
				PointAtPoint(GetPos() + m_velocity);
			};
	}
	int reportPlayerIndex = m_nodes.size() - 1;
	{
		Node& node = m_nodes.emplace_back(Node());
		node.m_func = [this](GameTickDesc& desc, float timeFromEnter, int frameFromEnter)
			{
				m_stateOverview = State::SEARCHING;
				m_speed = m_normalSpeed;
				m_isAttacking = false;
				Player* player = &GlobalData::Get().m_gameScene->GetPlayer();
				m_searchFinish = false;
				if (timeFromEnter < SEARCHTIME)
				{
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
	int searchCoinIndex = m_nodes.size() - 1;

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
		bridge.m_destIndex = searchCoinIndex;
		bridge.m_determineFunc = [this](float time, int frame) -> bool
			{
				GlobalData& gData = GlobalData::Get();
				AudioManager& audio = gData.m_scene->GetAudio();
				for (auto& [uuid, sound] : audio.GetSounds())
				{
					glm::vec2 soundPos = audio.GetSoundPos(uuid);
					if (!audio.IsSoundPlaying(uuid))
						continue;

					if (glm::distance(soundPos, GetPos()) < audio.GetSoundMinDist(uuid))
					{
						if (audio.GetSoundSource(uuid) == gData.m_audioCoin)
						{
							if (!gData.m_collision->Collide(0, GetPos(), soundPos).m_hasHit)
							{
								m_searchPos = soundPos;
								return true;
							}
						}
					}
				}
				return false;
			};
	}
	{
		Bridge& bridge = m_bridges.emplace_back(Bridge());
		bridge.m_originIndexes.emplace_back(moveOnRouteIndex);
		bridge.m_originIndexes.emplace_back(lookAtPlayerIndex);
		bridge.m_destIndex = reportPlayerIndex;
		bridge.m_determineFunc = [this](float time, int frame) -> bool
			{
				GlobalData& gData = GlobalData::Get();
				AudioManager& audio = gData.m_scene->GetAudio();
				Player& player = gData.m_gameScene->GetPlayer();
				m_reportPos = player.GetPos();

				if (IsPlayerDetected() && player.GetActionType() == Player::ActionType::ILLEGAL)
				{
					m_reportIdentity = player.GetDisguise();
					m_reportIdentityVal = (int)DisguiseState::COMPROMISED;
					return true;
				}

				for (uint64_t& uuid : m_detectedNPCs)
				{
					NPC* npc = gData.m_gameScene->GetNPCs()[uuid].get();
					if (npc->GetHealth() <= 0)
					{
						m_reportPos = npc->GetPos();
						return true;
					}
				}

				for (auto& [uuid, sound] : audio.GetSounds())
					if (glm::distance(audio.GetSoundPos(uuid), GetPos()) < GUNSOUNDRADIUS)
						if (audio.GetSoundSource(uuid) == gData.m_audioGun1)
							return true;

				return false;
			};
	}
	{
		Bridge& bridge = m_bridges.emplace_back(Bridge());
		bridge.m_originIndexes.emplace_back(reportPlayerIndex);
		bridge.m_originIndexes.emplace_back(searchCoinIndex);
		bridge.m_destIndex = moveOnRouteIndex;
		bridge.m_determineFunc = [this](float time, int frame) -> bool
			{
				return m_searchFinish;
			};
	}
}