#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdio>

class Window {
    public:
        Window() {}
        ~Window();

        void init(int width, int height);
        void setFullscreen(bool enabled);
        void toggleFullscreen();

        bool shouldClose();
        void close();
        bool isFullscreen() const { return fullscreen; }
        GLFWwindow* getHandle() const { return window; }
        // Call at the end of run loop
        void swapBuffers();
    private:
        GLFWwindow* window = nullptr;
        bool fullscreen = false;
        int windowedX = 100;
        int windowedY = 100;
        int windowedWidth = 1280;
        int windowedHeight = 720;
};
