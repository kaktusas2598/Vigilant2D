#include "UILabel.hpp"

#include "UIRenderer.hpp"
#include "TextRenderer.hpp"

void UILabel::drawScreen(UIRenderer& uiRenderer,
                TextRenderer& textRenderer,
                const Font& font,
                int viewportWidth, int viewportHeight) const {
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

    // FIXME: having another pass with shader binding/unbinding here is not ideal, same for other draw method
    textRenderer.beginScreen(viewportWidth, viewportHeight);
    textRenderer.drawText(font, text, position + padding, scale, textColor);
    textRenderer.end();
}

void UILabel::drawWorld(UIRenderer& uiRenderer,
                TextRenderer& textRenderer,
                const Font& font,
                const Camera2D& camera) const {
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

    textRenderer.begin(camera);
    textRenderer.drawText(font, text, position + padding, scale, textColor);
    textRenderer.end();

}