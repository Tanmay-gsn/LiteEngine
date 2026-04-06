#include "hzpch.h"
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "AudioManager.h"
#include <iostream>

// -----------------------------------------------------------------------
// Static member definitions
// -----------------------------------------------------------------------
ma_engine* AudioManager::s_Engine = nullptr;
std::unordered_map<std::string, ma_sound*>   AudioManager::s_Sounds;

// -----------------------------------------------------------------------
// Init
// -----------------------------------------------------------------------
void AudioManager::Init()
{
    s_Engine = new ma_engine();
    ma_result result = ma_engine_init(nullptr, s_Engine);
    if (result != MA_SUCCESS)
    {
        std::cerr << "[AudioManager] Failed to initialise miniaudio engine.\n";
        delete s_Engine;
        s_Engine = nullptr;
    }
}

// -----------------------------------------------------------------------
// Shutdown
// -----------------------------------------------------------------------
void AudioManager::Shutdown()
{
    // Stop and free every sound
    for (auto& [name, sound] : s_Sounds)
    {
        if (sound)
        {
            ma_sound_stop(sound);
            ma_sound_uninit(sound);
            delete sound;
        }
    }
    s_Sounds.clear();

    if (s_Engine)
    {
        ma_engine_uninit(s_Engine);
        delete s_Engine;
        s_Engine = nullptr;
    }
}

// -----------------------------------------------------------------------
// LoadSound
// -----------------------------------------------------------------------
void AudioManager::LoadSound(const std::string& filepath, const std::string& name)
{
    if (!s_Engine)
    {
        std::cerr << "[AudioManager] Engine not initialised. Call AudioManager::Init() first.\n";
        return;
    }

    ma_sound* sound = new ma_sound();
    // MA_SOUND_FLAG_DECODE pre-decodes to PCM so playback has no latency spike
    ma_result result = ma_sound_init_from_file(s_Engine, filepath.c_str(),
        MA_SOUND_FLAG_DECODE, nullptr, nullptr, sound);
    if (result != MA_SUCCESS)
    {
        std::cerr << "[AudioManager] Failed to load sound: " << filepath << "\n";
        delete sound;
        return;
    }

    s_Sounds[name] = sound;
}

// -----------------------------------------------------------------------
// PlaySound
// -----------------------------------------------------------------------
void AudioManager::PlaySound(const std::string& name, bool loop)
{
    auto it = s_Sounds.find(name);
    if (it == s_Sounds.end())
    {
        std::cerr << "[AudioManager] Sound not found: " << name << "\n";
        return;
    }

    ma_sound* sound = it->second;

    // Seek back to the start so short SFX can be replayed immediately
    ma_sound_seek_to_pcm_frame(sound, 0);
    ma_sound_set_looping(sound, loop ? MA_TRUE : MA_FALSE);
    ma_sound_start(sound);
}

// -----------------------------------------------------------------------
// StopSound
// -----------------------------------------------------------------------
void AudioManager::StopSound(const std::string& name)
{
    auto it = s_Sounds.find(name);
    if (it != s_Sounds.end())
        ma_sound_stop(it->second);
}

// -----------------------------------------------------------------------
// SetVolume
// -----------------------------------------------------------------------
void AudioManager::SetVolume(const std::string& name, float volume)
{
    auto it = s_Sounds.find(name);
    if (it != s_Sounds.end())
        ma_sound_set_volume(it->second, volume);
}