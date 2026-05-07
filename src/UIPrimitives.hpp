#pragma once

#include "glm/glm.hpp"
#include "TextureRegion.hpp"

enum class UIRenderSpace {
    Screen,
    World
};

struct UIRect {
    glm::vec2 position{0.0f, 0.0f};
    glm::vec2 size{0.0f, 0.0f};
};

struct UIQuad {
    UIRect rect;
    TextureRegion region = TextureRegion::full(nullptr);
    glm::vec4 color{1.0f, 1.0f, 1.0f, 1.0f};
};

// Border
struct UIQuadOutline {
    UIRect rect;
    float thickness = 1.0f;
    glm::vec4 color{1.0f, 1.0f, 1.0f, 1.0f};
};