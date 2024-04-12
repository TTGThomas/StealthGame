#pragma once

#include <vector>
#include <unordered_map>

#include <glm/glm.hpp>

#include <miniaudio/Miniaudio.h>

#include "../UUID.h"

#define MAXAUDIOS 10

class AudioManager
{
public:
	void Init();
	void Cleanup();

	GameUUID AddSound(const char* filePath, glm::vec2 pos, float minDist, float maxDist, bool deleteOnFinish = false, bool ignorePos = false, bool loop = false);
	GameUUID AddSound(GameUUID source, glm::vec2 pos, float minDist, float maxDist, bool deleteOnFinish = false, bool ignorePos = false, bool loop = false);
	void DeleteSound(GameUUID uuid);

	// delays the delete to tick
	void AddDeleteSound(GameUUID uuid);

	void StartSound(GameUUID uuid, int frameIndex = 0);
	void UpdateAllSound(glm::vec2 listenPos);
	void StopSound(GameUUID uuid);

	bool IsSoundFinished(GameUUID uuid);

	float GetSoundMinDist(GameUUID uuid);
	float GetSoundMaxDist(GameUUID uuid);
	glm::vec2 GetSoundPos(GameUUID uuid);
	uint64_t GetSoundSource(GameUUID uuid);

	std::unordered_map<uint64_t, ma_sound>& GetSounds() { return m_sounds; }
private:
	void ConfigSound(GameUUID uuid, glm::vec2 pos, float minDist, float maxDist, bool deleteOnFinish, bool ignorePos, bool loop);
private:
	ma_engine* m_engine;
	std::vector<GameUUID> m_deletes;
	std::unordered_map<uint64_t, ma_sound> m_sounds;
	std::unordered_map<uint64_t, uint64_t> m_sources;
	std::vector<uint64_t> m_followers;// ignore pos
};