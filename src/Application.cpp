#include "Application.hpp"

#include "ErrorHandler.hpp"
#include "Input.hpp"

void errorCallback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Application* app = (Application* )glfwGetWindowUserPointer(window);
    //Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_PRESS) {
        printf("Debug mode: %d\n", app->isDebugModeEnabled());
        app->switchDebugMode();
    }
}

static void windowSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}


void Application::init() {
    window.init(640, 480);

    glfwSetWindowUserPointer(window.getHandle(), this);
    // input.setupKeyCallback(window.getHandle());
    glfwSetErrorCallback(errorCallback);
    glfwSetKeyCallback(window.getHandle(), keyCallback);
    glfwSetWindowSizeCallback(window.getHandle(), windowSizeCallback);

    uiLayer.init(window.getHandle());

    clearColour = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    debugMode = false;

    renderer.init();
}

void Application::run() {
    while (!window.shouldClose()) {
        glfwPollEvents();

        int display_w, display_h;
        glfwGetFramebufferSize(window.getHandle(), &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clearColour.x * clearColour.w, clearColour.y * clearColour.w, clearColour.z * clearColour.w, clearColour.w);
        glClear(GL_COLOR_BUFFER_BIT);

        uiLayer.begin();

        if (debugMode)
            uiLayer.render((float*)&clearColour);

        renderer.render();

        uiLayer.end();
        window.swapBuffers();
    }
}

void Application::exit() {

}

void Application::switchDebugMode() {
    debugMode = !debugMode;
}

bool Application::isDebugModeEnabled() {
    return debugMode;
}
