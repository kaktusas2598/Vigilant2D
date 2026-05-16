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

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking

    ImGui::StyleColorsDark();
    mainWindow = window;

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
    if (dockspaceEnabled) {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGuiWindowFlags hostWindowFlags =
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus |
            ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_MenuBar;
        
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

            ImGui::Begin("EditorDockspace", nullptr, hostWindowFlags);

            ImGui::PopStyleVar(3);

            ImGuiID dockspaceId = ImGui::GetID("MainDockspace");
            ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("View")) {
                    for (auto& panel : panels)
                        ImGui::MenuItem(panel.name.c_str(), nullptr, &panel.enabled);
                    ImGui::EndMenu();
                }
                if (ImGui::MenuItem("Exit")) {
                    glfwSetWindowShouldClose(mainWindow, GLFW_TRUE);
                }
                ImGui::EndMenuBar();
            }

            ImGui::End();
    }
    // Commented code is the old way of one window of debug tools
    // ImGui::Begin("Debug Tools");
    for (auto& panel : panels) {
        if (!panel.enabled)
            continue;

        ImGui::SetNextWindowSize(ImVec2(320.0f, 240.0f), ImGuiCond_FirstUseEver);
        if (panel.defaultOpen)
            ImGui::SetNextWindowCollapsed(false, ImGuiCond_FirstUseEver);

        if (ImGui::Begin(panel.name.c_str()))
            panel.draw();
        ImGui::End();

        // Set unique identifier for each panel to prevent issues appearing when widgets share the same name
        // ImGui::PushID(panel.name.c_str());
        // if (ImGui::CollapsingHeader(panel.name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
        //     panel.draw();
        // }
        // ImGui::PopID();
    }
    // ImGui::End();
}

void ImGuiLayer::end() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiLayer::addPanel(const std::string& name, std::function<void()> drawFunc) {
    panels.push_back({name, std::move(drawFunc), true});
}

bool ImGuiLayer::wantsMouseCapture() const {
    return ImGui::GetIO().WantCaptureMouse;
}