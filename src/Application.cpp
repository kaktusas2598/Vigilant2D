#include "Application.hpp"

#include "ErrorHandler.hpp"
#include "Logger.hpp"
#include "Input.hpp"

void errorCallback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

// Static GLFW callbacks setup here by Application and updating Input service state
static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    app->getInput()->onKeyEvent(key, action);
}

static void mouseMoveCallback(GLFWwindow* window, double xpos, double ypos) {
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    app->getInput()->onMouseMoveEvent(xpos, ypos);
}

static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    app->getInput()->onMouseButtonEvent(button, action);
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
    glfwSetErrorCallback(errorCallback);
    glfwSetKeyCallback(window.getHandle(), keyCallback);
    glfwSetWindowSizeCallback(window.getHandle(), windowSizeCallback);
    glfwSetMouseButtonCallback(window.getHandle(), mouseButtonCallback);
    glfwSetCursorPosCallback(window.getHandle(), mouseMoveCallback);

    uiLayer.init(window.getHandle());

    clearColour = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    debugMode = false;

    renderer.init();
}

void Application::run() {
    lastTime = static_cast<float>(glfwGetTime());

    while (!window.shouldClose()) {
        float currentTime = static_cast<float>(glfwGetTime());
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        input.beginFrame(); // Reset input
        glfwPollEvents();

        update(deltaTime);
        
        render(deltaTime);
    }
}

void Application::update(float dt) {
    // TODO: already regretting not getting variadic argument support from Villain Logger
    // VA_DEBUG("Mouse X:  %s", x)

    float cameraSpeed = 500.0f * dt;

    // TEST camera
    if (input.isKeyDown(GLFW_KEY_W)) {
        camera.move({0.0f, cameraSpeed});
    } else if (input.isKeyDown(GLFW_KEY_S)) {
        camera.move({0.0f, -cameraSpeed});
    } else if (input.isKeyDown(GLFW_KEY_A)) {
        camera.move({-cameraSpeed, 0.0f});
    } else if (input.isKeyDown(GLFW_KEY_D)) {
        camera.move({cameraSpeed, 0.0f});
    }

    if ((input.isKeyPressed(GLFW_KEY_GRAVE_ACCENT))) {
        debugMode = !debugMode;
        if (debugMode) VG_INFO("Debug Mode ON");
    }

    if (input.isKeyPressed(GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(window.getHandle(), GLFW_TRUE);
    }
}

void Application::render(float dt) {
    int display_w, display_h;
    glfwGetFramebufferSize(window.getHandle(), &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clearColour.x * clearColour.w, clearColour.y * clearColour.w, clearColour.z * clearColour.w, clearColour.w);
    glClear(GL_COLOR_BUFFER_BIT);

    uiLayer.begin();

    if (debugMode)
        uiLayer.render((float *)&clearColour);

    camera.setViewportSize((float)display_w, (float)display_h);
    renderer.begin(camera);

    // TEST quad render code
    renderer.drawQuad({{0.0f, 0.0f}, {100.0f, 100.0f}, 0.0f});
    renderer.drawQuad({{200.0f, 0.0f}, {100.0f, 100.0f}, 0.0f});
    renderer.drawQuad({{400.0f, 0.0f}, {100.0f, 100.0f}, 0.0f});

    // Tile atlas test
    renderer.drawQuad({nullptr, {0.25f, 0.25f}, {1.0f, 1.0f}},{{-100.0f, -100.0f}, {100.0f, 100.0f}, 0.0f});

    renderer.end();

    uiLayer.end();
    window.swapBuffers();
}

void Application::exit() {

}

void Application::switchDebugMode() {
    debugMode = !debugMode;
}

bool Application::isDebugModeEnabled() {
    return debugMode;
}
