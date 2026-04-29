#pragma once

#include "Camera2D.hpp"

#include "Shader.hpp"
#include "Mesh.hpp"
#include "Texture.hpp"
#include "Transform2D.hpp"

struct TextureRegion {
    Texture* texture = nullptr;
    glm::vec2 uvMin{0.0f, 0.0f};
    glm::vec2 uvMax{1.0f, 1.0f};
};

struct QuadDrawParams {
    Transform2D transform;
    Texture* texture = nullptr;
    glm::vec2 uvMin{0.0f, 0.0f};
    glm::vec2 uvMax{1.0f, 1.0f};
    glm::vec4 color{1.0f, 1.0f, 1.0f, 1.0f};
};

class Renderer{
    public:
        ~Renderer();
        void init();
        void begin(const Camera2D& camera);
        void render(); // TODO: refactour out
        void drawQuad(const QuadDrawParams& params);
        void end() {} // For now not needed
        void exit();

        void drawQuad(const Transform2D& transform) {
            drawQuad({transform, nullptr});
        }
        // void drawQuad(Texture *texture, const Transform2D &transform);
        // void drawQuad(const QuadDrawParams &params);

    private:
        glm::mat4 viewProjection{1.0f};

        Mesh* quadMesh = nullptr;
        Shader* shader = nullptr;
        Texture* texture = nullptr;
};