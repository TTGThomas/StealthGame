#include "UUID.h"

static std::random_device randomDevice;
static std::mt19937_64 randomEngine(randomDevice());
static std::uniform_int_distribution<uint64_t> uniformDistribution;

GameUUID::GameUUID()
	: m_uuid(uniformDistribution(randomEngine))
{
}

GameUUID::GameUUID(uint64_t& uuid)
	: m_uuid(uuid)
{
}

GameUUID::GameUUID(const uint64_t& uuid)
	: m_uuid(uuid)
{
}

GameUUID::GameUUID(uint64_t&& uuid)
	: m_uuid(uuid)
{
}

void GameUUID::RegenerateUUID()
{
	m_uuid = uniformDistribution(randomEngine);
}