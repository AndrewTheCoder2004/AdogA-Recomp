#pragma once
// Audio Subsystem for Cadog Adventures
// Standalone audio mixer using native SDL2 audio APIs (zero external dependencies required)
// with optional SDL_mixer support for OGG music playback.

#include <SDL.h>
#include <string>
#include <map>
#include <vector>

enum SoundEffect {
    SFX_JUMP = 0,
    SFX_COIN,
    SFX_ENEMY,
    SFX_HIT,
    SFX_CHECK,
    SFX_LEVEL_COMPLETED,
    SFX_GAME_OVER,
    SFX_GAME_WON,
    SFX_SELECT,
    SFX_BACK,
    SFX_CANNON,
    SFX_WATER,
    SFX_COUNT
};

class Audio {
public:
    Audio();
    ~Audio();

    // Initialize audio device with standard 44.1kHz stereo format
    bool init(const std::string& assetsPath);

    // Play sound effect
    void playSound(SoundEffect sfx);

    // Music playback (if SDL_mixer available)
    void playMusic(const std::string& filename, bool loop = true);
    void stopMusic();

    // Volume control (0 to 128)
    void setSoundVolume(int volume) { soundVolume_ = volume; }
    void setMusicVolume(int volume) { musicVolume_ = volume; }

    void close();

private:
    struct WavData {
        Uint8* buffer = nullptr;
        Uint32 length = 0;
        SDL_AudioSpec spec;
    };

    bool loadWav(SoundEffect id, const std::string& filepath);

    SDL_AudioDeviceID deviceId_;
    SDL_AudioSpec targetSpec_;
    std::map<SoundEffect, WavData> sounds_;

    int soundVolume_;
    int musicVolume_;
    bool initialized_;
    bool mixerAvailable_;
};
