#pragma once

#include <string>
#include "glm/glm.hpp"

class Scene;
class AssetManager;
class ScriptSystem;
class Entity;
class AnimationRegistry;
class TileMapData;

class EntityFactory {
    public:
        EntityFactory(Scene& scene,
                      AssetManager& assetManager, 
                      ScriptSystem& scriptSystem, 
                      AnimationRegistry& animationRegistry);

        Entity* spawnFromDefinition(const std::string& entityId,
                                    const std::string& definitionFile,
                                    const glm::vec2& position);

        Entity* spawnRuntime(const std::string& definitionId,
                        const glm::vec2& position,
                        std::string* outRuntimeId = nullptr);
        
        void spawnFromMapObjects(const TileMapData& mapData, const std::string& layerName);
    private:
        Scene& scene;
        AssetManager& assetManager;
        ScriptSystem& scriptSystem;
        AnimationRegistry& animationRegistry;

        unsigned int runtimeIdCounter = 0;
};