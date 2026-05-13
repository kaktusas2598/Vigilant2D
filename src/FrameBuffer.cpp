#include "FrameBuffer.hpp"

#include "ErrorHandler.hpp"

FrameBuffer::~FrameBuffer() {
    destroy();
}

bool FrameBuffer::createColor(int w, int h) {
    destroy();
    width = w;
    height = h;

    GLCall(glGenFramebuffers(1, &fboID));
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, fboID));

    attachColorTexture(width, height);
    createDepthStencilRenderBuffer();

    const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    GLCall(glDrawBuffers(1, drawBuffers));

    const bool complete = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    if (!complete) {
        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        destroy();
        return false;
    }

    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    return true;
}

void FrameBuffer::destroy() {
    for (Texture* texture : colorTextures) {
        delete texture;
    }
    colorTextures.clear();

    if (rboID != 0) {
        GLCall(glDeleteRenderbuffers(1, &rboID));
        rboID = 0;
    }

    if (fboID != 0) {
        GLCall(glDeleteFramebuffers(1, &fboID));
        fboID = 0;
    }
}

void FrameBuffer::createDepthStencilRenderBuffer() {
    GLCall(glGenRenderbuffers(1, &rboID));
    GLCall(glBindRenderbuffer(GL_RENDERBUFFER, rboID));
    GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height));
    GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboID));
}

void FrameBuffer::attachColorTexture(int w, int h) {
    auto* texture = new Texture();

    TextureParams params;
    params.minFilter = GL_LINEAR;
    params.magFilter = GL_LINEAR;
    params.wrapS = GL_CLAMP_TO_EDGE;
    params.wrapT = GL_CLAMP_TO_EDGE;
    params.generateMipmaps = false;

    texture->initEmpty2D(w, h, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, params);
    colorTextures.push_back(texture);

    GLCall(glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        texture->getID(),
        0
    ));
}

void FrameBuffer::rescale(int w, int h) {
    if (w == width && h == height)
        return;

    createColor(w, h);
}

void FrameBuffer::bind() const {
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, fboID));
    // Good idea everytime binding to new render target to set size accordingly
    GLCall(glViewport(0, 0, width, height));
}

void FrameBuffer::unbind() const {
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}
