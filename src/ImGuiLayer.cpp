#include "ImGuiLayer.hpp"

#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include <stdio.h>

bool ImGuiLayer::showDemoWindow = false;
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

    windowPtr = window;
}

void ImGuiLayer::begin() {
    // These commands probably should go just before update() method so that states can setup their own ui
    // Start the Dear ImGui frame?
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiLayer::render(float *clearColour, float fps, float frameTimeMs) {
    // NOTE: Must be done after starting new frame and before any Imgui rendering is done!
    ImGui::Begin("Settings");
    ImGui::Checkbox("Demo Window", &showDemoWindow);
    ImGui::ColorEdit3("clear color", clearColour);
    ImGui::Text("FPS: %.1f", fps);
    ImGui::Text("Frame: %.3f ms", frameTimeMs);
    // ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();

    if (showDemoWindow)
        ImGui::ShowDemoWindow(&showDemoWindow);
}

void ImGuiLayer::end() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
