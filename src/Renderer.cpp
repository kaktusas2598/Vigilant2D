#include "Renderer.hpp"

#include "ErrorHandler.hpp"
#include "Vertex.hpp"

#include <cstddef>
#include "glm/gtc/matrix_transform.hpp"

void Renderer::init() {
    // TEST Render code below: Quad setup
    // TODO: need encapsulate quads for sprite and tile rendering later, particle engine render separate?
    shader = new Shader("shaders/texturedQuad.vert", "shaders/texturedQuad.frag");
    texture = new Texture("assets/textures/crate.png");

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
    // TODO: get rid of Texture dependency in Renderer, eventually introduce AssetManager
    texture->bind();

    shader->setUniform1i("spriteTexture", 0);
    shader->setUniformVec4("color", params.color);
    shader->setUniformMat4f("viewProjection", viewProjection);
    shader->setUniformMat4f("model", params.transform.toMatrix());
    shader->setUniformVec2("uvMin", params.region.uvMin);
    shader->setUniformVec2("uvMax", params.region.uvMax);

    quadMesh->draw();
    shader->unbind();
}