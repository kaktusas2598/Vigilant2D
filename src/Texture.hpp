#pragma once

#include <GL/glew.h>
#include <string>
#include <vector>

// TODO: Instead of these shitty constructor overloads below, just pass filename and const& to Params object
struct TextureParams {
    GLint wrapS = GL_CLAMP_TO_EDGE;
    GLint wrapT = GL_CLAMP_TO_EDGE;
    GLint minFilter = GL_LINEAR;
    GLint magFilter = GL_LINEAR;
    bool generateMipmaps = true;
    bool flipY = true;
};

static TextureParams PixelArt() {
    TextureParams pixelArtParams;
    pixelArtParams.minFilter = GL_NEAREST;
    pixelArtParams.magFilter = GL_NEAREST;
    pixelArtParams.generateMipmaps = false;
    return pixelArtParams;
}
static TextureParams Smooth() {
    TextureParams smoothParams;
    smoothParams.minFilter = GL_LINEAR_MIPMAP_LINEAR;
    return smoothParams;
}

class Texture {
    public:
        // Initialise empty texture, used by framebuffers
        Texture(GLenum targetType = GL_TEXTURE_2D, unsigned char* data = nullptr) : rendererID(0), target(targetType), localBuffer(data) {}
        void init(int w, int h, unsigned int id = 0, GLfloat filter = GL_LINEAR, GLint internalFormat = GL_RGBA, GLenum format = GL_RGBA, bool clamp = false);
        // Created specifically to init depth cubemap used for omnidirectional shadow mapping
        void initCubeMap(int w, int h, unsigned int id = 0, GLfloat filter = GL_NEAREST, GLint internalFormat = GL_DEPTH_COMPONENT16, GLenum format = GL_DEPTH_COMPONENT);

        // 2D Texture
        Texture(const std::string& fileName, GLint wrappingMode = GL_CLAMP_TO_EDGE);

        // Cubemap Texture
        Texture(std::vector<std::string> faces);

        ~Texture();

        void bind(unsigned int slot = 0) const;
        void unbind() const;

        unsigned char* getData() { return localBuffer; }
        void setData(unsigned char* data) { localBuffer = data; }

        inline int getWidth() const { return width; }
        inline int getHeight() const { return height; }

        void setType(std::string& t) { type = t; }
        inline std::string getType() const { return type; }

        unsigned int getID() { return rendererID; }
    private:
        unsigned int rendererID;
        std::string filePath;
        unsigned char* localBuffer;
        int width, height, BPP; // Bits per picture
        // Available types: texture_diffuse, texture_specular, texture_normal
        std::string type;
        GLenum target; ///< GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP
};
