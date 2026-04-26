#pragma once

#include <GLFW/glfw3.h>
#include <map>

class Input {
    public:
        bool isKeyDown(int key);
        void setupKeyCallback(GLFWwindow* window);

    private:
        std::map<int, bool> keys;
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};