#pragma once

#include <vector>
#include "Texture.hpp"

class FrameBuffer {
    public:
        FrameBuffer() = default;
        ~FrameBuffer();

        FrameBuffer(const FrameBuffer&) = delete;
        FrameBuffer operator=(const FrameBuffer&) = delete;

        // Create colour attachment framebuffer
        bool createColor(int w, int h);
        void destroy();
        void rescale(int w, int h);

        void bind() const;
        void unbind() const;

        int getWidth() const { return width; }
        int getHeight() const { return height; }

        Texture* getColorTexture() const { 
            return colorTextures.empty() ? nullptr : colorTextures[0]; 
        }

    private:
        void createDepthStencilRenderBuffer();
        void attachColorTexture(int w, int h);

        GLuint fboID = 0; //<<< framebuffer id
        GLuint rboID = 0; //<<< renderbuffer id
        int width = 0, height = 0; //<<< framebuffer size
        std::vector<Texture*> colorTextures;
};