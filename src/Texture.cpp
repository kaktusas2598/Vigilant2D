#include "Texture.hpp"

#include "ErrorHandler.hpp"
#include "stb_image/stb_image.h"
#include <iostream>

bool Texture::load2D(const std::string& fileName, const TextureParams& params) {
    destroy();
    target = GL_TEXTURE_2D;

    stbi_set_flip_vertically_on_load(params.flipY ? 1 : 0);
    localBuffer = stbi_load(fileName.c_str(), &width, &height, &BPP, 4);

    if (!localBuffer) {
        return false;
    }

    GLCall(glGenTextures(1, &rendererID));
    GLCall(glBindTexture(target, rendererID));

    apply2DParams(params);

    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer));

    if (params.generateMipmaps) {
        GLCall(glGenerateMipmap(GL_TEXTURE_2D));
    }

    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
    stbi_image_free(localBuffer);
    localBuffer = nullptr;

    return true;
}
bool Texture::loadCubemap(std::vector<std::string> faces, const CubeMapParams& params) {
    destroy();

    target = GL_TEXTURE_CUBE_MAP;
    GLCall(glGenTextures(1, &rendererID));
    GLCall(glBindTexture(target, rendererID));

    applyCubemapParams(params);

    for (size_t i = 0; i < faces.size(); i++) {
        int faceWidth = 0;
        int faceHeight = 0;
        int faceBPP = 0;
        unsigned char* data = stbi_load(faces[i].c_str(), &faceWidth, &faceHeight, &faceBPP, 0);

        if (!data) {
            GLCall(glBindTexture(target, 0));
            return false;
        }

        GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                            GL_RGBA, faceWidth, faceHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
        stbi_image_free(data);

        width = faceWidth;
        height = faceHeight;
    }

    if (params.generateMipmaps) {
        GLCall(glGenerateMipmap(target));
    }

    GLCall(glBindTexture(target, 0));
    return true;
}

void Texture::initEmpty2D(int w, int h, GLint internalFormat, GLenum format, GLenum type, const TextureParams& params) {
    destroy();

    target = GL_TEXTURE_2D;
    width = w;
    height = h;

    GLCall(glGenTextures(1, &rendererID));
    GLCall(glBindTexture(target, rendererID));
    apply2DParams(params);

    GLCall(glTexImage2D(target, 0, internalFormat, width, height, 0, format, type, nullptr));
    GLCall(glBindTexture(target, 0));
}

void Texture::initEmptyCubemap(int w, int h,
                               GLint internalFormat,
                               GLenum format,
                               GLenum type,
                               const CubeMapParams& params) {
    destroy();
    target = GL_TEXTURE_CUBE_MAP;
    width = w;
    height = h;

    GLCall(glGenTextures(1, &rendererID));
    GLCall(glBindTexture(target, rendererID));
    applyCubemapParams(params);

    for (unsigned int i = 0; i < 6; ++i) {
        GLCall(glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0,
            internalFormat,
            width,
            height,
            0,
            format,
            type,
            nullptr
        ));
    }

    GLCall(glBindTexture(target, 0));
}


void Texture::initDepthCubemap(int w, int h, const CubeMapParams& params) {
    destroy();
    target = GL_TEXTURE_CUBE_MAP;
    GLCall(glGenTextures(1, &rendererID));
    GLCall(glBindTexture(target, rendererID));

    applyCubemapParams(params);

    for (size_t i = 0; i < 6; i++) {
        GLCall(glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0,
            GL_DEPTH_COMPONENT,
            width,
            height,
            0, 
            GL_DEPTH_COMPONENT,
            GL_FLOAT,
            nullptr
        ));
    }

    GLCall(glBindTexture(target, 0));
}

Texture::~Texture() {
    destroy();
}

void Texture::bind(unsigned int slot) const {
    GLCall(glActiveTexture(GL_TEXTURE0 + slot));
    GLCall(glBindTexture(target, rendererID));
}

void Texture::unbind() const {
    GLCall(glBindTexture(target, 0));
}

void Texture::destroy() {
    if (rendererID != 0) {
        GLCall(glDeleteTextures(1, &rendererID));
        rendererID = 0;
    }

    if (localBuffer != nullptr) {
        stbi_image_free(localBuffer);
        localBuffer = nullptr;
    }

    width = 0;
    height = 0;
    BPP = 0;
    filePath.clear();
    target = GL_TEXTURE_2D;
}

void Texture::apply2DParams(const TextureParams& params) {
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, params.magFilter));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, params.minFilter));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, params.wrapS));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, params.wrapT));
}

void Texture::applyCubemapParams(const CubeMapParams& params) {
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, params.minFilter));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, params.magFilter));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, params.wrapS));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, params.wrapT));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, params.wrapR));
}
