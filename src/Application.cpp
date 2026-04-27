#include "Application.hpp"

#include "ErrorHandler.hpp"
#include "Logger.hpp"
#include "Input.hpp"

void errorCallback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

// Static GLFW callbacks setup here by Application and updating various services like Input
static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    app->getInput()->onKeyEvent(key, action);
}

static void windowSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

Input* Application::getInput() {
    return &input;
}

void Application::init() {
    window.init(640, 480);

    glfwSetWindowUserPointer(window.getHandle(), this);
    // input.setupKeyCallback(window.getHandle());
    glfwSetErrorCallback(errorCallback);
    glfwSetKeyCallback(window.getHandle(), keyCallback);
    glfwSetWindowSizeCallback(window.getHandle(), windowSizeCallback);
    // glfwSetMouseButtonCallback(window.getHandle(), mouseButtonCallback);
    // glfwSetCursorPosCallback(window.getHandle(), mouseMoveCallback);

    uiLayer.init(window.getHandle());

    clearColour = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    debugMode = false;

    renderer.init();
}

void Application::run() {
    while (!window.shouldClose()) {
        input.beginFrame(); // Reset input
        glfwPollEvents();

        if (input.isKeyPressed(GLFW_KEY_A)) {

            VG_DEBUG("A pressed");
            // printf("A pressed.\n");
            // fflush(stdout);
        }

        if ((input.isKeyPressed(GLFW_KEY_GRAVE_ACCENT))) {
            debugMode = !debugMode;
            if (debugMode) VG_INFO("Debug Mode ON");
        }

        if (input.isKeyPressed(GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window.getHandle(), GLFW_TRUE);
        }

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
