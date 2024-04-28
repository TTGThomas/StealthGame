#pragma once

#include "../NPC.h"

// GUARD:
// 
//       +------------+
//       |SearchPlayer|
//       +------------+
//          ^      |
//          |      v
// +-----+->+------+ +------------+
// |Alert|  |Attack| |SeachGunShot|
// +-----+<-+------+ +------------+
//  ^    |             ^      |
//  |    v             |      v
// +-----------+------>+------+->+---------+
// |MoveOnRoute|       |Search|  |SeachBody|
// +-----------+<------+------+<-+---------+
//    ^      |         ^      |
//    |      v         |      v
// +------------+      +------------+
// |LookToPlayer|      |SeachIllegal|
// +------------+      +------------+

class Guard : public NPC
{
public:
	void Report(glm::vec2 pos, Identities identity)
	{
		m_disguiseStates[(int)identity] = DisguiseState::COMPROMISED;
		m_reported = true;
		m_reportPos = pos;
	}
protected:
	virtual void InitNodeGraph() override;
private:
	bool m_routeFinished = true;

	glm::vec2 m_searchPos = {};
	void* m_searchParam = nullptr;// can be anything
	bool m_searchFinish = false;
	bool m_reported = false;
	glm::vec2 m_reportPos = {};

	float m_shootDur = 0.0f;
};