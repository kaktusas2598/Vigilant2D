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

struct CubeMapParams {
    GLint wrapS = GL_CLAMP_TO_EDGE;
    GLint wrapT = GL_CLAMP_TO_EDGE;
    GLint wrapR = GL_CLAMP_TO_EDGE;
    GLint minFilter = GL_LINEAR;
    GLint magFilter = GL_LINEAR;
    bool generateMipmaps = false;
};

class Texture {
    public:
        Texture() = default;
        ~Texture();

        Texture(const Texture &) = delete;
        Texture &operator=(const Texture &) = delete;

        Texture(Texture &&other) noexcept;
        Texture &operator=(Texture &&other) noexcept;

        bool load2D(const std::string& fileName, const TextureParams& params = PixelArt());
        bool loadCubemap(std::vector<std::string> faces, const CubeMapParams& params = {});
        void initEmpty2D(int w, int h, 
            GLint internalFormat = GL_RGBA, 
            GLenum format = GL_RGBA, 
            GLenum type = GL_UNSIGNED_BYTE,
            const TextureParams& params = {}
        );
        void initFromBuffer2D(int w, int h, 
            GLint internalFormat, 
            GLenum format, 
            GLenum type,
            const void* data,
            const TextureParams& params = {}
        );
        void initEmptyCubemap(int w, int h,
            GLint internalFormat = GL_RGBA,
            GLenum format = GL_RGBA,
            GLenum type = GL_UNSIGNED_BYTE,
            const CubeMapParams& params = {});

        void initDepthCubemap(int w, int h, const CubeMapParams& params = {});

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
        void destroy();
        void apply2DParams(const TextureParams& params);
        void applyCubemapParams(const CubeMapParams& params);

        unsigned int rendererID = 0;
        std::string filePath;
        unsigned char* localBuffer = nullptr;
        int width, height, BPP; // Bits per picture
        // Available types: texture_diffuse, texture_specular, texture_normal
        std::string type;
        GLenum target; ///< GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP
};
