#pragma once

#include <string>
#include <vector>

// Acts as a middleware between lua definition and AnimationClip
struct AnimationDefinition {
    std::string texture;
    int rows = 1;
    int columns = 1;
    int row = 0;
    std::vector<int> frames;
    float frameDuration = 0.1f;
    bool looping = true;
};