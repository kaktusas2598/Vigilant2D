#include "Renderer.hpp"

#include "ErrorHandler.hpp"
#include "Vertex.hpp"

#include <cstddef>
#include "glm/gtc/matrix_transform.hpp"

void Renderer::init() {
    // TEST Render code below: Quad setup
    // TODO: need encapsulate quads for sprite and tile rendering later, particle engine render separate?
    shader = new Shader("shaders/texturedQuad.vert", "shaders/texturedQuad.frag");

    Vertex vertices[4] = {
        {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.0f}, {0.0f, 1.0f}}};

    unsigned int indices[6] = {
        0, 1, 2,
        2, 3, 0
    };

    GLCall(glGenBuffers(1, &vboID));
    GLCall(glGenBuffers(1, &eboID));
    GLCall(glGenVertexArrays(1, &vaoID));

    GLCall(glBindVertexArray(vaoID));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, vboID));
    GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

    GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position)));
    GLCall(glEnableVertexAttribArray(0));

    GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv)));
    GLCall(glEnableVertexAttribArray(1));
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


    GLCall(glBindVertexArray(vaoID));
    GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
    shader->unbind();
}

void Renderer::exit() {
}

