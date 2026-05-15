#include "AudioSystem.hpp"

#include "AudioBuffer.hpp"
#include "Logger.hpp"

AudioSystem::AudioSystem(AudioEngine& audioEngine)
    : audioEngine(audioEngine) {
}

AudioSystem::~AudioSystem() {
    clear();
}

bool AudioSystem::loadSound(const std::string& id, const std::string& filePath) {
    if (buffers.find(id) != buffers.end())
        return true;

    auto buffer = std::make_unique<AudioBuffer>();
    if (!buffer->loadFromFile(filePath))
        return false;

    buffers[id] = std::move(buffer);
    return true;
}

bool AudioSystem::hasSound(const std::string& id) const {
    return buffers.find(id) != buffers.end();
}

ALuint AudioSystem::acquireSource() {
    for (ALuint source : sources) {
        ALint state = 0;
        alGetSourcei(source, AL_SOURCE_STATE, &state);
        if (state != AL_PLAYING) {
            return source;
        }
    }

    ALuint source = 0;
    alGenSources(1, &source);
    sources.push_back(source);
    return source;
}

bool AudioSystem::playSound(const std::string& id) {
    auto it = buffers.find(id);
    if (it == buffers.end())
        return false;

    ALuint source = acquireSource();
    alSourcei(source, AL_BUFFER, static_cast<ALint>(it->second->getID()));
    alSourcei(source, AL_LOOPING, AL_FALSE);
    alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
    alSourcePlay(source);
    return true;
}

bool AudioSystem::playSoundAt(const std::string& id, const glm::vec2& position) {
    auto it = buffers.find(id);
    if (it == buffers.end())
        return false;

    ALuint source = acquireSource();
    alSourcei(source, AL_BUFFER, static_cast<ALint>(it->second->getID()));
    alSourcei(source, AL_LOOPING, AL_FALSE);
    alSource3f(source, AL_POSITION, position.x, position.y, 0.0f);
    alSourcePlay(source);
    return true;
}

void AudioSystem::update() {
}

void AudioSystem::clear() {
    for (ALuint source : sources) {
        alDeleteSources(1, &source);
    }
    sources.clear();
    buffers.clear();
}
