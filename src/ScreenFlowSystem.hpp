#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "ScreenDefinition.hpp"
#include "ScriptSystem.hpp"

class UISystem;

class ScreenFlowSystem {
public:
    ScreenFlowSystem(UISystem& uiSystem, ScriptSystem& scriptSystem);

    bool loadScreens(const std::vector<ScreenDefinition>& definitions);

    bool showBase(const std::string& screenId);
    bool showOverlay(const std::string& screenId);
    bool clearOverlay();
    bool toggleOverlay(const std::string& screenId);

    void update(float dt);

    bool isGameplayPaused() const { return gameplayPaused; }

    void requestSessionReset(const std::string& baseScreenId);
    bool consumeSessionResetRequest(std::string& outBaseScreenId);

    void requestMapWarp(const std::string& mapPath, const std::string& spawnName);
    bool consumeMapWarpRequest(std::string& outMapPath, std::string& outSpawnName);

    const std::string& getCurrentBase() const { return currentBaseScreenId; }
    const std::string& getCurrentOverlay() const { return currentOverlayScreenId; }

private:
    struct ScreenEntry {
        ScreenDefinition definition;
        ScriptInstance scriptInstance;
        bool built = false;
    };

    ScreenEntry* findScreen(const std::string& id);
    const ScreenEntry* findScreen(const std::string& id) const;

    bool buildScreen(ScreenEntry& entry);
    void setScreenGroupsVisible(const ScreenEntry* entry, bool visible);
    void applyVisibility();
    void refreshGameplayPaused();

    UISystem& uiSystem;
    ScriptSystem& scriptSystem;

    std::unordered_map<std::string, ScreenEntry> screens;
    std::string currentBaseScreenId;
    std::string currentOverlayScreenId;
    bool gameplayPaused = false;

    bool sessionResetRequested = false;
    std::string requestedResetBaseScreenId;

    bool mapWarpRequested = false;
    std::string requestWarpMapPath;
    std::string requestWarpSpawnName;
};