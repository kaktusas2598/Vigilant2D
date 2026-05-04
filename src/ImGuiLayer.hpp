#pragma once

#include <string>
#include <functional>
#include <vector>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

struct DebugPanel {
    std::string name;
    std::function<void()> draw;
    bool enabled = true;
};

class ImGuiLayer {
    public:
        ImGuiLayer();
        ~ImGuiLayer();

        // Sets up ImGui context and backends
        void init(GLFWwindow* window);
        // Shutdown ImGui
        void exit();

        // Always call these methods in the same order as such: begin() -> render() -> end()
        void begin();
        void render();
        void end();

        void addPanel(const std::string& name, std::function<void()> drawFunc);
        bool wantsMouseCapture() const;
    private:
        std::vector<DebugPanel> panels;
};
