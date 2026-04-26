#pragma once

#include "Shader.hpp"

class Renderer{
    public:
        void init();
        void render();
        void exit();

    private:
        unsigned int vaoID, vboID;
        Shader* shader;
};