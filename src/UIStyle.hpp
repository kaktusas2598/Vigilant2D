#pragma once

#include "glm/glm.hpp"

struct UIStyle {
    glm::vec4 panelColor{0.08f, 0.08f, 0.10f, 0.92f};
    glm::vec4 borderColor{0.85f, 0.80f, 0.55f, 1.0f};
    glm::vec4 slotColor{0.16f, 0.16f, 0.18f, 1.0f};
    glm::vec4 selectedSlotColor{0.30f, 0.24f, 0.10f, 1.0f};

    float borderThickness = 2.0f;
    float slotPadding = 8.0f;
    float slotSpacing = 8.0f;
};
