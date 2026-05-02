#include "ImGuiLayer.hpp"

#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include <stdio.h>

ImGuiLayer::ImGuiLayer() {}

ImGuiLayer::~ImGuiLayer() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiLayer::init(GLFWwindow* window) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;        // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430");
}

void ImGuiLayer::begin() {
    // These commands probably should go just before update() method so that states can setup their own ui
    // Start the Dear ImGui frame?
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiLayer::render() {
    ImGui::Begin("Debug Tools");
    for (auto& panel : panels) {
        if (!panel.enabled)
            continue;

        if (ImGui::CollapsingHeader(panel.name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
            panel.draw();
        }
    }
    ImGui::End();
}

void ImGuiLayer::end() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiLayer::addPanel(const std::string& name, std::function<void()> drawFunc) {
    panels.push_back({name, std::move(drawFunc), true});
}