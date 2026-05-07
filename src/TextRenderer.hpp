#pragma once

#include <string>

#include "Camera2D.hpp"
#include "Font.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include "glm/glm.hpp"

class TextRenderer{
    public:
        TextRenderer() = default;
        ~TextRenderer() = default;

        void init();
        void begin(const Camera2D& camera);
        void beginScreen(int viewportWidth, int viewportHeight);
        void end();

        void drawText(const Font& font,
             const std::string& text, const glm::vec2& position, float scale, const glm::vec4& color);
    private:
        Mesh quadMesh;
        Shader shader;
        Camera2D uiCamera;
        glm::mat4 viewProjection{1.0f};
};