#include "PostProcessPass.hpp"

void PostProcessPass::init() {
    shader = Shader("shaders/postProcess.vert", "shaders/postProcess.frag");
    fullscreenQuad = Mesh(Mesh::createFullscreenQuad());
}

void PostProcessPass::draw(Texture* sourceTexture,
            float vignetteStrength,
            float contrast,
            const glm::vec3& tint) {
    if (sourceTexture == nullptr)
        return;
    
    shader.bind();
    sourceTexture->bind();

    shader.setUniform1i("sceneTexture", 0);
    shader.setUniform1f("vignetteStrength", vignetteStrength);
    shader.setUniform1f("contrast", contrast);
    shader.setUniformVec3("tint", tint);

    fullscreenQuad.draw();
    shader.unbind();
}
