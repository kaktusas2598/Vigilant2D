#include "ScriptSystem.hpp"

#include <functional>
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

// ----------------------------------


bool ScriptSystem::init() {
    if (luaState != nullptr)
        return true;

    luaState = luaL_newstate();
    if (luaState == nullptr) {
        VG_ERROR("[Lua] Failed to create lua_State.");
        return false;
    }

    luaL_openlibs(luaState);
    return true;
}

void ScriptSystem::shutdown() {
    if (luaState != nullptr) {
        lua_close(luaState);
        luaState = nullptr;
    }
}

bool ScriptSystem::loadAssetManifest(const std::string& fileName,
     std::vector<TextureManifestEntry>& outTextures, std::vector<FontManifestEntry>& outFonts) {
    outTextures.clear();
    outFonts.clear();

    if (luaState == nullptr && !init())
        return false;

    auto reportLuaError = [this](int status, const std::string& context) {
        return reportError(status, context);
    };
    if (!loadLuaFileResultTable(luaState, fileName, reportLuaError))
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

    lua_pop(luaState, 1); // root table
    return true;    
}

bool ScriptSystem::loadAnimationManifest(const std::string& fileName, std::vector<AnimationManifestEntry>& outAnimations) {
    outAnimations.clear();

    if (luaState == nullptr && !init())
        return false;

    auto reportLuaError = [this](int status, const std::string& context) {
        return reportError(status, context);
    };
    if (!loadLuaFileResultTable(luaState, fileName, reportLuaError))
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

bool ScriptSystem::loadEntityDefinition(const std::string& fileName, EntityDefinition& outDefinition) {
    if (luaState == nullptr && !init())
        return false;

    auto reportLuaError = [this](int status, const std::string& context) {
        return reportError(status, context);
    };
    if (!loadLuaFileResultTable(luaState, fileName, reportLuaError))
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

    lua_pop(luaState, 1); // pop returned table
    return true;
}

bool ScriptSystem::loadAnimationDefinition(const std::string& fileName, AnimationDefinition& outDefinition) {
    if (luaState == nullptr && !init())
        return false;

    auto reportLuaError = [this](int status, const std::string& context) {
        return reportError(status, context);
    };
    if (!loadLuaFileResultTable(luaState, fileName, reportLuaError))
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

ScriptInstance ScriptSystem::loadBehavior(const std::string& fileName) {
    ScriptInstance instance;
    instance.fileName = fileName;

    if (luaState == nullptr && !init())
        return instance;

    const int loadStatus = luaL_loadfile(luaState, fileName.c_str());
    if (!reportError(loadStatus, "luaL_loadfile(" + fileName + ")"))
        return instance;

    const int callStatus = lua_pcall(luaState, 0, 1, 0);
    if (!reportError(callStatus, "execute(" + fileName + ")"))
        return instance;

    if (!lua_istable(luaState, -1)) {
        VG_ERROR("[Lua] Script '" + fileName + "' must return a table.");
        lua_pop(luaState, 1);
        return instance;
    }

    instance.tableRef = luaL_ref(luaState, LUA_REGISTRYINDEX);
    return instance;
}

bool ScriptSystem::callOnCreate(const ScriptInstance& instance) {
    if(luaState == nullptr || instance.tableRef == LUA_NOREF)
        return false;

    lua_rawgeti(luaState, LUA_REGISTRYINDEX, instance.tableRef); // push table
    lua_getfield(luaState, -1, "on_create"); // push field - function in this case

    if (!lua_isfunction(luaState, -1)) {
        lua_pop(luaState, 2);
        return true;
    }

    // Remove table at -2, leaving function at the top of the stack
    lua_remove(luaState, -2);

    const int status = lua_pcall(luaState, 0, 0, 0);
    return reportError(status, "callOnCreate(" + instance.fileName + ")");
}

bool ScriptSystem::callOnUpdate(const ScriptInstance& instance, float dt) {
    if(luaState == nullptr || instance.tableRef == LUA_NOREF)
        return false;

    lua_rawgeti(luaState, LUA_REGISTRYINDEX, instance.tableRef); // push table
    lua_getfield(luaState, -1, "on_update"); // push field - function in this case

    if (!lua_isfunction(luaState, -1)) {
        lua_pop(luaState, 2);
        return true;
    }

    // Remove table at -2, leaving function at the top of the stack
    lua_remove(luaState, -2);
    lua_pushnumber(luaState, dt);

    const int status = lua_pcall(luaState, 1, 0, 0);
    return reportError(status, "callOnUpdate(" + instance.fileName + ")");

}

void ScriptSystem::releaseInstance(ScriptInstance& instance) {
    if (luaState != nullptr && instance.tableRef != LUA_NOREF) {
        luaL_unref(luaState, LUA_REGISTRYINDEX, instance.tableRef);
    }

    instance.tableRef = LUA_NOREF;
    instance.fileName.clear();
}

bool ScriptSystem::attachToEntity(const Entity& entity) {
    if (!entity.hasScript())
        return false;

    auto it = entityScripts.find(entity.getID());
    if (it != entityScripts.end()) {
        releaseInstance(it->second);
        entityScripts.erase(it);
    }

    ScriptInstance instance = loadBehavior(entity.getScriptName());
    if (instance.tableRef == LUA_NOREF)
        return false;

    entityScripts[entity.getID()] = std::move(instance);
    return true;
}

bool ScriptSystem::callEntityOnCreate(const Entity& entity) {
    auto it = entityScripts.find(entity.getID());
    if (it == entityScripts.end())
        return false;
    return callOnCreate(it->second);
}

bool ScriptSystem::callEntityOnUpdate(const Entity& entity, float dt) {
    auto it = entityScripts.find(entity.getID());
    if (it == entityScripts.end())
        return false;
    return callOnUpdate(it->second, dt);
}

void ScriptSystem::detachFromEntity(const Entity& entity) {
    auto it = entityScripts.find(entity.getID());
    if (it == entityScripts.end())
        return;

    releaseInstance(it->second);
    entityScripts.erase(it);
}

bool ScriptSystem::reportError(int status, const std::string &context) {
    if (status == LUA_OK)
        return true;
    
    const char* message = lua_tostring(luaState, -1);
    VG_ERROR("[Lua] " + context + " failed: " + (message != nullptr ? message : "unkown error"));
    lua_pop(luaState, 1);
    return false;
}