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

#include "Scene.hpp"
#include "SelectionManager.hpp"
#include "AssetManager.hpp"
#include "ParticleSystem.hpp"
#include "ScriptSystem.hpp"

#include "EntityFactory.hpp"
#include "AnimationRegistry.hpp"
#include "TopDownControllerSystem.hpp"
#include "ContentLoader.hpp"

#include "UIRenderer.hpp"
#include "UISlotStrip.hpp"

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

        // TODO: probably temporary, create something better in debug tools
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
        ScriptSystem scriptSystem;

        SelectionManager selectionManager;
        ParticleSystem particleSystem;
        // Temporary emitters
        ParticleEmitter* bloodEmitter = nullptr;
        ParticleEmitter* textureEmitter = nullptr;

        AnimationRegistry animationRegistry;
        std::unique_ptr<EntityFactory> entityFactory = nullptr;
        std::unique_ptr<TopDownControllerSystem> topDownControllerSystem = nullptr;
        std::unique_ptr<ContentLoader> contentLoader = nullptr;
        bool cameraFollowPlayer = true;

        std::unique_ptr<UIRenderer> uiRenderer = nullptr;
        // For UI Widget test
        UISlotStrip slotStrip;
        UIStyle uiStyle;
};
