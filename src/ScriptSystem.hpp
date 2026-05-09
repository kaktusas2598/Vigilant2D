#pragma once

#include <string>

extern "C" {
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}

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
class AnimationRegistry;
class ParticleEmitterRegistry;

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

class ScriptSystem {
    public:
        ScriptSystem() = default;
        ~ScriptSystem() {
            shutdown();
        };

        bool init();
        void shutdown();

        bool loadAssetManifest(const std::string& fileName,
            std::vector<TextureManifestEntry>& outTextures, std::vector<FontManifestEntry>& outFonts);
        bool loadEntityDefinition(const std::string& fileName, EntityDefinition& outDefinition);
        bool loadAnimationManifest(const std::string& fileName, std::vector<AnimationManifestEntry>& outAnimations);
        bool loadAnimationDefinition(const std::string& fileName, AnimationDefinition& outDefinition);
        bool loadParticlePresetManifest(const std::string& fileName,
                                std::vector<ParticlePresetManifestEntry>& outPresets);
        bool loadParticlePresetDefinition(const std::string& fileName, ParticlePreset& outPreset);


        ScriptInstance loadBehavior(const std::string& fileName);
        bool callOnCreate(const ScriptInstance& instance);
        bool callOnUpdate(const ScriptInstance& instance, float dt);
        void releaseInstance(ScriptInstance& instance);
        bool attachToEntity(const Entity& entity);
        bool callEntityOnCreate(const Entity& entity);
        bool callEntityOnUpdate(const Entity& entity, float dt);
        void detachFromEntity(const Entity& entity);

        lua_State* getState() const { return luaState; }

        void setRuntimeContext(ScriptRuntimeContext newContext);
        Scene* getRuntimeScene() const { return runtimeContext.scene; }
        Input* getRuntimeInput() const { return runtimeContext.input; }
        Camera2D* getRuntimeCamera() const { return runtimeContext.camera; }
        AnimationRegistry* getAnimationRegistry() const { return runtimeContext.animationRegistry; }
        ParticleEmitterRegistry* getParticleEmitterRegistry() const { return runtimeContext.particleEmitterRegistry; }
    private:
        bool reportError(int status, const std::string& context);

        lua_State* luaState = nullptr;
        std::unordered_map<std::string, ScriptInstance> entityScripts;

        ScriptRuntimeContext runtimeContext;
};