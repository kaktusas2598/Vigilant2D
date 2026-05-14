#pragma once

#include <functional>
#include <string>
#include <vector>

extern "C" {
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}

#include "AnimationDefinition.hpp"
#include "EntityDefinition.hpp"
#include "ParticlePreset.hpp"

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

class ScriptContentLoader {
public:
    void init(lua_State* newLuaState) { luaState = newLuaState; }

    bool loadAssetManifest(const std::string& fileName,
        std::vector<TextureManifestEntry>& outTextures,
        std::vector<FontManifestEntry>& outFonts,
        const std::function<bool(int, const std::string&)>& reportError);

    bool loadEntityDefinition(const std::string& fileName,
        EntityDefinition& outDefinition,
        const std::function<bool(int, const std::string&)>& reportError);

    bool loadAnimationManifest(const std::string& fileName,
        std::vector<AnimationManifestEntry>& outAnimations,
        const std::function<bool(int, const std::string&)>& reportError);

    bool loadAnimationDefinition(const std::string& fileName,
        AnimationDefinition& outDefinition,
        const std::function<bool(int, const std::string&)>& reportError);

    bool loadParticlePresetManifest(const std::string& fileName,
        std::vector<ParticlePresetManifestEntry>& outPresets,
        const std::function<bool(int, const std::string&)>& reportError);

    bool loadParticlePresetDefinition(const std::string& fileName,
        ParticlePreset& outPreset,
        const std::function<bool(int, const std::string&)>& reportError);

private:
    lua_State* luaState = nullptr;
};
 