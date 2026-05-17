#include "UIButton.hpp"

#include "UIRenderer.hpp"
#include "TextRenderer.hpp"

void UIButton::drawScreenGeometry(UIRenderer& uiRenderer) const {
    uiRenderer.drawQuad({
        {position, size},
        TextureRegion::full(nullptr),
        backgroundColor
    });

    if (borderEnabled) {
        uiRenderer.drawQuadOutline({
            {position, size},
            1.0f,
            borderColor
        });
    }
}

void UIButton::drawScreenText(TextRenderer& textRenderer, const Font& font) const {
    const glm::vec2 textSize = textRenderer.measureText(font, text, textScale);
    const glm::vec2 textPosition{
        position.x + (size.x - textSize.x) * 0.5f,
        position.y + (size.y - textSize.y) * 0.5f
    };
    textRenderer.drawText(font, text, textPosition, textScale, textColor);
}

void UIButton::drawScreenIcon(UIRenderer& uiRenderer) const {
    if (!iconEnabled)
        return;

    const glm::vec2 iconPosition{
        position.x + (size.x - iconSize.x) * 0.5f,
        position.y + (size.y - iconSize.y) * 0.5f
    };
    uiRenderer.drawQuad({
        {{iconPosition}, iconSize},
        icon,
        iconTint
    });
        
}
