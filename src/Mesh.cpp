#include "Mesh.hpp"

#include <cstddef>

#include "ErrorHandler.hpp"

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    init(vertices, indices);
}

Mesh::~Mesh() {
    if (eboID != 0)
        GLCall(glDeleteBuffers(1, &eboID));
    if (vboID != 0)
        GLCall(glDeleteBuffers(1, &vboID));
    if (vaoID != 0)
        GLCall(glDeleteVertexArrays(1, &vaoID));
}

void Mesh::init(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    indexCount = static_cast<unsigned int>(indices.size());

    GLCall(glGenVertexArrays(1, &vaoID));
    GLCall(glGenBuffers(1, &vboID));
    GLCall(glGenBuffers(1, &eboID));

    GLCall(glBindVertexArray(vaoID));

    GLCall(glBindBuffer(GL_ARRAY_BUFFER, vboID));
    GLCall(glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)),
        vertices.data(),
        GL_STATIC_DRAW
    ));

    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID));
    GLCall(glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)),
        indices.data(),
        GL_STATIC_DRAW
    ));

    GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position)));
    GLCall(glEnableVertexAttribArray(0));

    GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv)));
    GLCall(glEnableVertexAttribArray(1));

    GLCall(glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color)));
    GLCall(glEnableVertexAttribArray(2));

    GLCall(glBindVertexArray(0));
}

void Mesh::bind() const {
    GLCall(glBindVertexArray(vaoID));
}

void Mesh::unbind() const {
    GLCall(glBindVertexArray(0));
}

void Mesh::draw() const {
    bind();
    GLCall(glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr));
}

Mesh Mesh::createQuad() {
    std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
        {{ 0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
        {{ 0.5f,  0.5f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
        {{-0.5f,  0.5f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}}
    };

    std::vector<unsigned int> indices = {
        0, 1, 2,
        2, 3, 0
    };

    return Mesh(vertices, indices);
}
