#include "UILabel.hpp"

#include "UIRenderer.hpp"
#include "TextRenderer.hpp"

void UILabel::drawScreenGeometry(UIRenderer& uiRenderer,
                TextRenderer& textRenderer,
                const Font& font) const {
    const glm::vec2 textSize = textRenderer.measureText(font, text, scale);
    const glm::vec2 boxSize = textSize + padding * 2.0f;

    if (backgroundEnabled) {
        uiRenderer.drawQuad({
            {position, boxSize},
            TextureRegion::full(nullptr),
            backgroundColor
        });
    }

    if (borderEnabled) {
        uiRenderer.drawQuadOutline({
            {position, boxSize},
            1.0f,
            borderColor
        });
    }
}

void UILabel::drawScreenText(TextRenderer& textRenderer, const Font& font) const {
    textRenderer.drawText(font, text, position + padding, scale, textColor);
}

void UILabel::drawWorldGeometry(UIRenderer& uiRenderer,
                    TextRenderer& textRenderer,
                    const Font& font) const {
    const glm::vec2 textSize = textRenderer.measureText(font, text, scale);
    const glm::vec2 boxSize = textSize + padding * 2.0f;

    if (backgroundEnabled) {
        uiRenderer.drawQuad({
            {position, boxSize},
            TextureRegion::full(nullptr),
            backgroundColor
        });
    }

    if (borderEnabled) {
        uiRenderer.drawQuadOutline({
            {position, boxSize},
            1.0f,
            borderColor
        });
    }
}

void UILabel::drawWorldText(TextRenderer& textRenderer,const Font& font) const {
    textRenderer.drawText(font, text, position + padding, scale, textColor);
}