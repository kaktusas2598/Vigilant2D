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
class EntityFactory;
class Input;
class Camera2D;
class CameraFollowState;
class AssetManager;
class AnimationRegistry;
class ParticleSystem;
class ParticleEmitterRegistry;
class ParticlePresetRegistry;
class UISystem;
class AudioSystem;
class Window;
class ScreenFlowSystem;
class PostProcessSettings;
class DataGridRegistry;
class DataListRegistry;
class GameClock;

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
        bool callEntityOnDestroy(const Entity& entity);
        void detachFromEntity(const Entity& entity);
        bool callTableFunction(const ScriptInstance& instance, const char* functionName);
        bool callTableFunction(const ScriptInstance& instance, const char* functionName, float dt);
        ScriptInstance loadScriptTable(const std::string& fileName);
        bool runScriptInstanceFunction(const ScriptInstance& instance, const char* functionName);
        bool runScriptFileFunction(const std::string& fileName, const char* functionName);

        // Coroutine methods
        bool startEntityCoroutine(const std::string& ownerEntityId, int functionIndex, int selfIndex);
        bool startGlobalCoroutine(int functionIndex);
        void updateTasks(float dt);
        void cancelTasksForEntity(const std::string& entityId);
        void cancelAllTasks();

        lua_State* getState() const { return luaState; }
        bool callNamedFunction(const ScriptInstance& instance, const char* functionName);
        bool runGlobalScriptFunction(const std::string& fileName, const char* functionName);

        void setRuntimeContext(ScriptRuntimeContext newContext);
        Scene* getRuntimeScene() const { return runtimeContext.scene; }
        Input* getRuntimeInput() const { return runtimeContext.input; }
        Camera2D* getRuntimeCamera() const { return runtimeContext.camera; }
        EntityFactory* getRuntimeEntityFactory() const { return runtimeContext.entityFactory; }
        CameraFollowState* getRuntimeCameraFollowState() const { return runtimeContext.cameraFollowState; }
        AssetManager* getAssetManager() const { return runtimeContext.assetManager; }
        AnimationRegistry* getAnimationRegistry() const { return runtimeContext.animationRegistry; }
        ParticleEmitterRegistry* getParticleEmitterRegistry() const { return runtimeContext.particleEmitterRegistry; }
        ParticlePresetRegistry* getParticlePresetRegistry() const { return runtimeContext.particlePresetRegistry; }
        ParticleSystem* getParticleSystem() const { return runtimeContext.particleSystem; }
        UISystem* getUISystem() const { return runtimeContext.uiSystem; }
        AudioSystem* getRuntimeAudioSystem() const { return runtimeContext.audioSystem; }
        ScreenFlowSystem* getRuntimeScreenFlowSystem() const { return runtimeContext.screenFlowSystem; }
        Window* getRuntimeWindow() const { return runtimeContext.window; }
        GameClock* getRuntimeGameClock() const { return runtimeContext.gameClock; }
        PostProcessSettings* getPostProcessSettings() const { return runtimeContext.postProcessSettings; }
        DataGridRegistry* getDataGridRegistry() const { return runtimeContext.dataGridRegistry; }
        DataListRegistry* getDataListRegistry() const { return runtimeContext.dataListRegistry; }
        void setRuntimeEntityFactory(EntityFactory* factory) { runtimeContext.entityFactory = factory; }    
    private:
        bool reportError(int status, const std::string& context);

        lua_State* luaState = nullptr;
        std::unordered_map<std::string, ScriptInstance> entityScripts;

        ScriptContentLoader contentLoader;
        ScriptRuntimeContext runtimeContext;
        ScriptTaskRunner taskRunner;
};