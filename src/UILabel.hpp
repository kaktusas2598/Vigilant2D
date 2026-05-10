#pragma once

#include <string>

#include "UIPrimitives.hpp"
#include "UIStyle.hpp"
#include "glm/glm.hpp"

#include "Camera2D.hpp"

class UIRenderer;
class TextRenderer;
class Font;

class UILabel {
    public:
        void setText(const std::string& value) { text = value; }
        void setPosition(const glm::vec2& value) { position = value; }
        void setScale(float value) { scale = value; }
        void setTextColor(const glm::vec4& value) { textColor = value; }

        void setPadding(const glm::vec2& value) { padding = value; }
        void setBackgroundEnabled(bool enabled) { backgroundEnabled = enabled; }
        void setBackgroundColor(const glm::vec4& value) { backgroundColor = value; }
        void setBorderEnabled(bool enabled) { borderEnabled = enabled; }
        void setBorderColor(const glm::vec4& value) { borderColor = value; }

        const std::string& getText() const { return text; }
        const glm::vec2& getPosition() const { return position; }

        void drawScreenGeometry(UIRenderer& uiRenderer,
                        TextRenderer& textRenderer,
                        const Font& font) const;
        void drawScreenText(TextRenderer& textRenderer, const Font& font) const;

        void drawWorldGeometry(UIRenderer& uiRenderer,
                            TextRenderer& textRenderer,
                            const Font& font) const;
        void drawWorldText(TextRenderer& textRenderer,const Font& font) const;

    private:
        std::string text;
        glm::vec2 position{0.0f, 0.0f};

        float scale = 1.0f;
        glm::vec4 textColor{1.0f, 1.0f, 1.0f, 1.0f};

        glm::vec2 padding{6.0f, 4.0f};
        bool backgroundEnabled = false;
        glm::vec4 backgroundColor{0.08f, 0.08f, 0.10f, 0.90f};

        bool borderEnabled = false;
        glm::vec4 borderColor{0.85f, 0.80f, 0.55f, 1.0f};
};