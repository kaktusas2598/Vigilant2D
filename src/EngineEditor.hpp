#pragma once

#include <functional>

#include "glm/glm.hpp"
#include "imgui/imgui.h"

class ImGuiLayer;
class Window;
class Time;
class Camera2D;
class Scene;
class SelectionManager;
class AssetManager;
class ParticleSystem;
class ParticleEmitterRegistry;

struct EngineEditorContext {
    Window& window;
    Time& time;
    Camera2D& camera;
    Scene& scene;
    SelectionManager& selectionManager;
    AssetManager& assetManager;
    ParticleSystem& particleSystem;
    ParticleEmitterRegistry& particleEmitterRegistry;

    ImVec4& clearColour;
    bool& showPhysicsDebug;
    bool& selectionManagerEnabled;
    bool& cameraFollowPlayer;

    // TODO: probably temporary
    std::function<void(const glm::vec2&)> spawnSlime;
    std::function<void(const glm::vec2&)> spawnEmptyEntity;
};

class EngineEditor {
public:
    explicit EngineEditor(EngineEditorContext context);

    void registerPanels(ImGuiLayer& uiLayer);

private:
    EngineEditorContext context;
};
