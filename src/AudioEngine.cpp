#include "AudioEngine.hpp"
#include "Logger.hpp"

bool AudioEngine::init() {
    if (context != nullptr)
        return true;

    device = alcOpenDevice(nullptr);
    if (device == nullptr) {
        VG_ERROR("Failed to open OpenAL device.");
        return false;
    }

    context = alcCreateContext(device, nullptr);
    if (context == nullptr) {
        VG_ERROR("Failed to create OpenAL context.");
        alcCloseDevice(device);
        device = nullptr;
        return false;
    }

    if (!alcMakeContextCurrent(context)) {
        VG_ERROR("Failed to make OpenAL context current.");
        alcDestroyContext(context);
        alcCloseDevice(device);
        context = nullptr;
        device = nullptr;
        return false;
    }

    alDistanceModel(AL_INVERSE_DISTANCE);
    return true;
}

void AudioEngine::shutdown() {
    if (context != nullptr) {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(context);
        context = nullptr;
    }

    if (device != nullptr) {
        alcCloseDevice(device);
        device = nullptr;
    }
}

AudioEngine::~AudioEngine() {
    shutdown();
}

void AudioEngine::setListenerPosition(const glm::vec2& position) {
    alListener3f(AL_POSITION, position.x, position.y, 0.0f);
    alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
}
