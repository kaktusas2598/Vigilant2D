#include "Renderer.hpp"

#include "ErrorHandler.hpp"
#include "Vertex.hpp"

#include <cstddef>
#include "glm/gtc/matrix_transform.hpp"

void Renderer::init() {
    // TEST Render code below: Quad setup
    // TODO: need encapsulate quads for sprite and tile rendering later, particle engine render separate?
    shader = new Shader("shaders/texturedQuad.vert", "shaders/texturedQuad.frag");

    quadMesh = new Mesh(Mesh::createQuad());
}

void Renderer::render(Camera2D& camera) {
    shader->bind();

    // TODO: Is quad separate or does it contain transform and calculates Model matrix?
    glm::mat4 model = glm::mat4(1.0f);

    // Important: your quad is currently only 1x1 world units.
    // With a pixel-sized orthographic camera, this will be tiny unless scaled.
    model = glm::scale(model, glm::vec3(100.0f, 100.0f, 1.0f));

    shader->setUniformMat4f("projection", camera.getProjectionMatrix());
    shader->setUniformMat4f("view", camera.getViewMatrix());
    shader->setUniformMat4f("model", model);

    quadMesh->draw();
    shader->unbind();
}

void Renderer::exit() {
}

Renderer::~Renderer() {
    if (shader != nullptr)
        delete(shader);

    if (quadMesh != nullptr)
        delete(quadMesh);

}