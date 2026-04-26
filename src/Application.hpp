#pragma once

#include "ImGuiLayer.hpp"
#include "Window.hpp"
#include "Renderer.hpp"
#include "Input.hpp"

class Application {
    public:
        void init();
        void run();
        void exit();

        void switchDebugMode();
        bool isDebugModeEnabled();
    private:

        Window window;
        ImGuiLayer uiLayer;
        Renderer renderer;

        ImVec4 clearColour;
        bool debugMode;

        Input input;
};
