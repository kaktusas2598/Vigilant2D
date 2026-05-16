#include "ScriptSystem.hpp"

#include "Logger.hpp"
#include "LuaBindings.hpp"

bool ScriptSystem::init() {
    if (luaState != nullptr)
        return true;

    luaState = luaL_newstate();
    if (luaState == nullptr) {
        VG_ERROR("[Lua] Failed to create lua_State.");
        return false;
    }

    luaL_openlibs(luaState);
    registerEngineBindings(luaState, *this);
    taskRunner.init(luaState);
    contentLoader.init(luaState);
    return true;
}

void ScriptSystem::shutdown() {
    taskRunner.shutdown();

    if (luaState != nullptr) {
        lua_close(luaState);
        luaState = nullptr;
    }
}

//Content Bootstraping methods - functionality delegated to ScriptContentLoader
bool ScriptSystem::loadAssetManifest(const std::string& fileName,
     std::vector<TextureManifestEntry>& outTextures,
     std::vector<FontManifestEntry>& outFonts,
     std::vector<SoundManifestEntry>& outSounds) {
    if (luaState == nullptr && !init())
        return false;

    auto reportLuaError = [this](int status, const std::string& context) {
        return reportError(status, context);
    };
    return contentLoader.loadAssetManifest(fileName, outTextures, outFonts, outSounds, reportLuaError);
}

bool ScriptSystem::loadAnimationManifest(const std::string& fileName, std::vector<AnimationManifestEntry>& outAnimations) {
    if (luaState == nullptr && !init())
        return false;

    auto reportLuaError = [this](int status, const std::string& context) {
        return reportError(status, context);
    };
    return contentLoader.loadAnimationManifest(fileName, outAnimations, reportLuaError);
}

bool ScriptSystem::loadEntityDefinition(const std::string& fileName, EntityDefinition& outDefinition) {
    if (luaState == nullptr && !init())
        return false;

    auto reportLuaError = [this](int status, const std::string& context) {
        return reportError(status, context);
    };
    return contentLoader.loadEntityDefinition(fileName, outDefinition, reportLuaError);
}

bool ScriptSystem::loadAnimationDefinition(const std::string& fileName, AnimationDefinition& outDefinition) {
    if (luaState == nullptr && !init())
        return false;

    auto reportLuaError = [this](int status, const std::string& context) {
        return reportError(status, context);
    };
    return contentLoader.loadAnimationDefinition(fileName, outDefinition, reportLuaError);
}

bool ScriptSystem::loadParticlePresetManifest(const std::string& fileName,
                                std::vector<ParticlePresetManifestEntry>& outPresets) {
    if (luaState == nullptr && !init())
        return false;

    auto reportLuaError = [this](int status, const std::string& context) {
        return reportError(status, context);
    };
    return contentLoader.loadParticlePresetManifest(fileName, outPresets, reportLuaError);
}

bool ScriptSystem::loadParticlePresetDefinition(const std::string& fileName, ParticlePreset& outPreset) {
    if (luaState == nullptr && !init())
        return false;

    auto reportLuaError = [this](int status, const std::string& context) {
        return reportError(status, context);
    };
    return contentLoader.loadParticlePresetDefinition(fileName, outPreset, reportLuaError);
}

bool ScriptSystem::loadScreenDefinitions(const std::string& fileName, std::vector<ScreenDefinition>& outScreens) {
    if (luaState == nullptr && !init())
        return false;

    auto reportLuaError = [this](int status, const std::string& context) {
        return reportError(status, context);
    };
    return contentLoader.loadScreenDefinitions(fileName, outScreens, reportLuaError);
}

// Script instance methods
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

    lua_rawgeti(luaState, LUA_REGISTRYINDEX, instance.tableRef);

    // Attach "id" and "entity_id" to table containing behavior scripts
    lua_pushstring(luaState, entity.getID().c_str());
    lua_setfield(luaState, -2, "id");

    lua_pushstring(luaState, entity.getID().c_str());
    lua_setfield(luaState, -2, "entity_id");

    lua_pop(luaState, 1);

    entityScripts[entity.getID()] = std::move(instance);
    return true;
}

bool ScriptSystem::callEntityOnCreate(const Entity& entity) {
    auto it = entityScripts.find(entity.getID());
    if (it == entityScripts.end())
        return false;
    return callTableFunction(it->second, "on_create");
}

bool ScriptSystem::callEntityOnUpdate(const Entity& entity, float dt) {
    auto it = entityScripts.find(entity.getID());
    if (it == entityScripts.end())
        return false;
    return callTableFunction(it->second, "on_update", dt);
}

void ScriptSystem::detachFromEntity(const Entity& entity) {
    taskRunner.cancelTasksForEntity(entity.getID());

    auto it = entityScripts.find(entity.getID());
    if (it == entityScripts.end())
        return;

    releaseInstance(it->second);
    entityScripts.erase(it);
}

bool ScriptSystem::callTableFunction(const ScriptInstance& instance, const char* functionName) {
    if (luaState == nullptr || instance.tableRef == LUA_NOREF)
        return false;

    lua_rawgeti(luaState, LUA_REGISTRYINDEX, instance.tableRef);
    lua_getfield(luaState, -1, functionName);

    if (!lua_isfunction(luaState, -1)) {
        lua_pop(luaState, 2);
        return true;
    }

    lua_pushvalue(luaState, -2); // table, function, self
    lua_remove(luaState, -3);    // function, self

    const int status = lua_pcall(luaState, 1, 0, 0);
    return reportError(status, std::string("callTableFunction(") + instance.fileName + ":" + functionName + ")");
}

bool ScriptSystem::callTableFunction(const ScriptInstance& instance, const char* functionName, float dt) {
    if (luaState == nullptr || instance.tableRef == LUA_NOREF)
        return false;

    lua_rawgeti(luaState, LUA_REGISTRYINDEX, instance.tableRef);
    lua_getfield(luaState, -1, functionName);

    if (!lua_isfunction(luaState, -1)) {
        lua_pop(luaState, 2);
        return true;
    }

    lua_pushvalue(luaState, -2); // table, function, self
    lua_remove(luaState, -3);    // function, self
    lua_pushnumber(luaState, dt);

    const int status = lua_pcall(luaState, 2, 0, 0);
    return reportError(status, std::string("callTableFunction(") + instance.fileName + ":" + functionName + ")");
}

//Coroutine methods - functionality delegated to ScriptTaskRunner
bool ScriptSystem::startEntityCoroutine(const std::string& ownerEntityId, int functionIndex, int selfIndex) {
    return taskRunner.startEntityCoroutine(ownerEntityId, functionIndex, selfIndex);
}

bool ScriptSystem::startGlobalCoroutine(int functionIndex) {
    return taskRunner.startGlobalCoroutine(functionIndex);
}

void ScriptSystem::updateTasks(float dt) {
    taskRunner.update(dt, runtimeContext.scene);
}

void ScriptSystem::cancelTasksForEntity(const std::string& entityId) {
    taskRunner.cancelTasksForEntity(entityId);
}

bool ScriptSystem::callNamedFunction(const ScriptInstance& instance, const char* functionName) {
    if (luaState == nullptr || instance.tableRef == LUA_NOREF)
        return false;

    lua_rawgeti(luaState, LUA_REGISTRYINDEX, instance.tableRef);
    lua_getfield(luaState, -1, functionName);

    if (!lua_isfunction(luaState, -1)) {
        lua_pop(luaState, 2);
        return true;
    }

    lua_remove(luaState, -2); // remove table, leave function
    const int status = lua_pcall(luaState, 0, 0, 0);
    return reportError(status, std::string("callNamedFunction(") + instance.fileName + ":" + functionName + ")");
}

bool ScriptSystem::runGlobalScriptFunction(const std::string& fileName, const char* functionName) {
    ScriptInstance instance = loadBehavior(fileName);
    if (instance.tableRef == LUA_NOREF)
        return false;

    const bool ok = callNamedFunction(instance, functionName);
    releaseInstance(instance);
    return ok;
}

void ScriptSystem::setRuntimeContext(ScriptRuntimeContext newContext) {
    runtimeContext.scene = newContext.scene;
    runtimeContext.animationRegistry = newContext.animationRegistry;
    runtimeContext.camera = newContext.camera;
    runtimeContext.input = newContext.input;
    runtimeContext.particleEmitterRegistry = newContext.particleEmitterRegistry;
    runtimeContext.assetManager = newContext.assetManager;
    runtimeContext.uiSystem = newContext.uiSystem;
    runtimeContext.audioSystem = newContext.audioSystem;
    runtimeContext.cameraFollowState = newContext.cameraFollowState;
    runtimeContext.postFadeAmount = newContext.postFadeAmount;
    runtimeContext.screenFlowSystem = newContext.screenFlowSystem;
}

bool ScriptSystem::reportError(int status, const std::string &context) {
    if (status == LUA_OK)
        return true;
    
    const char* message = lua_tostring(luaState, -1);
    VG_ERROR("[Lua] " + context + " failed: " + (message != nullptr ? message : "unkown error"));
    lua_pop(luaState, 1);
    return false;
}