#pragma once

#include "glm/glm.hpp"

// 2D Particle model
struct Particle {
    public:
        glm::vec2 position{0.0f};
        glm::vec2 velocity{0.0f};
        glm::vec4 color{1.0f};
        float size = 16.0f;
        float life = 0.0f;
        float maxLife = 1.0f;
        float rotation = 0.0f;
        bool active = false;
};