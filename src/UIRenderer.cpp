#include "UIRenderer.hpp"

#include "Renderer.hpp"
#include "Transform2D.hpp"


UIRenderer::UIRenderer(Renderer& renderer) : renderer(renderer) {}

void UIRenderer::beginScreen(int viewportWidth, int viewportHeight) {
    currentSpace = UIRenderSpace::Screen;
    uiCamera.setViewportSize(static_cast<float>(viewportWidth), static_cast<float>(viewportHeight));
    uiCamera.setZoom(1.0f);
    uiCamera.setPosition({
        viewportWidth * 0.5f,
        viewportHeight * 0.5f
    });
}

void UIRenderer::beginWorld(const Camera2D& camera) {
    currentSpace = UIRenderSpace::World;
}

void UIRenderer::drawQuad(const UIQuad& quad) {
    Transform2D transform;
    transform.scale = quad.rect.size;
    transform.position = quad.rect.position;

    renderer.drawQuad({
        transform,
        quad.region,
        quad.color
    });
}

void UIRenderer::drawQuadOutline(const UIQuadOutline& outline) {
    const glm::vec2 pos = outline.rect.position;
    const glm::vec2 size = outline.rect.size;
    const float t = outline.thickness;

    drawQuad({{{pos.x, pos.y}, {size.x, t}}, TextureRegion::full(nullptr), outline.color});
    drawQuad({{{pos.x, pos.y + size.y - t}, {size.x, t}}, TextureRegion::full(nullptr), outline.color});
    drawQuad({{{pos.x, pos.y}, {t, size.y}}, TextureRegion::full(nullptr), outline.color});
    drawQuad({{{pos.x + size.x - t, pos.y}, {t, size.y}}, TextureRegion::full(nullptr), outline.color});
}

void UIRenderer::end() {
    // TODO: use later for flushing batches, clipping, state, etc
    // Curently only marks end of UI phrase and renderer pass ownership lives outside
}

