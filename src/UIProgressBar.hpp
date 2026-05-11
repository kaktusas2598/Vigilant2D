#pragma once

#include "UIPrimitives.hpp"
#include "glm/glm.hpp"

class UIRenderer;

struct UIProgressBarStyle {
    glm::vec4 backgroundColor{0.15f, 0.15f, 0.15f, 0.95f};
    glm::vec4 fillColor{0.25f, 0.85f, 0.35f, 1.0f};
    glm::vec4 borderColor{0.0f, 0.0f, 0.0f, 0.0f};
    bool borderEnabled = false;
    float borderThickness = 1.0f;
    glm::vec2 fillInset{1.0f, 1.0f};
};

class UIProgressBar {
public:
    void draw(UIRenderer& uiRenderer,
              const glm::vec2& position,
              const glm::vec2& size,
              float minValue,
              float maxValue,
              float value,
              const UIProgressBarStyle& style) const;
};
