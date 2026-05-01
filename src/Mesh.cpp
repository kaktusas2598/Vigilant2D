#include "Mesh.hpp"

#include <cstddef>
#include <stdexcept>

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
    dynamic = false;
    vertexCapacity = vertices.size();
    indexCapacity = indices.size();

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

    setupVertexAttributes();

    GLCall(glBindVertexArray(0));
}

void Mesh::initDynamic(size_t maxVertices, size_t maxIndices) {
    // destroy();
    dynamic = true;
    vertexCapacity = maxVertices;
    indexCapacity = maxIndices;

    GLCall(glGenVertexArrays(1, &vaoID));
    GLCall(glGenBuffers(1, &vboID));
    GLCall(glGenBuffers(1, &eboID));

    GLCall(glBindVertexArray(vaoID));

    GLCall(glBindBuffer(GL_ARRAY_BUFFER, vboID));
    GLCall(glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(vertexCapacity * sizeof(Vertex)),
        nullptr,
        GL_DYNAMIC_DRAW
    ));

    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID));
    GLCall(glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(indexCapacity * sizeof(unsigned int)),
        nullptr,
        GL_DYNAMIC_DRAW
    ));

    setupVertexAttributes();
    GLCall(glBindVertexArray(0));
}

void Mesh::setupVertexAttributes() {
    GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, position)));
    GLCall(glEnableVertexAttribArray(0));

    GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv)));
    GLCall(glEnableVertexAttribArray(1));

    GLCall(glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color)));
    GLCall(glEnableVertexAttribArray(2));
}

void Mesh::upload(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices) {
    if (!dynamic)
        throw std::runtime_error("Mesh::upload() can only be called on a dynamic mesh.");

    if (vertices.size() > vertexCapacity || indices.size() > indexCapacity)
        throw std::runtime_error("Mesh::upload() exceeded dynamic mesh capacity.");

    indexCount = static_cast<unsigned int>(indices.size());

    GLCall(glBindVertexArray(vaoID));

    GLCall(glBindBuffer(GL_ARRAY_BUFFER, vboID));
    GLCall(glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)),
        vertices.data()
    ));

    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID));
    GLCall(glBufferSubData(
        GL_ELEMENT_ARRAY_BUFFER,
        0,
        static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)),
        indices.data()
    ));
}

void Mesh::bind() const {
    GLCall(glBindVertexArray(vaoID));
}

void Mesh::unbind() const {
    GLCall(glBindVertexArray(0));
}

void Mesh::draw() const {
    if (indexCount == 0)
        return;

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
