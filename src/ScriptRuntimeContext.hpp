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
    // FIXME: Does not make much sense to store random post fx setting by itself here
    float* postFadeAmount = nullptr;
};