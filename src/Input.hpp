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
        void onMouseButtonEvent(int key, int action);
        void OnMouseMoveEvent(double x, double y);

        bool isKeyDown(int key) const;
        bool isKeyPressed(int key) const;
        bool isKeyReleased(int key) const;

        bool isMouseButtonDown(int key) const;
        bool isMouseButtonKeyPressed(int key) const;
        bool isMouseButtonKeyReleased(int key) const;

        double getMouseX() const { return mouseX; }
        double getMouseY() const { return mouseY; }
    private:
        static constexpr int KeyCount = GLFW_KEY_LAST + 1;
        static constexpr int MouseButtonCount = GLFW_MOUSE_BUTTON_LAST + 1;

        void applyKeyEvent(KeyState& state, int action);

        bool isValidKey(int key) const;
        bool isValidMouseButton(int button) const;

        std::array<KeyState, KeyCount> keys{};
        std::array<KeyState, MouseButtonCount> mouseButtons{};
        double mouseX = 0.0;
        double mouseY = 0.0;
};