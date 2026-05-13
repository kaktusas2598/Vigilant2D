#pragma once

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

class EntityFactory;

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

    float& postVignetteStrength;
    float& postContrast;
    float& postBrightness;
    float& postSaturation;
    glm::vec3& postTint;
    glm::vec4& postFadeColor;
    float& postFadeAmount;

    EntityFactory& entityFactory;
};

class EngineEditor {
public:
    explicit EngineEditor(EngineEditorContext context);

    void registerPanels(ImGuiLayer& uiLayer);

private:
    // TODO: probably temporary, create something better in debug tools
    void spawnSlime(const glm::vec2& position);
    void spawnEmptyEntity(const glm::vec2& position);

    EngineEditorContext context;
};
