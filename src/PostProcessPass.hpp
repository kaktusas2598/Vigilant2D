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
              float brightness,
              float saturation,
              const glm::vec3& tint,
              const glm::vec4& fadeColor,
              float fadeAmount);

private:
    Shader shader;
    Mesh fullscreenQuad;
};
