#pragma once

#include <vector>
#include <memory>

#include "ImGuiLayer.hpp"
#include "Window.hpp"
#include "Renderer.hpp"
#include "Input.hpp"
#include "Logger.hpp"
#include "Camera2D.hpp"
#include "Time.hpp"

#include "Texture.hpp"
#include "AnimationClip.hpp"
#include "Scene.hpp"

class Application {
    public:
        ~Application() {
            delete boxTexture;
        }
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
        Time time;

        // TODO: research whats best for Camera - probably not storing single insance in Application class!
        Camera2D camera;

        Scene scene;
        // TEMPORARY resources for testing - TODO: asset/resource manager!!
        Texture* boxTexture = nullptr;
        Texture* playerTexture = nullptr;
        AnimationClip testIdleClip;
        // TESTING members
        glm::ivec2 tileHover{-1, -1};
        glm::ivec2 tileSelected{-1, -1};
};
