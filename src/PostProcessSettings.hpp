#pragma once

#include "glm/glm.hpp"

struct PostProcessSettings {
    float vignetteStrength = 0.18f;
    float contrast = 1.0f;
    float brightness = 0.0f;
    float saturation = 1.0f;
    glm::vec3 tint{1.0f, 1.0f, 1.0f};
    glm::vec4 fadeColor{0.0f, 0.0f, 0.0f, 1.0f};
    float fadeAmount = 0.0f;
};