#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

class ImGuiLayer {
    public:
        ImGuiLayer();
        ~ImGuiLayer();

        // Sets up ImGui context and backends
        void init(GLFWwindow* window);
        // Shutdown ImGui
        void exit();

        // Call before render() to start a new frame
        void begin();
        // Call after render() to actually render and end frame
        void end();

        void render();

    private:
        void setupDockspace();

        static bool showDemoWindow; ///< Toggle IMGui Demo Window for Docs
};
