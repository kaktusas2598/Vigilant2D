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

    ALCdevice* getDevice() const { return device; }
    ALCcontext* getContext() const { return context; }

private:
    ALCdevice* device = nullptr;
    ALCcontext* context = nullptr;
};
