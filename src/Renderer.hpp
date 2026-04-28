#pragma once

#include "Camera2D.hpp"

#include "Shader.hpp"
#include "Mesh.hpp"
#include "Texture.hpp"

class Renderer{
    public:
        ~Renderer();
        void init();
        void render(Camera2D& camera);
        void exit();

    private:
        Mesh* quadMesh = nullptr;
        Shader* shader = nullptr;
};