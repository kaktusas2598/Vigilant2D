#include "TextRenderer.hpp"

#include "Transform2D.hpp"

void TextRenderer::init() {
    shader = Shader("shaders/text.vert", "shaders/text.frag");
    quadMesh = Mesh(Mesh::createQuad());
}

void TextRenderer::begin(const Camera2D& camera) {
    viewProjection = camera.getViewProjectionMatrix();
    shader.bind();
}

void TextRenderer::beginScreen(int viewportWidth, int viewportHeight) {
    uiCamera.setViewportSize(static_cast<float>(viewportWidth), static_cast<float>(viewportHeight));
    uiCamera.setZoom(1.0f);
    uiCamera.setPosition({
        viewportWidth * 0.5f,
        viewportHeight * 0.5f
    });

    begin(uiCamera);
}

void TextRenderer::end() {
    shader.unbind();
}

void TextRenderer::drawText(const Font& font,
        const std::string& text, const glm::vec2& position, float scale, const glm::vec4& color) {
    shader.setUniformMat4f("viewProjection", viewProjection);
    shader.setUniformVec4("textColor", color);
    shader.setUniform1i("glyphTexture", 0);

    glm::vec2 pen = position;
    for (char c : text) {
        const Glyph* glyph = font.getGlyph(c);
        if (glyph == nullptr)
            continue;
        
        const float xpos = pen.x + glyph->bearing.x * scale;
        const float ypos = pen.y - (glyph->size.y - glyph->bearing.y) * scale;
        const float w = glyph->size.x * scale;
        const float h = glyph->size.y * scale;

        Transform2D transform;
        transform.position = {xpos, ypos};
        transform.scale = {w, h};

        glyph->texture.bind(0);
        shader.setUniformMat4f("model", transform.toMatrix());

        // TODO: this is horrible, text rendering should be batched!
        quadMesh.draw();
        pen.x += (glyph->advance >> 6) * scale;
    }
}