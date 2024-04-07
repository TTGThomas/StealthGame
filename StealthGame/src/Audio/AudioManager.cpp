#include "AudioManager.h"

#define MINIAUDIO_IMPLEMENTATION
#include "Miniaudio/miniaudio.h"

void AudioManager::Init()
{
    ma_result result = ma_engine_init(NULL, &m_engine);
    if (result != MA_SUCCESS) {
        printf("Failed to initialize audio engine.");
    }
}

void AudioManager::Cleanup()
{
    for (RawSound& sound : m_rawSounds)
    {
        if (sound.m_isLoaded)
        {
            if (ma_sound_at_end(&sound.m_sound))
                ma_sound_stop(&sound.m_sound);
            ma_sound_uninit(&sound.m_sound);
        }
    }
    ma_engine_uninit(&m_engine);

    m_sounds = {};
    m_rawSounds = {};
}

void AudioManager::LoadRawSound(const char* filePath)
{
    m_rawSounds.push_back(RawSound(ma_sound()));
    ma_result result = ma_sound_init_from_file(&m_engine, filePath, 0, nullptr, nullptr, &m_rawSounds.back().m_sound);
    if (result != MA_SUCCESS)
    {
        printf("audio file: %s; is not loaded correctly\n", filePath);
    }
}

void AudioManager::UnloadRawSound(int index)
{
    m_rawSounds[index].m_isLoaded = false;
    ma_sound_uninit(&m_rawSounds[index].m_sound);
}

int AudioManager::LastRawSoundIndex()
{
    return (int)m_rawSounds.size() - 1;
}

GameUUID AudioManager::AddSound(int rawSoundIndex, glm::vec2 pos, float maxDist, bool deleteOnFinish, bool ignorePos)
{
    GameUUID uuid;
    m_sounds[uuid.GetUUID()] = Sound(rawSoundIndex, pos, maxDist, deleteOnFinish, ignorePos);
    return uuid;
}

void AudioManager::DeleteSound(GameUUID uuid)
{
    if (!ma_sound_at_end(&SoundFromUUID(uuid)))
        StopSound(uuid);
    m_sounds.erase(uuid.GetUUID());
}

void AudioManager::StartSound(GameUUID uuid, int frameIndex)
{
    ma_sound_seek_to_pcm_frame(&SoundFromUUID(uuid), frameIndex);
    ma_sound_start(&SoundFromUUID(uuid));
}

void AudioManager::UpdateSound(GameUUID uuid, glm::vec2 listenPos)
{
    if (IsSoundFinished(uuid) && m_sounds[uuid.GetUUID()].m_deleteOnFinish)
        DeleteSound(uuid);

    if (m_sounds[uuid.GetUUID()].m_ignorePos)
        return;

    glm::vec2 soundPos = m_sounds[uuid.GetUUID()].m_pos;
    float dist = glm::distance(soundPos, listenPos);
    // fun
    float vol = max((float)1 - (float)std::pow(dist / m_sounds[uuid.GetUUID()].m_maxDist, 4), 0.0f);
    //ma_sound_set_pitch(&SoundFromUUID(uuid), vol);
    ma_sound_set_volume(&SoundFromUUID(uuid), vol);
}

void AudioManager::UpdateAllSound(glm::vec2 listenPos)
{
    for (auto& [uuid, val] : m_sounds)
        UpdateSound(uuid, listenPos);
}

void AudioManager::StopSound(GameUUID uuid)
{
    ma_sound_stop(&SoundFromUUID(uuid));
}

bool AudioManager::IsSoundFinished(GameUUID uuid)
{
    return ma_sound_at_end(&SoundFromUUID(uuid));
}

bool AudioManager::IsRawSoundFinished(int index)
{
    return ma_sound_at_end(&m_rawSounds[index].m_sound);
}
