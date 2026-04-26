#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdio>

class Window {
    public:
        Window() {}
        ~Window();

        void init(int width, int height);

        bool shouldClose();
        void close();
        GLFWwindow* getHandle() const { return window; }
        // Call at the end of run loop
        void swapBuffers();
    private:
        GLFWwindow* window;
};

