#pragma once

#include <AL/al.h>
#include <string>

enum SampleFormatType {
    Int16,
    Float,
    IMA4,
    MSADPCM
};

class AudioBuffer {
public:
    AudioBuffer() = default;
    ~AudioBuffer();

    AudioBuffer(const AudioBuffer&) = delete;
    AudioBuffer& operator=(const AudioBuffer&) = delete;

    bool loadFromFile(const std::string& filePath);
    void destroy();

    ALuint getID() const { return bufferId; }

private:
    ALuint bufferId = 0;
};
