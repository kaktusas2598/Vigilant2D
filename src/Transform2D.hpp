#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

struct Transform2D {
    glm::vec2 position{0.0f, 0.0f};
    glm::vec2 scale{1.0f, 1.0f};
    float rotation = 0.0f;

    // Generated Model matrix
    glm::mat4 toMatrix() const {
        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(position, 0.0f));
        model = glm::rotate(model, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(scale, 1.0f));
        return model;
    }
};
