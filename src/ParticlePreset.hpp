#pragma once

#include <string>

#include "TextureRegion.hpp"
#include "glm/glm.hpp"

// Used for bridging the gap between content definition in lua files for particle emitters and particle registry
struct ParticlePreset {
    std::string textureId;
    int capacity = 1024;

    glm::vec4  baseColor{1.0f, 1.0f, 1.0f, 1.0f};
    float baseSize = 8.0f;
    float baseLifetime = 1.0f;

    glm::vec2 baseVelocity{0.0f, 0.0f};
    glm::vec2 velocityVariance{0.0f, 0.0f};
};