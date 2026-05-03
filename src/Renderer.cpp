#include "Renderer.hpp"

#include "ErrorHandler.hpp"
#include "Vertex.hpp"

#include <cstddef>
#include "glm/gtc/matrix_transform.hpp"

void Renderer::init() {
    // TODO: probably not a good idea to have statuc shader in renderer in case we need more rendering options later
    shader = new Shader("shaders/texturedQuad.vert", "shaders/texturedQuad.frag");

    quadMesh = new Mesh(Mesh::createQuad());
}

void Renderer::begin(const Camera2D& camera) {
    viewProjection = camera.getViewProjectionMatrix();

    currentTexture = nullptr;
    shader->bind();
}

void Renderer::end() {
    shader->unbind();
}

Renderer::~Renderer() {
    if (shader != nullptr)
        delete(shader);

    if (quadMesh != nullptr)
        delete(quadMesh);

}

void Renderer::drawQuad(const QuadDrawParams &params) {
    const bool useTexture = params.region.texture != nullptr;
    shader->setUniform1i("useTexture", useTexture ? 1 : 0);

    if (useTexture) {
        // Temporary texture caching on renderer side
        if (currentTexture != params.region.texture) {
            params.region.texture->bind();
            currentTexture = params.region.texture;
        }

        shader->setUniform1i("spriteTexture", 0);
        shader->setUniformVec2("uvMin", params.region.uvMin);
        shader->setUniformVec2("uvMax", params.region.uvMax);
    }

    shader->setUniformVec4("color", params.color);
    shader->setUniformMat4f("viewProjection", viewProjection);
    shader->setUniformMat4f("model", params.transform.toMatrix());
    shader->setUniform1i("remapUVs", 1);
    
    quadMesh->draw();
}

void Renderer::drawMesh(const Mesh& mesh, Texture* texture,
                        const glm::mat4& model,
                        const glm::vec4& color) {
    if (texture != currentTexture) {
        if (texture != nullptr) {
            texture->bind();
        }
        currentTexture = texture;
    }

    shader->setUniform1i("useTexture", texture != nullptr ? 1 : 0);
    shader->setUniform1i("spriteTexture", 0);
    shader->setUniformVec4("color", color);
    shader->setUniformMat4f("viewProjection", viewProjection);
    shader->setUniformMat4f("model", model);
    shader->setUniform1i("remapUVs", 0);
    
    mesh.draw();
}
