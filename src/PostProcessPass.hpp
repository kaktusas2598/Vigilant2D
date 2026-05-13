#pragma once

#include "Mesh.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "glm/glm.hpp"

class PostProcessPass {
public:
    void init();
    void draw(Texture* sourceTexture,
              float vignetteStrength,
              float contrast,
              const glm::vec3& tint);

private:
    Shader shader;
    Mesh fullscreenQuad;
};
