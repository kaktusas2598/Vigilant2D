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
#include "Scene.hpp"
#include "SelectionManager.hpp"
#include "AssetManager.hpp"
#include "ParticleSystem.hpp"
#include "ScriptSystem.hpp"

#include "EntityFactory.hpp"
#include "AnimationRegistry.hpp"

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

        // TODO: probably best to do through scripting?
        void movePlayer();
        void spawnSlime(const glm::vec2& position);
        void spawnEmptyEntity(const glm::vec2& position);

        Window window;
        ImGuiLayer uiLayer;
        Renderer renderer;

        ImVec4 clearColour;
        bool debugMode = false;
        bool showPhysicsDebug = false;

        Input input;
        Time time;

        // TODO: research whats best for Camera - probably not storing single insance in Application class!
        Camera2D camera;

        Scene scene;
        AssetManager assetManager;
        // TEMPORARY resources for testing - owned by AssetManager 
        Texture* boxTexture = nullptr;

        SelectionManager selectionManager;
        ScriptSystem scriptSystem;
        ParticleSystem particleSystem;
        ParticleEmitter* bloodEmitter = nullptr;
        ParticleEmitter* textureEmitter = nullptr;

        AnimationRegistry animationRegistry;
        EntityFactory* entityFactory = nullptr;
};
