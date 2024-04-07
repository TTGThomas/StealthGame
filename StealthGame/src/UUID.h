#pragma once

#include <random>

class GameUUID
{
public:
	GameUUID();
	GameUUID(uint64_t& uuid);
	GameUUID(const uint64_t& uuid);
	GameUUID(uint64_t&& uuid);

	void RegenerateUUID();

	const uint64_t& GetUUID() { return m_uuid; }
private:
	uint64_t m_uuid;
};