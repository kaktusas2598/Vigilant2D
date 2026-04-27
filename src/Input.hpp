#pragma once

#include <GLFW/glfw3.h>
#include <array>

struct KeyState {
    bool down = false;
    bool pressed = false;
    bool released = false;
    bool repeated = false;
};

class Input {
    public:
        void beginFrame();
        
        void onKeyEvent(int key, int action);

        bool isKeyDown(int key) const;
        bool isKeyPressed(int key) const;
        bool isKeyReleased(int key) const;
    private:
        std::array<KeyState, int(GLFW_KEY_LAST + 1)> keys{};
        std::array<KeyState, int(GLFW_MOUSE_BUTTON_LAST + 1)> mouseButtons{};
        double mouseX = 0.0;
        double mouseY = 0.0;

        // static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};