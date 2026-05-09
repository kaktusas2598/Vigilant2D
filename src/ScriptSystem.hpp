#pragma once

#include <string>

extern "C" {
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}

#include <unordered_map>
#include "Entity.hpp"
#include "EntityDefinition.hpp"
#include "AnimationDefinition.hpp"

class Scene;
class AnimationRegistry;

// Raw assets
struct TextureManifestEntry {
    std::string id;
    std::string path;
};

struct FontManifestEntry {
    std::string id;
    std::string path;
    int pixelSize = 16;
};

// Built content 
// TODO: particle emitters manifests and bootstraping?
struct AnimationManifestEntry {
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
        bool loadAnimationManifest(const std::string& fileName, std::vector<AnimationManifestEntry>& outAnimations);
        bool loadEntityDefinition(const std::string& fileName, EntityDefinition& outDefinition);
        bool loadAnimationDefinition(const std::string& fileName, AnimationDefinition& outDefinition);

        ScriptInstance loadBehavior(const std::string& fileName);
        bool callOnCreate(const ScriptInstance& instance);
        bool callOnUpdate(const ScriptInstance& instance, float dt);
        void releaseInstance(ScriptInstance& instance);
        bool attachToEntity(const Entity& entity);
        bool callEntityOnCreate(const Entity& entity);
        bool callEntityOnUpdate(const Entity& entity, float dt);
        void detachFromEntity(const Entity& entity);

        lua_State* getState() const { return luaState; }

        void setRuntimeContext(Scene& scene, AnimationRegistry& newAnimationRegsitry);
        Scene* getRuntimeScene() const { return runtimeScene; }
        AnimationRegistry* getAnimationRegistry() const { return animationRegistry; }
    private:
        bool reportError(int status, const std::string& context);

        lua_State* luaState = nullptr;
        std::unordered_map<std::string, ScriptInstance> entityScripts;

        Scene* runtimeScene = nullptr;
        AnimationRegistry* animationRegistry = nullptr;
};