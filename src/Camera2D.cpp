#include "Camera2D.hpp"

void Camera2D::setPosition(const glm::vec2 &newPosition) { position = newPosition; }

void Camera2D::move(const glm::vec2 &delta) { position += delta; }

void Camera2D::setZoom(float newZoom) {
    if (newZoom < 0.25f) newZoom = 0.25f;
    if (newZoom > 2.5f) newZoom = 2.5f;
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
