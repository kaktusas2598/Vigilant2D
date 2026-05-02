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

#include "TileMap.hpp"

#include "Sprite.hpp"
#include "AnimatedSprite.hpp"
#include "AnimationClip.hpp"

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

        // TEMPORARY textures for testing
        Texture* boxTexture = nullptr;
        // Tiled map test with batched rendering
        TileMap testMap;
        // Sprite and animated sprite test
        Texture* playerTexture = nullptr;
        Sprite testPlayerSprite;
        AnimatedSprite testPlayerAnimation;
        AnimationClip testIdleClip;
        Transform2D testPlayerTransform;
};
