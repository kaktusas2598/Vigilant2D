#include "Window.hpp"

#include "Logger.hpp"

void Window::init(int width, int height) {

    /* Initialize the library */
    if (!glfwInit()) {
        VG_ERROR("Could not initialise GLFW.");
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Vigilant2D", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        VG_ERROR("Failed creating GLFW window.");
        return;
    }

    glfwMakeContextCurrent(window);
    glfwFocusWindow(window);
    glfwSwapInterval(1); // Enable vsync

    if (glewInit() != GLEW_OK) {
        VG_ERROR("Could not initialise Glew.");
    }

    windowedWidth = width;
    windowedHeight = height;
}

Window::~Window() {
    if (window != nullptr) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}

bool Window::shouldClose() {
    return glfwWindowShouldClose(window);
}

void Window::close() {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void Window::swapBuffers() {
    /* Swap front and back buffers */
    glfwSwapBuffers(window);
}

void Window::setFullscreen(bool enabled) {
    if (window == nullptr || fullscreen == enabled)
        return;

    if (enabled) {
        glfwGetWindowPos(window, &windowedX, &windowedY);
        glfwGetWindowSize(window, &windowedWidth, &windowedHeight);

        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        if (monitor == nullptr)
            return;

        const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
        if (videoMode == nullptr)
            return;

        glfwSetWindowMonitor(
            window,
            monitor,
            0,
            0,
            videoMode->width,
            videoMode->height,
            videoMode->refreshRate
        );
    } else {
        glfwSetWindowMonitor(
            window,
            nullptr,
            windowedX,
            windowedY,
            windowedWidth,
            windowedHeight,
            0
        );
    }

    fullscreen = enabled;
}

void Window::toggleFullscreen() {
    setFullscreen(!fullscreen);
}
