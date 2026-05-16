#pragma once

#include <string>

extern "C" {
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}

#include <vector>
#include <unordered_map>
#include "Entity.hpp"
#include "ScriptContentLoader.hpp"
#include "ScriptTaskRunner.hpp"
#include "ScriptRuntimeContext.hpp"

// For content bootstrapping
#include "EntityDefinition.hpp"
#include "AnimationDefinition.hpp"
#include "ParticlePreset.hpp"
#include "ScreenDefinition.hpp"

// For runtime script context
class Scene;
class Input;
class Camera2D;
class CameraFollowState;
class AssetManager;
class AnimationRegistry;
class ParticleEmitterRegistry;
class UISystem;
class AudioSystem;
class ScreenFlowSystem;

struct ScriptInstance {
    std::string fileName;
    int tableRef = LUA_NOREF;
};

class ScriptSystem {
    public:
        ScriptSystem() = default;
        ~ScriptSystem() {
            shutdown();
        };

        bool init();
        void shutdown();

        // Content Bootstrapping Loaders
        bool loadAssetManifest(const std::string& fileName,
            std::vector<TextureManifestEntry>& outTextures,
            std::vector<FontManifestEntry>& outFonts,
            std::vector<SoundManifestEntry>& outSounds);
        bool loadEntityDefinition(const std::string& fileName, EntityDefinition& outDefinition);
        bool loadAnimationManifest(const std::string& fileName, std::vector<AnimationManifestEntry>& outAnimations);
        bool loadAnimationDefinition(const std::string& fileName, AnimationDefinition& outDefinition);
        bool loadParticlePresetManifest(const std::string& fileName,
                                std::vector<ParticlePresetManifestEntry>& outPresets);
        bool loadParticlePresetDefinition(const std::string& fileName, ParticlePreset& outPreset);
        bool loadScreenDefinitions(const std::string& fileName, std::vector<ScreenDefinition>& outScreens);

        // Script behavior methods
        ScriptInstance loadBehavior(const std::string& fileName);
        void releaseInstance(ScriptInstance& instance);
        bool attachToEntity(const Entity& entity);
        bool callEntityOnCreate(const Entity& entity);
        bool callEntityOnUpdate(const Entity& entity, float dt);
        void detachFromEntity(const Entity& entity);
        bool callTableFunction(const ScriptInstance& instance, const char* functionName);
        bool callTableFunction(const ScriptInstance& instance, const char* functionName, float dt);

        // Coroutine methods
        bool startEntityCoroutine(const std::string& ownerEntityId, int functionIndex, int selfIndex);
        bool startGlobalCoroutine(int functionIndex);
        void updateTasks(float dt);
        void cancelTasksForEntity(const std::string& entityId);

        lua_State* getState() const { return luaState; }
        bool callNamedFunction(const ScriptInstance& instance, const char* functionName);
        bool runGlobalScriptFunction(const std::string& fileName, const char* functionName);

        void setRuntimeContext(ScriptRuntimeContext newContext);
        Scene* getRuntimeScene() const { return runtimeContext.scene; }
        Input* getRuntimeInput() const { return runtimeContext.input; }
        Camera2D* getRuntimeCamera() const { return runtimeContext.camera; }
        CameraFollowState* getRuntimeCameraFollowState() const { return runtimeContext.cameraFollowState; }
        AssetManager* getAssetManager() const { return runtimeContext.assetManager; }
        AnimationRegistry* getAnimationRegistry() const { return runtimeContext.animationRegistry; }
        ParticleEmitterRegistry* getParticleEmitterRegistry() const { return runtimeContext.particleEmitterRegistry; }
        UISystem* getUISystem() const { return runtimeContext.uiSystem; }
        AudioSystem* getRuntimeAudioSystem() const { return runtimeContext.audioSystem; }
        ScreenFlowSystem* getRuntimeScreenFlowSystem() const { return runtimeContext.screenFlowSystem; }
        float *getPostFadeAmount() const { return runtimeContext.postFadeAmount; }
    private:
        bool reportError(int status, const std::string& context);

        lua_State* luaState = nullptr;
        std::unordered_map<std::string, ScriptInstance> entityScripts;

        ScriptContentLoader contentLoader;
        ScriptRuntimeContext runtimeContext;
        ScriptTaskRunner taskRunner;
};