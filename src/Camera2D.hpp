#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Camera2D {
    public:
        Camera2D(float viewportWidth = 1280.0f, float viewportHeight = 720.0f)
            : position(0.0f, 0.0f), zoom(1.0f), viewportWidth(viewportWidth), viewportHeight(viewportHeight) {}

        void setPosition(const glm::vec2& newPosition);
        void move(const glm::vec2& delta);
        void setZoom(float newZoom);
        void setViewportSize(float width, float height);

        glm::mat4 getViewMatrix() const;
        glm::mat4 getProjectionMatrix() const;
        glm::mat4 getViewProjectionMatrix() const;

        glm::vec2 getPosition() const { return position; }
        float getZoom() const { return zoom; }
        float getViewportWidth() const { return viewportWidth; }
        float getViewportHeight() const { return viewportHeight; }

        glm::vec2 screenToWorld(const glm::vec2& screenPosition) const;
        glm::vec2 worldToScreen(const glm::vec2& worldPosition) const;

        void setTargetPosition(const glm::vec2& target) {
            targetPosition = target;
            hasTargetPosition = true;
        }
        void clearTargetPosition() {
            hasTargetPosition = false;
        }
        bool hasTarget() const { return hasTargetPosition; }
        void updateTarget() {
            if (hasTargetPosition) {
                position = targetPosition;
            }
        }

    private:
        glm::vec2 position;
        float zoom;
        float viewportWidth;
        float viewportHeight;

        bool hasTargetPosition = false;
        glm::vec2 targetPosition{0.0f, 0.0f};
};