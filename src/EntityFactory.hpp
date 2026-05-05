#pragma once

#include <string>
#include "glm/glm.hpp"

class Scene;
class AssetManager;
class ScriptSystem;
class Entity;
class AnimationClip;

class EntityFactory {
    public:
        EntityFactory(Scene& scene, AssetManager& assetManager, ScriptSystem& scriptSystem);

        // TODO: probably makes more sense to load position from definition?
        Entity* spawnFromDefinition(const std::string& entityId,
                                    const std::string& definitionFile,
                                    const glm::vec2& position);
    private:
        Scene& scene;
        AssetManager& assetManager;
        ScriptSystem& scriptSystem;
};