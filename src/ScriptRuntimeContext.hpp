#pragma once

class Scene;
class EntityFactory;
class AnimationRegistry;
class Input;
class Camera2D;
class ParticleSystem;
class ParticlePresetRegistry;
class ParticleEmitterRegistry;
class AssetManager;
class UISystem;
class Window;
class AudioSystem;
class ScreenFlowSystem;
class CameraFollowState;
class PostProcessSettings;
class DataGridRegistry;
class DataListRegistry;
class GameClock;

struct ScriptRuntimeContext {
    Scene* scene = nullptr;
    EntityFactory* entityFactory = nullptr;
    AnimationRegistry* animationRegistry = nullptr;
    Input* input = nullptr;
    Camera2D* camera = nullptr;
    ParticleSystem* particleSystem = nullptr;
    ParticlePresetRegistry* particlePresetRegistry = nullptr;
    ParticleEmitterRegistry* particleEmitterRegistry = nullptr;
    AssetManager* assetManager = nullptr;
    UISystem* uiSystem = nullptr;
    Window* window = nullptr;
    AudioSystem* audioSystem = nullptr;
    ScreenFlowSystem* screenFlowSystem = nullptr;
    CameraFollowState* cameraFollowState = nullptr;
    PostProcessSettings* postProcessSettings = nullptr;
    DataGridRegistry* dataGridRegistry = nullptr;
    DataListRegistry* dataListRegistry = nullptr;
    GameClock* gameClock = nullptr;
};