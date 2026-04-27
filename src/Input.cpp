#include "Input.hpp"

void Input::beginFrame() {
    for (auto& key: keys) {
        key.pressed = false;
        key.released = false;
        key.repeated = false;
    }
    for (auto& button: mouseButtons) {
        button.pressed = false;
        button.released = false;
        button.repeated = false;
    }
}

bool Input::isKeyDown(int key) const {
    return keys[key].down;
}

bool Input::isKeyPressed(int key) const {
    return keys[key].pressed; 
};

bool Input::isKeyReleased(int key) const {
    return keys[key].released; 
};

void Input::onKeyEvent(int key, int action) {
    if (!isValidKey(key)) return;

    applyKeyEvent(keys[key], action);
}

void Input::onMouseButtonEvent(int key, int action) {
    if (!isValidMouseButton(key)) return;

    applyKeyEvent(mouseButtons[key], action);
}

void Input::OnMouseMoveEvent(double x, double y) {
    mouseX = x;
    mouseY = y;
}

void Input::applyKeyEvent(KeyState& state, int action) {
    switch (action) {
    case GLFW_PRESS:
        if (!state.down)
            state.pressed = true;
        state.down = true;
        break;

    case GLFW_RELEASE:
        state.down = false;
        state.released = true;
        break;

    case GLFW_REPEAT:
        state.down = true;
        state.repeated = true;
        break;

    default:
        break;
    }
}

bool Input::isValidKey(int key) const {
    return key >= 0 && key < KeyCount;
}

bool Input::isValidMouseButton(int button) const {
    return button >= 0 && button < MouseButtonCount;
}
