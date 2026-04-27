#pragma once

#include "Shader.hpp"
#include "Camera2D.hpp"

class Renderer{
    public:
        void init();
        void render(Camera2D& camera);
        void exit();

    private:
        unsigned int vaoID, vboID, eboID;
        Shader* shader;
};