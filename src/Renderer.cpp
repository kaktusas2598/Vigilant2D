#include "Renderer.hpp"

#include "ErrorHandler.hpp"

void Renderer::init() {
    // TESTING Rendering Setup Code
    shader = new Shader("shaders/triangleTest.vert", "shaders/triangleTest.frag");

    glm::vec3 vertices[] = {
        {-0.5, -0.5, 0.0},
        {0.5, -0.5, 0.0},
        {0.0,  0.5, 0.0}
    };

    GLCall(glGenBuffers(1, &vboID));
    GLCall(glGenVertexArrays(1, &vaoID));

    GLCall(glBindVertexArray(vaoID));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, vboID));
    GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

    GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0));
    GLCall(glEnableVertexAttribArray(0));
}

void Renderer::render() {
    shader->bind();
    GLCall(glBindVertexArray(vaoID));
    GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
    shader->unbind();
}

void Renderer::exit() {
}

