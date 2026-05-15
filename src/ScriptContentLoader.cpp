#include "ScriptContentLoader.hpp"

#include "Logger.hpp"

// --------- STATIC HELPERS ---------
static bool readVec2Field(lua_State* L, int tableIndex, const char* fieldName, glm::vec2& outVec) {
    lua_getfield(L, tableIndex, fieldName);
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        return false;
    }

    const int vecIndex = lua_gettop(L);
    lua_geti(L, vecIndex, 1);
    lua_geti(L, vecIndex, 2);
    if (!lua_isnumber(L, -2) || !lua_isnumber(L, -1)) {
        lua_pop(L, 3);
        return false;
    }

    outVec.x = static_cast<float>(lua_tonumber(L, -2));
    outVec.y = static_cast<float>(lua_tonumber(L, -1));
    lua_pop(L, 3);
    return true;
}

static bool readVec4Field(lua_State* L, int tableIndex, const char* fieldName, glm::vec4& outVec) {
    tableIndex = lua_absindex(L, tableIndex);
    lua_getfield(L, tableIndex, fieldName);
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        return false;
    }

    const int vecIndex = lua_gettop(L);
    lua_geti(L, vecIndex, 1);
    lua_geti(L, vecIndex, 2);
    lua_geti(L, vecIndex, 3);
    lua_geti(L, vecIndex, 4);
    if (!lua_isnumber(L, -4) || !lua_isnumber(L, -3) || !lua_isnumber(L, -2) || !lua_isnumber(L, -1)) {
        lua_pop(L, 5);
        return false;
    }

    outVec.r = static_cast<float>(lua_tonumber(L, -4));
    outVec.g = static_cast<float>(lua_tonumber(L, -3));
    outVec.b = static_cast<float>(lua_tonumber(L, -2));
    outVec.a = static_cast<float>(lua_tonumber(L, -1));
    lua_pop(L, 5); // table + 4 numbers
    return true;
}

static bool readIntArrayField(lua_State* L, int tableIndex, const char* fieldName, std::vector<int>& outValues) {
    tableIndex = lua_absindex(L, tableIndex);

    lua_getfield(L, tableIndex, fieldName);
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        return false;
    }

    const int arrayIndex = lua_gettop(L);
    const int length = static_cast<int>(lua_rawlen(L, arrayIndex));
    outValues.clear();
    outValues.reserve(length);

    for (int i = 1; i <= length; ++i) {
        lua_geti(L, arrayIndex, i);
        if (!lua_isnumber(L, -1)) {
            lua_pop(L, 2); // pop bad value + array
            return false;
        }

        outValues.push_back(static_cast<int>(lua_tointeger(L, -1)));
        lua_pop(L, 1);
    }
    
    lua_pop(L, 1); // pop array
    return true;
}

static bool readStringField(lua_State* L, int tableIndex, const char* fieldName, std::string& outValue) {
    tableIndex = lua_absindex(L, tableIndex);

    lua_getfield(L, tableIndex, fieldName);
    if (!lua_isstring(L, -1)) {
        lua_pop(L, 1);
        return false;
    }

    outValue = lua_tostring(L, -1);
    lua_pop(L, 1);
    return true;
}

static bool readIntField(lua_State* L, int tableIndex, const char* fieldName, int& outValue) {
    tableIndex = lua_absindex(L, tableIndex);

    lua_getfield(L, tableIndex, fieldName);
    if (!lua_isnumber(L, -1)) {
        lua_pop(L, 1);
        return false;
    }

    outValue = static_cast<int>(lua_tointeger(L, -1));
    lua_pop(L, 1);
    return true;
}

static bool readFloatField(lua_State* L, int tableIndex, const char* fieldName, float& outValue) {
    tableIndex = lua_absindex(L, tableIndex);

    lua_getfield(L, tableIndex, fieldName);
    if (!lua_isnumber(L, -1)) {
        lua_pop(L, 1);
        return false;
    }

    outValue = static_cast<float>(lua_tonumber(L, -1));
    lua_pop(L, 1);
    return true;
}

static bool loadLuaFileResultTable(lua_State* L,
                                   const std::string& fileName,
                                   std::function<bool(int, const std::string&)> reportError) {
    const int loadStatus = luaL_loadfile(L, fileName.c_str());
    if (!reportError(loadStatus, "luaL_loadfile(" + fileName + ")"))
        return false;

    const int callStatus = lua_pcall(L, 0, 1, 0);
    if (!reportError(callStatus, "execute(" + fileName + ")"))
        return false;

    if (!lua_istable(L, -1)) {
        VG_ERROR("[Lua] File '" + fileName + "' must return a table.");
        lua_pop(L, 1);
        return false;
    }

    return true;
}

// Gets lua table from manifest definition file and length of it
static bool readManifestEntriesTable(lua_State* L, int rootIndex, const char* fieldName, int& outTableIndex, int& outCount) {
    rootIndex = lua_absindex(L, rootIndex);

    lua_getfield(L, rootIndex, fieldName);
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        return false;
    }

    outTableIndex = lua_gettop(L);
    outCount = static_cast<int>(lua_rawlen(L, outTableIndex));
    return true;
}

static bool readCustomValue(lua_State* L, int index, CustomValue& outValue) {
    index = lua_absindex(L, index);

    if (lua_isboolean(L, index)) {
        outValue = (lua_toboolean(L, index) != 0);
        return true;
    }

    if (lua_isinteger(L, index)) {
        outValue = static_cast<int>(lua_tointeger(L, index));
        return true;
    }

    if (lua_isnumber(L, index)) {
        outValue = static_cast<float>(lua_tonumber(L, index));
        return true;
    }

    if (lua_isstring(L, index)) {
        outValue = std::string(lua_tostring(L, index));
        return true;
    }

    return false;
}

static bool readPropertyBagField(lua_State* L, int tableIndex, const char* fieldName, PropertyBag& outBag) {
    tableIndex = lua_absindex(L, tableIndex);

    lua_getfield(L, tableIndex, fieldName);
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        return false;
    }

    const int dataIndex = lua_gettop(L);

    lua_pushnil(L);
    while (lua_next(L, dataIndex) != 0) {
        if (!lua_isstring(L, -2)) {
            lua_pop(L, 2);
            lua_pop(L, 1);
            return false;
        }

        const std::string key = lua_tostring(L, -2);
        CustomValue value;
        if (!readCustomValue(L, -1, value)) {
            lua_pop(L, 2);
            lua_pop(L, 1);
            return false;
        }

        outBag.set(key, std::move(value));
        lua_pop(L, 1); // pop value, keep key for next lua_next
    }

    lua_pop(L, 1); // pop data table
    return true;
}
// ----------------------------------

bool ScriptContentLoader::loadAssetManifest(const std::string& fileName,
     std::vector<TextureManifestEntry>& outTextures,
     std::vector<FontManifestEntry>& outFonts,
     std::vector<SoundManifestEntry>& outSounds,
     const std::function<bool(int, const std::string&)>& reportError) {
    outTextures.clear();
    outFonts.clear();
    outSounds.clear();

    if (luaState == nullptr)
        return false;

    if (!loadLuaFileResultTable(luaState, fileName, reportError))
        return false;

    const int rootIndex = lua_gettop(luaState);

    int texturesIndex = 0;
    int texturesCount = 0;
    if (readManifestEntriesTable(luaState, rootIndex, "textures", texturesIndex, texturesCount)) {
        for (int i = 1; i <= texturesCount; ++i) {
            lua_geti(luaState, texturesIndex, i);
            if (!lua_istable(luaState, -1)) {
                lua_pop(luaState, 3); // bad entry + textures + root
                VG_ERROR("[Lua] Asset manifest '" + fileName + "' contains a non-table texture entry.");
                return false;
            }

            const int entryIndex = lua_gettop(luaState);

            TextureManifestEntry entry;
            if (!readStringField(luaState, entryIndex, "id", entry.id) ||
                !readStringField(luaState, entryIndex, "path", entry.path)) {
                lua_pop(luaState, 3); // entry + textures + root
                VG_ERROR("[Lua] Asset manifest '" + fileName + "' has a texture entry missing 'id' or 'path'.");
                return false;
            }

            outTextures.push_back(std::move(entry));
            lua_pop(luaState, 1); // entry
        }
        lua_pop(luaState, 1); // textures table
    }

    int fontsIndex = 0;
    int fontsCount = 0;
    if (readManifestEntriesTable(luaState, rootIndex, "fonts", fontsIndex, fontsCount)) {
        for (int i = 1; i <= fontsCount; ++i) {
            lua_geti(luaState, fontsIndex, i);
            if (!lua_istable(luaState, -1)) {
                lua_pop(luaState, 3); // bad entry + fonts + root
                VG_ERROR("[Lua] Asset manifest '" + fileName + "' contains a non-table font entry.");
                return false;
            }

            const int entryIndex = lua_gettop(luaState);

            FontManifestEntry entry;
            if (!readStringField(luaState, entryIndex, "id", entry.id) ||
                !readStringField(luaState, entryIndex, "path", entry.path) ||
                !readIntField(luaState, entryIndex, "pixel_size", entry.pixelSize)) {
                lua_pop(luaState, 3); // entry + fonts + root
                VG_ERROR("[Lua] Asset manifest '" + fileName + "' has a font entry missing 'id', 'path' or 'pixel_size.");
                return false;
            }

            outFonts.push_back(std::move(entry));
            lua_pop(luaState, 1); // entry
        }
        lua_pop(luaState, 1); // fonts table
    }

    int soundsIndex = 0;
    int soundsCount = 0;
    if (readManifestEntriesTable(luaState, rootIndex, "sounds", soundsIndex, soundsCount)) {
        for (int i = 1; i <= soundsCount; ++i) {
            lua_geti(luaState, soundsIndex, i);
            if (!lua_istable(luaState, -1)) {
                lua_pop(luaState, 3); // bad entry + sounds + root
                VG_ERROR("[Lua] Asset manifest '" + fileName + "' contains a non-table sound entry.");
                return false;
            }

            const int entryIndex = lua_gettop(luaState);

            SoundManifestEntry entry;
            if (!readStringField(luaState, entryIndex, "id", entry.id) ||
                !readStringField(luaState, entryIndex, "path", entry.path)) {
                lua_pop(luaState, 3); // entry + sounds + root
                VG_ERROR("[Lua] Asset manifest '" + fileName + "' has a sound entry missing 'id' or 'path'.");
                return false;
            }

            outSounds.push_back(std::move(entry));
            lua_pop(luaState, 1); // entry
        }
        lua_pop(luaState, 1); // sounds table
    }

    lua_pop(luaState, 1); // root table
    return true;    
}

bool ScriptContentLoader::loadAnimationManifest(const std::string& fileName, std::vector<AnimationManifestEntry>& outAnimations, const std::function<bool(int, const std::string&)>& reportError) {
    outAnimations.clear();

    if (luaState == nullptr)
        return false;

    if (!loadLuaFileResultTable(luaState, fileName, reportError))
        return false;

    const int rootIndex = lua_gettop(luaState);

    lua_getfield(luaState, rootIndex, "animations");
    if (!lua_istable(luaState, -1)) {
        lua_pop(luaState, 2); // animations + root
        VG_ERROR("[Lua] Animation manifest '" + fileName + "' must contain an 'animations' table.");
        return false;
    }

    const int animationsIndex = lua_gettop(luaState);
    const int count = static_cast<int>(lua_rawlen(luaState, animationsIndex));

    for (int i = 1; i <= count; ++i) {
        lua_geti(luaState, animationsIndex, i);
        if (!lua_istable(luaState, -1)) {
            lua_pop(luaState, 3); // bad entry + animations + root
            VG_ERROR("[Lua] Animation manifest '" + fileName + "' contains a non-table animation entry.");
            return false;
        }

        const int entryIndex = lua_gettop(luaState);

        AnimationManifestEntry entry;
        if (!readStringField(luaState, entryIndex, "id", entry.id) ||
            !readStringField(luaState, entryIndex, "path", entry.path)) {
            lua_pop(luaState, 3); // entry + animations + root
            VG_ERROR("[Lua] Animation manifest '" + fileName + "' has an animation entry missing 'id' or 'path'.");
            return false;
        }

        outAnimations.push_back(std::move(entry));
        lua_pop(luaState, 1); // entry
    }

    lua_pop(luaState, 2); // animations + root
    return true;
}

bool ScriptContentLoader::loadEntityDefinition(const std::string& fileName, EntityDefinition& outDefinition, const std::function<bool(int, const std::string&)>& reportError) {
    if (luaState == nullptr)
        return false;

    if (!loadLuaFileResultTable(luaState, fileName, reportError))
        return false;

    const int tableIndex = lua_gettop(luaState);

    readStringField(luaState, tableIndex, "texture", outDefinition.texture);
    readStringField(luaState, tableIndex, "animation", outDefinition.animation);
    readVec2Field(luaState, tableIndex, "scale", outDefinition.scale);

    if (readVec2Field(luaState, tableIndex, "bounds_offset", outDefinition.boundsOffset)) {
        outDefinition.hasBounds = true;
    }

    if (readVec2Field(luaState, tableIndex, "bounds_size", outDefinition.boundsSize)) {
        outDefinition.hasBounds = true;
    }

    readStringField(luaState, tableIndex, "script", outDefinition.behaviorScript);

    lua_getfield(luaState, tableIndex, "physics_enabled");
    if (lua_isboolean(luaState, -1)) {
        outDefinition.physicsEnabled = lua_toboolean(luaState, -1) != 0;
    }
    lua_pop(luaState, 1);

    // Read optional custom data fields
    readPropertyBagField(luaState, tableIndex, "data", outDefinition.customData);

    lua_pop(luaState, 1); // pop returned table
    return true;
}

bool ScriptContentLoader::loadAnimationDefinition(const std::string& fileName, AnimationDefinition& outDefinition,const std::function<bool(int, const std::string&)>& reportError) {
    if (luaState == nullptr)
        return false;

    if (!loadLuaFileResultTable(luaState, fileName, reportError))
        return false;

    const int tableIndex = lua_gettop(luaState);

    lua_getfield(luaState, tableIndex, "texture");
    if (lua_isstring(luaState, -1)) {
        outDefinition.texture = lua_tostring(luaState, -1);
    }
    lua_pop(luaState, 1);

    readIntField(luaState, tableIndex, "rows", outDefinition.rows);
    readIntField(luaState, tableIndex, "columns", outDefinition.columns);
    readIntField(luaState, tableIndex, "row", outDefinition.row);
    readIntArrayField(luaState, tableIndex, "frames", outDefinition.frames);

    lua_getfield(luaState, tableIndex, "frame_duration");
    if (lua_isnumber(luaState, -1)) {
        outDefinition.frameDuration = lua_tonumber(luaState, -1);
    }
    lua_pop(luaState, 1);

    lua_getfield(luaState, tableIndex, "looping");
    if (lua_isboolean(luaState, -1)) {
        outDefinition.looping = lua_toboolean(luaState, -1) != 0;
    }
    lua_pop(luaState, 1);

    lua_pop(luaState, 1); // pop returned table
    return true;
}

bool ScriptContentLoader::loadParticlePresetManifest(const std::string& fileName,
                                std::vector<ParticlePresetManifestEntry>& outPresets, const std::function<bool(int, const std::string&)>& reportError) {
    outPresets.clear();

    if (luaState == nullptr)
        return false;

    if (!loadLuaFileResultTable(luaState, fileName, reportError))
        return false;

    const int rootIndex = lua_gettop(luaState);

    lua_getfield(luaState, rootIndex, "emitters");
    if (!lua_istable(luaState, -1)) {
        lua_pop(luaState, 2); // presets + root
        VG_ERROR("[Lua] Particle emitter preset manifest '" + fileName + "' must contain an 'emitters' table.");
        return false;
    }

    const int presetsIndex = lua_gettop(luaState);
    const int count = static_cast<int>(lua_rawlen(luaState, presetsIndex));

    for (int i = 1; i <= count; ++i) {
        lua_geti(luaState, presetsIndex, i);
        if (!lua_istable(luaState, -1)) {
            lua_pop(luaState, 3); // bad entry + presets + root
            VG_ERROR("[Lua] Particle emitter preset manifest '" + fileName + "' contains a non-table emitter entry.");
            return false;
        }

        const int entryIndex = lua_gettop(luaState);

        ParticlePresetManifestEntry entry;
        if (!readStringField(luaState, entryIndex, "id", entry.id) ||
            !readStringField(luaState, entryIndex, "path", entry.path)) {
            lua_pop(luaState, 3); // entry + emitters + root
            VG_ERROR("[Lua] Particle emitter preset manifest '" + fileName + "' has an emitter entry missing 'id' or 'path'.");
            return false;
        }

        outPresets.push_back(std::move(entry));
        lua_pop(luaState, 1); // entry
    }

    lua_pop(luaState, 2); // presets + root
    return true;
}

bool ScriptContentLoader::loadParticlePresetDefinition(const std::string& fileName, ParticlePreset& outPreset, const std::function<bool(int, const std::string&)>& reportError) {
    if (luaState == nullptr)
        return false;

    if (!loadLuaFileResultTable(luaState, fileName, reportError))
        return false;

    const int tableIndex = lua_gettop(luaState);

    readStringField(luaState, tableIndex, "texture", outPreset.textureId);
    readIntField(luaState, tableIndex, "capacity", outPreset.capacity);
    readVec4Field(luaState, tableIndex, "base_color", outPreset.baseColor);
    readFloatField(luaState, tableIndex, "base_size", outPreset.baseSize);
    readFloatField(luaState, tableIndex, "base_lifetime", outPreset.baseLifetime);
    readVec2Field(luaState, tableIndex, "base_velocity", outPreset.baseVelocity);
    readVec2Field(luaState, tableIndex, "velocity_variance", outPreset.velocityVariance);

    lua_pop(luaState, 1); // returned table
    return true;
}
