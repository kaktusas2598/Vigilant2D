#include "FrameBuffer.hpp"
#include "ImGuiLayer.hpp"
#include "Shader.hpp"
#include "Window.hpp"

void errorCallback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(void) {
    Window window;
    window.init(640, 480);

    // TODO: refactor callback and poll event code to glfw abstraction layer
    glfwSetErrorCallback(errorCallback);
    glfwSetKeyCallback(window.getHandle(), keyCallback);

    ImGuiLayer uiLayer;
    uiLayer.init(window.getHandle());

    bool showDemoWindow = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    FrameBuffer* renderTarget = new FrameBuffer(640, 480);

    // TESTING Rendering Setup Code
    Shader* simpleShader = new Shader("shaders/triangleTest.vert", "shaders/triangleTest.frag");

    glm::vec3 vertices[] = {
        {-0.5, -0.5, 0.0},
        {0.5, -0.5, 0.0},
        {0.0,  0.5, 0.0}
    };

    unsigned int vaoID, vboID;
    glGenBuffers(1, &vboID);
    glGenVertexArrays(1, &vaoID);

    glBindVertexArray(vaoID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    /* Loop until the user closes the window */
    while (!window.shouldClose()) {
        /* Poll for and process events */
        glfwPollEvents();

        // Start the Dear ImGui frame
        uiLayer.begin();

        renderTarget->bind();
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);

        simpleShader->bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);

        renderTarget->unbind();

        uiLayer.render();

        if (showDemoWindow)
            ImGui::ShowDemoWindow(&showDemoWindow);

        ImGui::Begin("Settings");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &showDemoWindow);      // Edit bools storing our window open/close state

        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();


        ImGui::Begin("Viewport");

        ImGui::Image(
                (ImTextureID)renderTarget->getTextureID(),
                ImGui::GetContentRegionAvail(),
                ImVec2(0, 1),
                ImVec2(1, 0)
                );

        ImGui::End();


        uiLayer.end();

        window.swapBuffers();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    return 0;
}
