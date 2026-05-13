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
#include "ScriptSystem.hpp"

#include "FrameBuffer.hpp"
#include "PostProcessPass.hpp"

#include "EntityFactory.hpp"
#include "AnimationRegistry.hpp"
#include "TopDownControllerSystem.hpp"
#include "ContentLoader.hpp"

#include "UIRenderer.hpp"
#include "UISystem.hpp"
#include "TextRenderer.hpp"

#include "ParticleSystem.hpp"
#include "ParticlePresetRegistry.hpp"
#include "ParticleEmitterRegistry.hpp"

#include "EngineEditor.hpp"

#include "game/FarmWorldState.hpp"

class Application {
    public:
        ~Application() {}

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
        std::unique_ptr<EngineEditor> engineEditor = nullptr;
        Renderer renderer;
        int initialWindowWidth = 1024;
        int initialWindowHeight = 768;

        ImVec4 clearColour;
        bool debugMode = false;
        bool showPhysicsDebug = false;
        bool selectionManagerEnabled = false;

        Input input;
        Time time;
        Camera2D camera;
        Scene scene;
        AssetManager assetManager;
        ScriptSystem scriptSystem;

        std::unique_ptr<FrameBuffer> sceneFrameBuffer = nullptr;
        PostProcessPass postProcessPass;
        float postVignetteStrength = 0.18f;
        float postContrast = 1.0f;
        glm::vec3 postTint{1.0f, 1.0f, 1.0f};

        SelectionManager selectionManager;

        ParticleSystem particleSystem;
        ParticlePresetRegistry  particlePresetRegistry;
        ParticleEmitterRegistry  particleEmitterRegistry;

        AnimationRegistry animationRegistry;
        std::unique_ptr<EntityFactory> entityFactory = nullptr;
        std::unique_ptr<TopDownControllerSystem> topDownControllerSystem = nullptr;
        std::unique_ptr<ContentLoader> contentLoader = nullptr;
        bool cameraFollowPlayer = true;

        std::unique_ptr<UIRenderer> uiRenderer = nullptr;
        TextRenderer textRenderer;
        UISystem uiSystem;

        // Game specific members
        FarmWorldState farmWorldState;
};
