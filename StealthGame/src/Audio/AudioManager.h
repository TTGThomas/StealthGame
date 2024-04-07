#pragma once

#include <vector>
#include <unordered_map>

#include <glm/glm.hpp>

#include <miniaudio/Miniaudio.h>

#include "../UUID.h"

class AudioManager
{
public:
	struct RawSound
	{
		RawSound() = default;
		RawSound(ma_sound sound, bool isLoaded = true)
			: m_sound(sound), m_isLoaded(isLoaded) {}

		ma_sound m_sound;
		bool m_isLoaded;
	};

	struct Sound
	{
		Sound() = default;
		Sound(int rawSoundIndex, glm::vec2 pos, float maxDist, bool deleteOnFinsih = false, bool ignorePos = false)
			: m_rawSoundIndex(rawSoundIndex), m_pos(pos), m_maxDist(maxDist), m_deleteOnFinish(deleteOnFinsih), m_ignorePos(ignorePos) {}

		int m_rawSoundIndex;
		glm::vec2 m_pos;
		float m_maxDist;
		bool m_deleteOnFinish;
		bool m_ignorePos;
	};
public:
	void Init();
	void Cleanup();

	void LoadRawSound(const char* filePath);
	void UnloadRawSound(int index);

	int LastRawSoundIndex();

	GameUUID AddSound(int rawSoundIndex, glm::vec2 pos, float maxDist, bool deleteOnFinish = false, bool ignorePos = false);
	void DeleteSound(GameUUID uuid);

	void StartSound(GameUUID uuid, int frameIndex = 0);
	void UpdateSound(GameUUID uuid, glm::vec2 listenPos);
	void UpdateAllSound(glm::vec2 listenPos);
	void StopSound(GameUUID uuid);

	bool IsSoundFinished(GameUUID uuid);
	bool IsRawSoundFinished(int index);

	std::unordered_map<uint64_t, Sound>& GetSounds() { return m_sounds; }
	std::vector<RawSound>& GetRawSounds() { return m_rawSounds; }
	ma_sound& SoundFromUUID(GameUUID uuid) { return m_rawSounds[m_sounds[uuid.GetUUID()].m_rawSoundIndex].m_sound; }
private:
	ma_engine m_engine;
	std::unordered_map<uint64_t, Sound> m_sounds;
	std::vector<RawSound> m_rawSounds;
};