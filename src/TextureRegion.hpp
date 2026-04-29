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

    /*static TextureRegion fromPixels(Texture *texture,
                                    int x, int y,
                                    int width, int height,
                                    int textureWidth, int textureHeight) {
        glm::vec2 uvMin(
            static_cast<float>(x) / textureWidth,
            static_cast<float>(y) / textureHeight);

        glm::vec2 uvMax(
            static_cast<float>(x + width) / textureWidth,
            static_cast<float>(y + height) / textureHeight);

        return {texture, uvMin, uvMax};
    }*/
};