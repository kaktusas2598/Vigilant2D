#pragma once

#include "Camera2D.hpp"

#include "Shader.hpp"
#include "Mesh.hpp"
#include "TextureRegion.hpp"
#include "Transform2D.hpp"

struct QuadDrawParams {
    Transform2D transform;
    TextureRegion region;
    glm::vec4 color{1.0f, 1.0f, 1.0f, 1.0f};
    bool flipX = false;
    bool flipY = false;
};

class Renderer{
    public:
        ~Renderer();
        void init();
        void begin(const Camera2D& camera);
        void end();

        void drawQuad(const QuadDrawParams &params);
        void drawQuad(const TextureRegion& region, const Transform2D& transform) {
            drawQuad({transform, region});
        }
        // Plain, non textured quad
        void drawQuad(const Transform2D& transform) {
            drawQuad({transform, TextureRegion::full(nullptr)});
        }

        void drawMesh(const Mesh& mesh,
                    Texture* texture,
                    const glm::mat4& model = glm::mat4(1.0f),
                    const glm::vec4& color = glm::vec4(1.0f));

    private:
        glm::mat4 viewProjection{1.0f};

        Mesh* quadMesh = nullptr;
        Shader* shader = nullptr;
        // Also temporary at the moment for texture caching in renderer
        Texture* currentTexture = nullptr;
};