#include "ScreenFlowSystem.hpp"

#include "core/Logger.hpp"
#include "UISystem.hpp"

ScreenFlowSystem::ScreenFlowSystem(UISystem& uiSystem, ScriptSystem& scriptSystem)
    : uiSystem(uiSystem), scriptSystem(scriptSystem) {}

bool ScreenFlowSystem::loadScreens(const std::vector<ScreenDefinition>& definitions) {
    screens.clear();
    currentBaseScreenId.clear();
    currentOverlayScreenId.clear();
    gameplayPaused = false;
    sessionResetRequested = false;
    requestedResetBaseScreenId = "main_menu";
    mapWarpRequested = false;
    requestWarpMapPath.clear();
    requestWarpSpawnName.clear();

    std::string initialScreenId;
    for (const ScreenDefinition& definition: definitions) {
        if (definition.id.empty()) {
            VG_ERROR("Screen definition is missing an ID.");
            return false;
        }

        if (screens.find(definition.id) != screens.end()) {
            VG_ERROR("Duplicate screen ID found: '" + definition.id + "'.");
            return false;
        }

        ScreenEntry entry;
        entry.definition = definition;
        if (!definition.script.empty()) {
            entry.scriptInstance = scriptSystem.loadBehavior(definition.script);
            if (entry.scriptInstance.tableRef == LUA_NOREF) {
                VG_ERROR("Failed to load script '" + definition.script + "' for screen '" + definition.id + "'.");
                return false;
            }
        }

        screens[definition.id] = std::move(entry);

        if (definition.initial) {
            if (!initialScreenId.empty()) {
                VG_ERROR("Multiple screens marked as initial: '" + initialScreenId + "' and '" + definition.id + "'.");
                return false;
            }
            initialScreenId = definition.id;
        }
    }

    for (auto& pair : screens) {
        if (!buildScreen(pair.second)) {
            return false;
        }
    }

    applyVisibility();
    if (!initialScreenId.empty()) {
            return showBase(initialScreenId);
    }

    return true;
}

bool ScreenFlowSystem::showBase(const std::string& screenId) {
    ScreenEntry* next = findScreen(screenId);
    if (next == nullptr) {
        VG_ERROR("No screen found with ID '" + screenId + "'.");
        return false;
    }

    if (next->definition.overlay) {
        VG_ERROR("Cannot use overlay screen '" + screenId + "' as base screen.");
        return false;
    }

    ScreenEntry* currentOverlay = findScreen(currentOverlayScreenId);
    if (currentOverlay != nullptr && currentOverlay->scriptInstance.tableRef != LUA_NOREF) {
        scriptSystem.callTableFunction(currentOverlay->scriptInstance, "on_exit");
    }

    ScreenEntry* currentBase = findScreen(currentBaseScreenId);
    if (currentBase != nullptr && currentBase->scriptInstance.tableRef != LUA_NOREF) {
        scriptSystem.callTableFunction(currentBase->scriptInstance, "on_exit");
    }

    currentBaseScreenId = screenId;
    currentOverlayScreenId.clear();

    applyVisibility();
    refreshGameplayPaused();

    if (next->scriptInstance.tableRef != LUA_NOREF) {
        scriptSystem.callTableFunction(next->scriptInstance, "on_enter");
    }

    return true;
}

bool ScreenFlowSystem::showOverlay(const std::string& screenId) {
    ScreenEntry* next = findScreen(screenId);
    if (next == nullptr) {
        VG_ERROR("No screen found with ID '" + screenId + "'.");
        return false;
    }

    if (!next->definition.overlay) {
        VG_ERROR("Cannot use base screen '" + screenId + "' as overlay screen.");
        return false;
    }

    if (currentOverlayScreenId == screenId) {
        return true;
    }

    ScreenEntry* currentOverlay = findScreen(currentOverlayScreenId);
    if (currentOverlay != nullptr && currentOverlay->scriptInstance.tableRef != LUA_NOREF) {
        scriptSystem.callTableFunction(currentOverlay->scriptInstance, "on_exit");
    }

    currentOverlayScreenId = screenId;
    applyVisibility();
    refreshGameplayPaused();

    if (next->scriptInstance.tableRef != LUA_NOREF) {
        scriptSystem.callTableFunction(next->scriptInstance, "on_enter");
    }

    return true;
}

bool ScreenFlowSystem::clearOverlay() {
    if (currentOverlayScreenId.empty())
        return true;

    ScreenEntry* currentOverlay = findScreen(currentOverlayScreenId);
    if (currentOverlay != nullptr && currentOverlay->scriptInstance.tableRef != LUA_NOREF) {
        scriptSystem.callTableFunction(currentOverlay->scriptInstance, "on_exit");
    }

    currentOverlayScreenId.clear();
    applyVisibility();
    refreshGameplayPaused();

    return true;
}

bool ScreenFlowSystem::toggleOverlay(const std::string& screenId) {
    if (currentOverlayScreenId == screenId)
        return clearOverlay();

    return showOverlay(screenId);
}

void ScreenFlowSystem::update(float dt) {
    ScreenEntry* base = findScreen(currentBaseScreenId);
    ScreenEntry* overlay = findScreen(currentOverlayScreenId);

    if (base != nullptr && base->scriptInstance.tableRef != LUA_NOREF) {
        scriptSystem.callTableFunction(base->scriptInstance, "on_update", dt);
    }

    if (overlay != nullptr && overlay->scriptInstance.tableRef != LUA_NOREF) {
        scriptSystem.callTableFunction(overlay->scriptInstance, "on_update", dt);
    }
}

void ScreenFlowSystem::requestSessionReset(const std::string& baseScreenId) {
    sessionResetRequested = true;
    requestedResetBaseScreenId = baseScreenId.empty() ? "main_menu" : baseScreenId;
}

bool ScreenFlowSystem::consumeSessionResetRequest(std::string& outBaseScreenId) {
    if (!sessionResetRequested)
        return false;

    sessionResetRequested = false;
    outBaseScreenId = requestedResetBaseScreenId;
    requestedResetBaseScreenId = "main_menu";
    return true;
}

void ScreenFlowSystem::requestMapWarp(const std::string& mapPath, const std::string& spawnName) {
    mapWarpRequested = !mapPath.empty();
    requestWarpMapPath = mapPath;
    requestWarpSpawnName = spawnName;
}

bool ScreenFlowSystem::consumeMapWarpRequest(std::string& outMapPath, std::string& outSpawnName) {
    if (!mapWarpRequested)
        return false;
    
    mapWarpRequested = false;
    outMapPath = requestWarpMapPath;
    outSpawnName = requestWarpSpawnName;
    requestWarpMapPath.clear();
    requestWarpSpawnName.clear();
    return true;
}

ScreenFlowSystem::ScreenEntry* ScreenFlowSystem::findScreen(const std::string& id) {
    auto it = screens.find(id);
    return it != screens.end() ? &it->second : nullptr;
}

const ScreenFlowSystem::ScreenEntry* ScreenFlowSystem::findScreen(const std::string& id) const {
    auto it = screens.find(id);
    return it != screens.end() ? &it->second : nullptr;
}

bool ScreenFlowSystem::buildScreen(ScreenEntry& entry) {
    if (entry.built)
        return true;

    if (entry.scriptInstance.tableRef != LUA_NOREF) {
        if (!scriptSystem.callTableFunction(entry.scriptInstance, "build")) {
            return false;
        }
    }

    entry.built = true;
    return true;
}

void ScreenFlowSystem::setScreenGroupsVisible(const ScreenEntry* entry, bool visible) {
    if (entry == nullptr)
        return;

    for (const std::string& group : entry->definition.groups) {
        uiSystem.setGroupVisible(group, visible);
    }
}

void ScreenFlowSystem::applyVisibility() {
    for (auto& pair : screens) {
        setScreenGroupsVisible(&pair.second, false);
    }

    setScreenGroupsVisible(findScreen(currentBaseScreenId), true);
    setScreenGroupsVisible(findScreen(currentOverlayScreenId), true);
}

void ScreenFlowSystem::refreshGameplayPaused() {
    gameplayPaused = false;

    const ScreenEntry* baseEntry = findScreen(currentBaseScreenId);
    if (baseEntry != nullptr && baseEntry->definition.pausesGameplay) {
        gameplayPaused = true;
        return;
    }

    const ScreenEntry* overlayEntry = findScreen(currentOverlayScreenId);
    if (overlayEntry != nullptr && overlayEntry->definition.pausesGameplay) {
        gameplayPaused = true;
        return;
    }
}
