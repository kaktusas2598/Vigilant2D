#include "UIProgressBar.hpp"
#include "UIRenderer.hpp"
#include "glm/common.hpp"

void UIProgressBar::draw(UIRenderer& uiRenderer,
                         const glm::vec2& position,
                         const glm::vec2& size,
                         float minValue,
                         float maxValue,
                         float value,
                         const UIProgressBarStyle& style) const {
    const float range = maxValue - minValue;
    const float t = range > 0.0f
        ? glm::clamp((value - minValue) / range, 0.0f, 1.0f)
        : 0.0f;

    uiRenderer.drawQuad({
        {position, size},
        TextureRegion::full(nullptr),
        style.backgroundColor
    });

    const glm::vec2 innerPos = position + style.fillInset;
    const glm::vec2 innerSize = {
        glm::max(0.0f, (size.x - style.fillInset.x * 2.0f) * t),
        glm::max(0.0f, size.y - style.fillInset.y * 2.0f)
    };

    if (innerSize.x > 0.0f && innerSize.y > 0.0f) {
        uiRenderer.drawQuad({
            {innerPos, innerSize},
            TextureRegion::full(nullptr),
            style.fillColor
        });
    }

    if (style.borderEnabled) {
        uiRenderer.drawQuadOutline({
            {position, size},
            style.borderThickness,
            style.borderColor
        });
    }
}
