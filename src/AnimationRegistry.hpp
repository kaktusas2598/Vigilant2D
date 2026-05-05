#pragma once

#include <string>
#include <unordered_map>

#include "AnimationClip.hpp"
#include "AnimationDefinition.hpp"

class AssetManager;
class ScriptSystem;

class AnimationRegistry {
    public:
        bool loadClip(const std::string& clipId, const std::string& definitionFile,
                    AssetManager& assetManager, ScriptSystem& scriptSystem);

        const AnimationClip* getClip(const std::string& clipId) const;
        bool hasClip(const std::string& clipId) const;

    private:
        std::unordered_map<std::string, AnimationClip> clips;
};