#pragma once
#include <string>
#include <unordered_map>

// Forward-declare the miniaudio types we need so this header stays clean.
// The actual miniaudio implementation lives in AudioManager.cpp.
struct ma_engine;
struct ma_sound;

class AudioManager
{
public:
    // Call once in Game::Init() before any sounds are played.
    static void Init();

    // Call once when the application closes (Game destructor or BreakoutLayer::OnDetach).
    static void Shutdown();

    // Load a sound file and store it under 'name' for later use.
    // Supports WAV, MP3, FLAC, OGG (via miniaudio built-ins).
    static void LoadSound(const std::string& filepath, const std::string& name);

    // Play a previously loaded sound.
    // loop = true keeps it running until StopSound() is called (use for music).
    static void PlaySound(const std::string& name, bool loop = false);

    // Stop a looping sound (e.g. background music on game over).
    static void StopSound(const std::string& name);

    // 0.0f = silent, 1.0f = full volume.
    static void SetVolume(const std::string& name, float volume);

private:
    AudioManager() = delete;

    static ma_engine* s_Engine;
    static std::unordered_map<std::string, ma_sound*>   s_Sounds;
};