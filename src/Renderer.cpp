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

void Renderer::render() {
    shader->bind();

    texture->bind();
    shader->setUniform1i("spriteTexture", 0);
    //fragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);

    // TODO: Is quad separate or does it contain transform and calculates Model matrix?
    glm::mat4 model = glm::mat4(1.0f);

    // Important: your quad is currently only 1x1 world units.
    // With a pixel-sized orthographic camera, this will be tiny unless scaled.
    model = glm::scale(model, glm::vec3(100.0f, 100.0f, 1.0f));

    shader->setUniformMat4f("viewProjection", viewProjection);
    shader->setUniformMat4f("model", model);

    quadMesh->draw();
    shader->unbind();

    // drawQuad({texture, {{1.0f, -25.0f}, {100.0f, 100.0f}}});
    // drawQuad({texture, {{10.0f, 15.0f}, {100.0f, 100.0f}}});
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
    texture->bind();

    shader->setUniform1i("spriteTexture", 0);
    shader->setUniformVec4("color", params.color);
    shader->setUniformMat4f("viewProjection", viewProjection);
    shader->setUniformMat4f("model", params.transform.toMatrix());

    quadMesh->draw();
    shader->unbind();
}