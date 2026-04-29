#pragma once

#include "Camera2D.hpp"

#include "Shader.hpp"
#include "Mesh.hpp"
#include "Texture.hpp"
#include "TextureRegion.hpp"
#include "Transform2D.hpp"

struct QuadDrawParams {
    Transform2D transform;
    TextureRegion region;
    glm::vec4 color{1.0f, 1.0f, 1.0f, 1.0f};
};

class Renderer{
    public:
        ~Renderer();
        void init();
        void begin(const Camera2D& camera);
        void drawQuad(const QuadDrawParams& params);
        void end() {} // For now not needed
        void exit();

        void drawQuad(const TextureRegion& region, const Transform2D& transform) {
            drawQuad({transform, region});
        }
        void drawQuad(const Transform2D& transform) {
            drawQuad({transform, TextureRegion::full(nullptr)});
        }
        // void drawTile(const );
        // void drawQuad(Texture *texture, const Transform2D &transform);
        // void drawQuad(const QuadDrawParams &params);

    private:
        glm::mat4 viewProjection{1.0f};

        Mesh* quadMesh = nullptr;
        Shader* shader = nullptr;
        Texture* texture = nullptr;
};