#pragma once

#include <string>
#include <vector>

#include "AnimationFrame.hpp"

class AnimationClip {
public:
    AnimationClip() = default;
    AnimationClip(std::string name, bool looping = true)
        : name(std::move(name)), looping(looping) {}

    void addFrame(const TextureRegion& region, float duration) {
        frames.push_back({region, duration});
    }

    const AnimationFrame& getFrame(size_t index) const { return frames[index]; }
    size_t getFrameCount() const { return frames.size(); }

    bool isLooping() const { return looping; }
    const std::string& getName() const { return name; }

private:
    std::string name;
    bool looping = true;
    std::vector<AnimationFrame> frames;
};
