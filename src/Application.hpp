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
#include "SelectionManager.hpp"
#include "AssetManager.hpp"
#include "ParticleSystem.hpp"

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
        Time time;

        // TODO: research whats best for Camera - probably not storing single insance in Application class!
        Camera2D camera;

        Scene scene;
        AssetManager assetManager;
        // TEMPORARY resources for testing - owned by AssetManager 
        Texture* boxTexture = nullptr;
        Texture* playerTexture = nullptr;
        Texture* slimeTexture = nullptr;

        AnimationClip testIdleClip;
        AnimationClip testWalkDownClip;
        AnimationClip testWalkUpClip;
        AnimationClip testWalkRightClip;
        AnimationClip testSlimeClip;

        SelectionManager selectionManager;
        ParticleSystem particleSystem;
        ParticleEmitter* bloodEmitter = nullptr;
        ParticleEmitter* textureEmitter = nullptr;
};
