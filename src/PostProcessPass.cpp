#include "PostProcessPass.hpp"

void PostProcessPass::init() {
    shader = Shader("shaders/postProcess.vert", "shaders/postProcess.frag");
    fullscreenQuad = Mesh(Mesh::createFullscreenQuad());
}

void PostProcessPass::draw(Texture* sourceTexture,
              float vignetteStrength,
              float contrast,
              float brightness,
              float saturation,
              const glm::vec3& tint,
              const glm::vec4& fadeColor,
              float fadeAmount) {
    if (sourceTexture == nullptr)
        return;
    
    shader.bind();
    sourceTexture->bind();

    shader.setUniform1i("sceneTexture", 0);
    shader.setUniform1f("vignetteStrength", vignetteStrength);
    shader.setUniform1f("contrast", contrast);
    shader.setUniform1f("brightness", brightness);
    shader.setUniform1f("saturation", saturation);
    shader.setUniformVec3("tint", tint);
    shader.setUniformVec4("fadeColor", fadeColor);
    shader.setUniform1f("fadeAmount", fadeAmount);

    fullscreenQuad.draw();
    shader.unbind();
}
