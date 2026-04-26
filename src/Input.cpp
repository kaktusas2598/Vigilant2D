#include "Input.hpp"

bool Input::isKeyDown(int key) {

    return false;
}

void Input::setupKeyCallback(GLFWwindow *window) {
    glfwSetKeyCallback(window, keyCallback);
}

// Static callback to register keys onto map
void Input::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    // keys[key] = action;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    // if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_PRESS) {
    //     app.switchDebugMode();
    // }

}
