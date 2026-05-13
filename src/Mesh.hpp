#pragma once

#include <vector>
#include <GL/glew.h>

#include "Vertex.hpp"

class Mesh {
    public:
        Mesh() = default;
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
        ~Mesh();

        // Make mesh non-copyable and movable
        Mesh(const Mesh &) = delete;
        Mesh &operator=(const Mesh &) = delete;

        Mesh(Mesh &&other) noexcept;
        Mesh &operator=(Mesh &&other) noexcept;

        // Static mesh 
        void init(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
        void initDynamic(size_t maxVertices, size_t maxIndices);
        void upload(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

        void bind() const;
        void unbind() const;
        void draw() const;

        static Mesh createQuad();
        static Mesh createFullscreenQuad();
    private:
        void setupVertexAttributes();

        unsigned int vaoID = 0;
        unsigned int vboID = 0;
        unsigned int eboID = 0;
        unsigned int indexCount = 0;

        bool dynamic = false;
        size_t vertexCapacity = 0;
        size_t indexCapacity = 0;
        // std::vector<Vertex> vertices;
        // std::vector<unsigned int> indices;
};