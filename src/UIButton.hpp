#pragma once

#include <string>
#include "TextureRegion.hpp"
#include "glm/glm.hpp"

class UIRenderer;
class TextRenderer;
class Font;

class UIButton {
public:
    void setText(const std::string& value) { text = value; }
    void setPosition(const glm::vec2& value) { position = value; }
    void setSize(const glm::vec2& value) { size = value; }
    void setPadding(const glm::vec2& value) { padding = value; }
    void setTextScale(float value) { textScale = value; }

    void setTextColor(const glm::vec4& value) { textColor = value; }
    void setBackgroundColor(const glm::vec4& value) { backgroundColor = value; }
    void setBorderColor(const glm::vec4& value) { borderColor = value; }
    void setBorderEnabled(bool value) { borderEnabled = value; }

    void setIconEnabled(bool value) { iconEnabled = value; }
    void setIcon(const TextureRegion& value) { icon = value; }
    void setIconSize(const glm::vec2& value) { iconSize = value; }
    void setIconTint(const glm::vec4& value) { iconTint = value; }

    void drawScreenGeometry(UIRenderer& uiRenderer) const;
    void drawScreenText(TextRenderer& textRenderer, const Font& font) const;
    void drawScreenIcon(UIRenderer& uiRenderer) const;

private:
    std::string text;
    glm::vec2 position{0.0f, 0.0f};
    glm::vec2 size{160.0f, 44.0f};
    glm::vec2 padding{12.0f, 8.0f};
    float textScale = 1.0f;

    glm::vec4 textColor{1.0f, 1.0f, 1.0f, 1.0f};
    glm::vec4 backgroundColor{0.12f, 0.12f, 0.14f, 0.95f};
    glm::vec4 borderColor{0.85f, 0.80f, 0.55f, 1.0f};
    bool borderEnabled = true;

    bool iconEnabled = false;
    TextureRegion icon = TextureRegion::full(nullptr);
    glm::vec2 iconSize{16.0f, 16.0f};
    glm::vec4 iconTint{1.0f, 1.0f, 1.0f, 1.0f};
};