#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <AL/al.h>
#include "glm/glm.hpp"

class AudioBuffer;
class AudioEngine;

class AudioSystem {
public:
    explicit AudioSystem(AudioEngine& audioEngine);
    ~AudioSystem();

    bool loadSound(const std::string& id, const std::string& filePath);
    bool hasSound(const std::string& id) const;
    bool playSound(const std::string& id, float volume = 1.0f);
    bool playSoundAt(const std::string& id, const glm::vec2& position, float volume = 1.0f);

    void setMasterVolume(float volume);
    float getMasterVolume() const;

    void update();
    void clear();

private:
    ALuint acquireSource();

    AudioEngine& audioEngine;
    std::unordered_map<std::string, std::unique_ptr<AudioBuffer>> buffers;
    std::vector<ALuint> sources;
};
