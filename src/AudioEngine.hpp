#pragma once

#include <AL/al.h>
#include <AL/alc.h>
#include "glm/glm.hpp"

class AudioEngine {
public:
    AudioEngine() = default;
    ~AudioEngine();

    bool init();
    void shutdown();

    void setListenerPosition(const glm::vec2& position);
    void setMasterVolume(float volume);

    float getMasterVolume() const { return masterVolume; }

    ALCdevice* getDevice() const { return device; }
    ALCcontext* getContext() const { return context; }

private:
    ALCdevice* device = nullptr;
    ALCcontext* context = nullptr;
    float masterVolume = 1.0f;
};
