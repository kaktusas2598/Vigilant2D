#include "Camera2D.hpp"

#include <algorithm>

void Camera2D::setPosition(const glm::vec2 &newPosition) { position = newPosition; }

void Camera2D::move(const glm::vec2 &delta) { position += delta; }

void Camera2D::setZoom(float newZoom) {
    if (newZoom < 1.0f) newZoom = 1.0f;
    if (newZoom > 4.0f) newZoom = 4.0f;
    zoom = newZoom;
}

void Camera2D::setViewportSize(float width, float height) {
    viewportWidth = width;
    viewportHeight = height;
}

glm::mat4 Camera2D::getViewMatrix() const {
    return glm::translate(glm::mat4(1.0f), glm::vec3(-position, 0.0f));
}

glm::mat4 Camera2D::getProjectionMatrix() const {
    float halfWidth = viewportWidth * 0.5f / zoom;
    float halfHeight = viewportHeight * 0.5f / zoom;

    return glm::ortho(
        -halfWidth, halfWidth,
        -halfHeight, halfHeight,
        -1.0f, 1.0f);
}

glm::mat4 Camera2D::getViewProjectionMatrix() const {
    return getProjectionMatrix() * getViewMatrix();
}

glm::vec2 Camera2D::screenToWorld(const glm::vec2 &screenPosition) const {
    const float worldWidth = viewportWidth / zoom;
    const float worldHeight = viewportHeight / zoom;
    const float left = position.x - worldWidth * 0.5f;
    const float bottom = position.y - worldHeight * 0.5f;

    return {  
        left + screenPosition.x / zoom,
        bottom + (viewportHeight - screenPosition.y) / zoom
    };
}

glm::vec2 Camera2D::worldToScreen(const glm::vec2 &worldPosition) const {
    const float worldWidth = viewportWidth / zoom;
    const float worldHeight = viewportHeight / zoom;
    const float left = position.x - worldWidth * 0.5f;
    const float bottom = position.y - worldHeight * 0.5f;

    return {
        (worldPosition.x - left) * zoom,
        viewportHeight - ((worldPosition.y - bottom) * zoom)
    };
}

glm::vec2 Camera2D::clampPositionToWorldBounds(const glm::vec2& desiredPosition,
                                               float worldWidth,
                                               float worldHeight) const {
    const float halfViewWidth = viewportWidth * 0.5f / zoom;
    const float halfViewHeight = viewportHeight * 0.5f / zoom;

    glm::vec2 clamped = desiredPosition;

    if (worldWidth <= halfViewWidth * 2.0f) {
        clamped.x = worldWidth * 0.5f;
    } else {
        clamped.x = std::clamp(clamped.x, halfViewWidth, worldWidth - halfViewWidth);
    }

    if (worldHeight <= halfViewHeight * 2.0f) {
        clamped.y = worldHeight * 0.5f;
    } else {
        clamped.y = std::clamp(clamped.y, halfViewHeight, worldHeight - halfViewHeight);
    }

    return clamped;
}