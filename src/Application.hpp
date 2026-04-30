#pragma once

#include "ImGuiLayer.hpp"
#include "Window.hpp"
#include "Renderer.hpp"
#include "Input.hpp"
#include "Logger.hpp"
#include "Camera2D.hpp"

#include "Texture.hpp"

class Application {
    public:
        void init();
        void run();
        void exit();

        void switchDebugMode();
        bool isDebugModeEnabled();

        Input* getInput();
    private:
        void update(float dt);
        void render(float dt);

        Window window;
        ImGuiLayer uiLayer;
        Renderer renderer;

        ImVec4 clearColour;
        bool debugMode;

        Input input;
        float deltaTime = 0.0f;
        float lastTime = 0.0f;

        // TODO: research whats best for Camera - probably not storing single insance in Application class!
        Camera2D camera;


        // TEMPORARY textures
        Texture* boxTexture = nullptr;
        Texture* atlasTexture = nullptr;
};
