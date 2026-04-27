#include "Input.hpp"

void Input::beginFrame() {
    for(auto& key: keys) {
        key.pressed = false;
        key.released = false;
        key.repeated = false;
    }
}

bool Input::isKeyDown(int key) const {
    return keys[key].down;
}

bool Input::isKeyPressed(int key) const {
    return keys[key].pressed; 
};

bool Input::isKeyReleased(int key) const {
    return keys[key].repeated; 
};

void Input::onKeyEvent(int key, int action) {
        KeyState& state = keys[key];
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