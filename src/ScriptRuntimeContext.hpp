#pragma once

class Scene;
class AnimationRegistry;
class Input;
class Camera2D;
class ParticleEmitterRegistry;

struct ScriptRuntimeContext {
    Scene* scene = nullptr;
    AnimationRegistry* animationRegistry = nullptr;
    Input* input = nullptr;
    Camera2D* camera = nullptr;
    ParticleEmitterRegistry* particleEmitterRegistry = nullptr;
};