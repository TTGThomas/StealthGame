#include "AudioManager.h"

#define MINIAUDIO_IMPLEMENTATION
#include "Miniaudio/miniaudio.h"

void AudioManager::Init()
{
    m_engine = (ma_engine*)malloc(sizeof(*m_engine));

    ma_engine_config engineConfig = ma_engine_config_init();
    engineConfig.listenerCount = 1;

    ma_result result = ma_engine_init(&engineConfig, m_engine);
    if (result != MA_SUCCESS) {
        printf("Failed to initialize audio engine.");
    }
}

void AudioManager::Cleanup()
{
    for (auto& [uuid, sound] : m_sounds)
    {
        if (ma_sound_at_end(&sound))
            ma_sound_stop(&sound);
        ma_sound_uninit(&sound);
    }
    ma_engine_uninit(m_engine);

    free(m_engine);

    m_sounds = {};
}

GameUUID AudioManager::AddSound(const char* filePath, glm::vec2 pos, float minDist, float maxDist, bool deleteOnFinish, bool ignorePos, bool loop)
{
    GameUUID uuid;
    ma_result result = ma_sound_init_from_file(m_engine, filePath, 0, NULL, NULL, &m_sounds[uuid.GetUUID()]);
    if (result != MA_SUCCESS)
        printf("Audio: %s; FAILED\n", filePath);
    else
        printf("Audio: %s; SUCCESS\n", filePath);

    ConfigSound(uuid, pos, minDist, maxDist, deleteOnFinish, ignorePos, loop);

    return uuid;
}

GameUUID AudioManager::AddSound(GameUUID source, glm::vec2 pos, float minDist, float maxDist, bool deleteOnFinish, bool ignorePos, bool loop)
{
    GameUUID uuid;
    ma_result result = ma_sound_init_copy(m_engine, &m_sounds[source.GetUUID()], 0, NULL, &m_sounds[uuid.GetUUID()]);
    if (result != MA_SUCCESS)
        printf("Audio: %s; FAILED\n", "copied");
    else
        printf("Audio: %s; SUCCESS\n", "copied");

    m_sources[uuid.GetUUID()] = source.GetUUID();

    ConfigSound(uuid, pos, minDist, maxDist, deleteOnFinish, ignorePos, loop);

    return uuid;
}

void AudioManager::DeleteSound(GameUUID uuid)
{
    if (!ma_sound_at_end(&m_sounds[uuid.GetUUID()]))
        StopSound(uuid);
    ma_sound_uninit(&m_sounds[uuid.GetUUID()]);
    m_sounds.erase(uuid.GetUUID());
}

void AudioManager::AddDeleteSound(GameUUID uuid)
{
    m_deletes.emplace_back(uuid);
}

void AudioManager::StartSound(GameUUID uuid, int frameIndex)
{
    ma_sound_seek_to_pcm_frame(&m_sounds[uuid.GetUUID()], frameIndex);
    ma_sound_start(&m_sounds[uuid.GetUUID()]);
}

void AudioManager::UpdateAllSound(glm::vec2 listenPos)
{
    ma_engine_listener_set_position(m_engine, 0, listenPos.x, listenPos.y, 0.0f);
    
    for (uint64_t& uuid : m_followers)
        ma_sound_set_position(&m_sounds[uuid], listenPos.x, listenPos.y, 0.0f);

    for (GameUUID& uuid : m_deletes)
        DeleteSound(uuid);
}

void AudioManager::StopSound(GameUUID uuid)
{
    ma_sound_stop(&m_sounds[uuid.GetUUID()]);
}

bool AudioManager::IsSoundFinished(GameUUID uuid)
{
    return ma_sound_at_end(&m_sounds[uuid.GetUUID()]);
}

bool AudioManager::IsSoundPlaying(GameUUID uuid)
{
    return ma_sound_is_playing(&m_sounds[uuid.GetUUID()]);
}

void AudioManager::SetSoundPos(GameUUID uuid, glm::vec2 pos)
{
    ma_sound_set_position(&m_sounds[uuid.GetUUID()], pos.x, pos.y, 0.0f);
}

void AudioManager::SetSoundVolume(GameUUID uuid, float volume)
{
    ma_sound_set_volume(&m_sounds[uuid.GetUUID()], volume);
}

float AudioManager::GetSoundMinDist(GameUUID uuid)
{
    return ma_sound_get_min_distance(&m_sounds[uuid.GetUUID()]);
}

float AudioManager::GetSoundMaxDist(GameUUID uuid)
{
    return ma_sound_get_max_distance(&m_sounds[uuid.GetUUID()]);
}

glm::vec2 AudioManager::GetSoundPos(GameUUID uuid)
{
    ma_vec3f t = ma_sound_get_position(&m_sounds[uuid.GetUUID()]);
    return glm::vec2(t.x, t.y);
}

uint64_t AudioManager::GetSoundSource(GameUUID uuid)
{
    if (m_sources.find(uuid.GetUUID())!= m_sources.end()) 
        return m_sources[uuid.GetUUID()];
    return 0;
}

void AudioManager::ConfigSound(GameUUID uuid, glm::vec2 pos, float minDist, float maxDist, bool deleteOnFinish, bool ignorePos, bool loop)
{
    if (deleteOnFinish)
    {
        struct EndData
        {
            AudioManager* m_audio;
            GameUUID m_uuid;
        };

        EndData* end = new EndData;
        end->m_audio = this;
        end->m_uuid = uuid;

        ma_sound_set_end_callback(&m_sounds[uuid.GetUUID()],
            [](void* end, ma_sound* sound)
            {
                AudioManager* manager = ((EndData*)end)->m_audio;
                GameUUID uuid = ((EndData*)end)->m_uuid;

                // this delays delete which is good
                manager->AddDeleteSound(uuid);

                delete end;
            },
            (void*)end
        );
    }
    ma_sound_set_position(&m_sounds[uuid.GetUUID()], pos.x, pos.y, 0.0f);
    ma_sound_set_attenuation_model(&m_sounds[uuid.GetUUID()], ma_attenuation_model_linear);
    ma_sound_set_min_distance(&m_sounds[uuid.GetUUID()], minDist);
    ma_sound_set_max_distance(&m_sounds[uuid.GetUUID()], maxDist);
    ma_sound_set_looping(&m_sounds[uuid.GetUUID()], loop);

    if (ignorePos)
        m_followers.emplace_back(uuid.GetUUID());
}
