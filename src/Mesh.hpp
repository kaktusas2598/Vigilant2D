#pragma once

#include <vector>
#include <GL/glew.h>

#include "Vertex.hpp"

class Mesh {
    public:
        Mesh() = default;
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
        ~Mesh();

        void init(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
        void bind() const;
        void unbind() const;
        void draw() const;

        static Mesh createQuad();
    private:
        unsigned int vaoID = 0;
        unsigned int vboID = 0;
        unsigned int eboID = 0;
        unsigned int indexCount = 0;

        // std::vector<Vertex> vertices;
        // std::vector<unsigned int> indices;
};