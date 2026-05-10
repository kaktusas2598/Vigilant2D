#pragma once

#include "Texture.hpp"
#include "glm/glm.hpp"

struct TextureRegion {
    Texture* texture = nullptr;
    glm::vec2 uvMin{0.0f, 0.0f};
    glm::vec2 uvMax{1.0f, 1.0f};

    static TextureRegion full(Texture* texture) {
        return {texture, {0.0f, 0.0f}, {1.0f, 1.0f}};
    }
};