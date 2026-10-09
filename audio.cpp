#include "audio.hpp"
#include <cstdio>
#include <cstring>

#if __has_include(<SDL_mixer.h>)
#define HAVE_SDL_MIXER 1
#include <SDL_mixer.h>
#elif __has_include(<SDL2/SDL_mixer.h>)
#define HAVE_SDL_MIXER 1
#include <SDL2/SDL_mixer.h>
#endif

Audio::Audio()
    : deviceId_(0),
      soundVolume_(128),
      musicVolume_(100),
      initialized_(false),
      mixerAvailable_(false) {
    std::memset(&targetSpec_, 0, sizeof(targetSpec_));
}

Audio::~Audio() {
    close();
}

bool Audio::init(const std::string& assetsPath) {
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        std::fprintf(stderr, "Audio init warning: %s\n", SDL_GetError());
        return false;
    }

    targetSpec_.freq = 44100;
    targetSpec_.format = AUDIO_S16SYS;
    targetSpec_.channels = 2;
    targetSpec_.samples = 2048;
    targetSpec_.callback = nullptr;

    deviceId_ = SDL_OpenAudioDevice(nullptr, 0, &targetSpec_, nullptr, 0);
    if (deviceId_ == 0) {
        std::fprintf(stderr, "SDL_OpenAudioDevice failed: %s\n", SDL_GetError());
        return false;
    }

    SDL_PauseAudioDevice(deviceId_, 0); // Unpause
    initialized_ = true;

    // Load authentic sound effects extracted from setup.exe
    loadWav(SFX_JUMP, assetsPath + "/sndJump.wav");
    loadWav(SFX_COIN, assetsPath + "/sndCoin.wav");
    loadWav(SFX_ENEMY, assetsPath + "/sndEnemy.wav");
    loadWav(SFX_HIT, assetsPath + "/sndHit.wav");
    loadWav(SFX_CHECK, assetsPath + "/sndCheck.wav");
    loadWav(SFX_LEVEL_COMPLETED, assetsPath + "/sndLevelCompleted.wav");
    loadWav(SFX_GAME_OVER, assetsPath + "/sndGameOver.wav");
    loadWav(SFX_GAME_WON, assetsPath + "/sndGameWon.wav");
    loadWav(SFX_SELECT, assetsPath + "/sndSelect.wav");
    loadWav(SFX_BACK, assetsPath + "/sndBack.wav");
    loadWav(SFX_CANNON, assetsPath + "/sndCannon.wav");
    loadWav(SFX_WATER, assetsPath + "/sndWater.wav");

#ifdef HAVE_SDL_MIXER
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) >= 0) {
        Mix_AllocateChannels(16);
        mixerAvailable_ = true;
    }
#endif

    return true;
}

bool Audio::loadWav(SoundEffect id, const std::string& filepath) {
    SDL_AudioSpec wavSpec;
    Uint8* wavBuffer = nullptr;
    Uint32 wavLength = 0;

    if (!SDL_LoadWAV(filepath.c_str(), &wavSpec, &wavBuffer, &wavLength)) {
        return false;
    }

    // Convert loaded WAV format to target device format
    SDL_AudioCVT cvt;
    if (SDL_BuildAudioCVT(&cvt, wavSpec.format, wavSpec.channels, wavSpec.freq,
                          targetSpec_.format, targetSpec_.channels, targetSpec_.freq) < 0) {
        SDL_FreeWAV(wavBuffer);
        return false;
    }

    cvt.len = wavLength;
    cvt.buf = (Uint8*)SDL_malloc(size_t(wavLength) * cvt.len_mult);
    if (!cvt.buf) {
        SDL_FreeWAV(wavBuffer);
        return false;
    }

    std::memcpy(cvt.buf, wavBuffer, wavLength);
    SDL_FreeWAV(wavBuffer);

    if (SDL_ConvertAudio(&cvt) != 0) {
        SDL_free(cvt.buf);
        return false;
    }

    WavData data;
    data.buffer = cvt.buf;
    data.length = Uint32(cvt.len_cvt);
    data.spec = targetSpec_;

    sounds_[id] = data;
    return true;
}

void Audio::playSound(SoundEffect sfx) {
    if (!initialized_ || deviceId_ == 0) return;

    auto it = sounds_.find(sfx);
    if (it != sounds_.end() && it->second.buffer && it->second.length > 0) {
        SDL_QueueAudio(deviceId_, it->second.buffer, it->second.length);
    }
}

void Audio::playMusic(const std::string& filename, bool loop) {
#ifdef HAVE_SDL_MIXER
    if (mixerAvailable_) {
        Mix_Music* mus = Mix_LoadMUS(filename.c_str());
        if (mus) {
            Mix_PlayMusic(mus, loop ? -1 : 1);
        }
    }
#else
    (void)filename;
    (void)loop;
#endif
}

void Audio::stopMusic() {
#ifdef HAVE_SDL_MIXER
    if (mixerAvailable_) {
        Mix_HaltMusic();
    }
#endif
}

void Audio::close() {
    for (auto& pair : sounds_) {
        if (pair.second.buffer) {
            SDL_free(pair.second.buffer);
            pair.second.buffer = nullptr;
        }
    }
    sounds_.clear();

#ifdef HAVE_SDL_MIXER
    if (mixerAvailable_) {
        Mix_CloseAudio();
        mixerAvailable_ = false;
    }
#endif

    if (deviceId_ != 0) {
        SDL_CloseAudioDevice(deviceId_);
        deviceId_ = 0;
    }
    initialized_ = false;
}
