#include "Guest.h"

#include "../../GameScene.h"

void Guest::InitNodeGraph()
{	
	// nodes
	{
		Node& node = m_nodes.emplace_back(Node());
		node.m_func = [this](GameTickDesc& desc, float timeFromEnter, int frameFromEnter)
			{
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
				m_speed = m_runningSpeed;
				m_isAttacking = false;
				GlobalData& gData = GlobalData::Get();

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

					matchUUID = npcUUID;
					matchDist = dist;
				}

				if (matchUUID == 0)
					return;

				NPC& guard = *gData.m_gameScene->GetNPCs()[matchUUID];

				if (frameFromEnter == 0)
				{
					// Tell guard to stop moving
					m_searchFinish = false;
					StartMoveToLocation(guard.GetPos());
				}

				if (MoveToLocation(gData.m_deltaTime))
				{
					// Report guard
					//guard.SetReport(ReportSearch::DEADBODY);

					if (glm::distance(GetPos(), guard.GetPos()) < MAP_SCALE)
						m_searchFinish = true;
					else
						m_frameFromEnter = -1;
				}
				PointAtPoint(GetPos() + m_velocity);
			};
	}
	int reportPlayerIndex = m_nodes.size() - 1;

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
		bridge.m_destIndex = reportPlayerIndex;
		bridge.m_determineFunc = [this](float time, int frame) -> bool
			{
				Player& player = GlobalData::Get().m_gameScene->GetPlayer();
				return IsPlayerDetected() && player.GetActionType() == Player::ActionType::ILLEGAL;
			};
	}
	{
		Bridge& bridge = m_bridges.emplace_back(Bridge());
		bridge.m_originIndexes.emplace_back(reportPlayerIndex);
		bridge.m_destIndex = moveOnRouteIndex;
		bridge.m_determineFunc = [this](float time, int frame) -> bool
			{
				return m_searchFinish;
			};
	}
}