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

struct AssetManifestEntry {
    std::string id;
    std::string path;
};

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

        // Load Lua script into global stated owned by the engine
        bool loadScript(const std::string& fileName);
        // TODO: should support all kinds of assets not just textures
        bool loadAssetManifest(const std::string& fileName, std::vector<AssetManifestEntry>& outTextures);
        bool loadAnimationManifest(const std::string& fileName, std::vector<AnimationManifestEntry>& outAnimations);
        bool loadEntityDefinition(const std::string& fileName, EntityDefinition& outDefinition);
        bool loadAnimationDefinition(const std::string& fileName, AnimationDefinition& outDefinition);

        // Call global Lua method
        bool callGlobal(const std::string& functionName);
        bool callGlobal(const std::string& functionName, float dt);

        ScriptInstance loadBehavior(const std::string& fileName);
        bool callOnCreate(const ScriptInstance& instance);
        bool callOnUpdate(const ScriptInstance& instance, float dt);
        void releaseInstance(ScriptInstance& instance);

        bool attachToEntity(const Entity& entity);
        bool callEntityOnCreate(const Entity& entity);
        bool callEntityOnUpdate(const Entity& entity, float dt);
        void detachFromEntity(const Entity& entity);

        lua_State* getState() const { return luaState; }
    private:
        bool reportError(int status, const std::string& context);

        lua_State* luaState = nullptr;
        std::unordered_map<std::string, ScriptInstance> entityScripts;
};