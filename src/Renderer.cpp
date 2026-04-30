#include "Renderer.hpp"

#include "ErrorHandler.hpp"
#include "Vertex.hpp"

#include <cstddef>
#include "glm/gtc/matrix_transform.hpp"

void Renderer::init() {
    // TEST Render code below: Quad setup
    // TODO: probably not a good idea to have statuc shader in renderer in case we need more rendering options later
    shader = new Shader("shaders/texturedQuad.vert", "shaders/texturedQuad.frag");

    quadMesh = new Mesh(Mesh::createQuad());
}

void Renderer::begin(const Camera2D& camera) {
    viewProjection = camera.getViewProjectionMatrix();
}

void Renderer::exit() {
}

Renderer::~Renderer() {
    if (shader != nullptr)
        delete(shader);

    if (quadMesh != nullptr)
        delete(quadMesh);

}

void Renderer::drawQuad(const QuadDrawParams &params) {
    shader->bind();

    const bool useTexture = params.region.texture != nullptr;
    shader->setUniform1i("useTexture", useTexture ? 1 : 0);

    if (useTexture) {
        params.region.texture->bind();
        shader->setUniform1i("spriteTexture", 0);
        shader->setUniformVec2("uvMin", params.region.uvMin);
        shader->setUniformVec2("uvMax", params.region.uvMax);
    }

    shader->setUniformVec4("color", params.color);
    shader->setUniformMat4f("viewProjection", viewProjection);
    shader->setUniformMat4f("model", params.transform.toMatrix());
    
    quadMesh->draw();
    shader->unbind();

    if (useTexture) {
        params.region.texture->unbind();
    }
}