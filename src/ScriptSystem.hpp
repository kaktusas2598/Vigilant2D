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
#include "ScriptRuntimeContext.hpp"

// For content bootstrapping
#include "EntityDefinition.hpp"
#include "AnimationDefinition.hpp"
#include "ParticlePreset.hpp"

// For runtime script context
class Scene;
class Input;
class Camera2D;
class AssetManager;
class AnimationRegistry;
class ParticleEmitterRegistry;
class UISystem;

// Raw assets manifest entry for each resource
struct TextureManifestEntry {
    std::string id;
    std::string path;
};

struct FontManifestEntry {
    std::string id;
    std::string path;
    int pixelSize = 16;
};

// Built content manifest entry for each resource
struct AnimationManifestEntry {
    std::string id;
    std::string path;
};

struct ParticlePresetManifestEntry {
    std::string id;
    std::string path;
};


struct ScriptInstance {
    std::string fileName;
    int tableRef = LUA_NOREF;
};

// Represents Lua coroutine allowing great many things to happen
struct ScriptTask {
    int threadRef = LUA_NOREF;
    std::string ownerEntityId;
    bool global = false;
    bool finished = false;

    enum class WaitMode {
        None,
        Seconds,
        NextFrame
    };

    WaitMode waitMode = WaitMode::None;
    float waitRemaining = 0.0f;
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
            std::vector<TextureManifestEntry>& outTextures, std::vector<FontManifestEntry>& outFonts);
        bool loadEntityDefinition(const std::string& fileName, EntityDefinition& outDefinition);
        bool loadAnimationManifest(const std::string& fileName, std::vector<AnimationManifestEntry>& outAnimations);
        bool loadAnimationDefinition(const std::string& fileName, AnimationDefinition& outDefinition);
        bool loadParticlePresetManifest(const std::string& fileName,
                                std::vector<ParticlePresetManifestEntry>& outPresets);
        bool loadParticlePresetDefinition(const std::string& fileName, ParticlePreset& outPreset);


        // Script behavior methods
        ScriptInstance loadBehavior(const std::string& fileName);
        bool callOnCreate(const ScriptInstance& instance);
        bool callOnUpdate(const ScriptInstance& instance, float dt);
        void releaseInstance(ScriptInstance& instance);
        bool attachToEntity(const Entity& entity);
        bool callEntityOnCreate(const Entity& entity);
        bool callEntityOnUpdate(const Entity& entity, float dt);
        void detachFromEntity(const Entity& entity);

        // Coroutine methods
        bool startEntityCoroutine(const std::string& ownerEntityId, int functionIndex, int selfIndex);
        bool startGlobalCoroutine(int functionIndex);
        void updateTasks(float dt);
        void cancelTasksForEntity(const std::string& entityId);

        lua_State* getState() const { return luaState; }

        void setRuntimeContext(ScriptRuntimeContext newContext);
        Scene* getRuntimeScene() const { return runtimeContext.scene; }
        Input* getRuntimeInput() const { return runtimeContext.input; }
        Camera2D* getRuntimeCamera() const { return runtimeContext.camera; }
        AssetManager* getAssetManager() const { return runtimeContext.assetManager; }
        AnimationRegistry* getAnimationRegistry() const { return runtimeContext.animationRegistry; }
        ParticleEmitterRegistry* getParticleEmitterRegistry() const { return runtimeContext.particleEmitterRegistry; }
        UISystem* getUISystem() const { return runtimeContext.uiSystem; }
    private:
        bool reportError(int status, const std::string& context);

        // Coroutine helpers
        lua_State* getTaskThread(const ScriptTask& task) const;
        bool startTask(ScriptTask& task, int argumentCount);
        bool resumeTask(ScriptTask& task, float dt);
        bool configureTaskWait(ScriptTask& task, lua_State* thread, int resultCount);
        void releaseTask(ScriptTask& task);
        bool reportThreadError(lua_State* thread, const std::string& context);

        lua_State* luaState = nullptr;
        std::unordered_map<std::string, ScriptInstance> entityScripts;

        ScriptRuntimeContext runtimeContext;

        std::vector<ScriptTask> activeTasks;
};