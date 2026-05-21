#pragma once

#include <string>
#include <vector>
#include <memory>

#include "ImGuiLayer.hpp"
#include "core/Window.hpp"
#include "Renderer.hpp"
#include "core/Input.hpp"
#include "Camera2D.hpp"
#include "core/Time.hpp"

#include "Scene.hpp"
#include "SelectionManager.hpp"
#include "AssetManager.hpp"
#include "ScriptSystem.hpp"
#include "AudioEngine.hpp"
#include "AudioSystem.hpp"

#include "FrameBuffer.hpp"
#include "PostProcessPass.hpp"
#include "PostProcessSettings.hpp"

#include "EntityFactory.hpp"
#include "AnimationRegistry.hpp"
#include "TopDownControllerSystem.hpp"
#include "ContentLoader.hpp"
#include "CameraFollowState.hpp"

#include "UIRenderer.hpp"
#include "UISystem.hpp"
#include "TextRenderer.hpp"

#include "ParticleSystem.hpp"
#include "ParticlePresetRegistry.hpp"
#include "ParticleEmitterRegistry.hpp"

#include "EngineEditor.hpp"

#include "ScreenFlowSystem.hpp"
#include "DataGridRegistry.hpp"
#include "DataListRegistry.hpp"

class Application {
    public:
        ~Application() {}

        void init();
        void run();
        void exit() {}

        void switchDebugMode() { debugMode = !debugMode; }
        bool isDebugModeEnabled() { return debugMode; }

        Input* getInput();

    private:
        void update(float dt);
        void render(float dt);

        bool bootstrapGameplaySession();
        void clearGameplaySession();
        void performPendingSessionReset();

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
        AudioEngine audioEngine;
        std::unique_ptr<AudioSystem> audioSystem = nullptr;

        std::unique_ptr<FrameBuffer> sceneFrameBuffer = nullptr;
        PostProcessPass postProcessPass;
        PostProcessSettings postProcessSettings;

        SelectionManager selectionManager;

        ParticleSystem particleSystem;
        ParticlePresetRegistry  particlePresetRegistry;
        ParticleEmitterRegistry  particleEmitterRegistry;

        AnimationRegistry animationRegistry;
        std::unique_ptr<EntityFactory> entityFactory = nullptr;
        std::unique_ptr<TopDownControllerSystem> topDownControllerSystem = nullptr;
        std::unique_ptr<ContentLoader> contentLoader = nullptr;
        CameraFollowState cameraFollowState;
        std::unique_ptr<ScreenFlowSystem> screenFlowSystem = nullptr;

        std::unique_ptr<UIRenderer> uiRenderer = nullptr;
        TextRenderer textRenderer;
        UISystem uiSystem;

        DataGridRegistry dataGridRegistry;
        DataListRegistry dataListRegistry;
};
