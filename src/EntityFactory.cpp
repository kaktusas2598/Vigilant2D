#include "EntityFactory.hpp"

#include "Scene.hpp"
#include "core/Logger.hpp"
#include "Entity.hpp"
#include "AssetManager.hpp"
#include "ScriptSystem.hpp"
#include "AnimationRegistry.hpp"
#include "Sprite.hpp"
#include "AnimatedSprite.hpp"
#include "Texture.hpp"
#include "TileMapData.hpp" // for makeRegionFromGrid

EntityFactory::EntityFactory(Scene& scene,
                             AssetManager& assetManager,
                             ScriptSystem& scriptSystem,
                             AnimationRegistry& animationRegistry)
    : scene(scene), assetManager(assetManager), scriptSystem(scriptSystem), animationRegistry(animationRegistry) {
}

Entity* EntityFactory::spawnFromDefinition(const std::string& entityId,
                            const std::string& definitionFile,
                            const glm::vec2& position) {
    EntityDefinition definition;
    if (!scriptSystem.loadEntityDefinition(definitionFile, definition)) {
        return nullptr;
    }

    Entity& entity = scene.createEntity(entityId);
    entity.transform.position = position;
    entity.transform.scale = definition.scale;

    if (definition.hasBounds) {
        entity.setBounds(definition.boundsOffset, definition.boundsSize);
    }

    if (!definition.texture.empty()) {
        Texture* texture = assetManager.getTexture(definition.texture);
        if (texture != nullptr) {
            auto sprite = std::make_unique<Sprite>();
            if (definition.hasTextureGrid) {
                sprite->setRegion(makeRegionFromGrid(
                    texture,
                    definition.textureGridColumn,
                    definition.textureGridRow,
                    definition.textureGridColumns,
                    definition.textureGridRows
                ));
            } else {
                sprite->setRegion(TextureRegion::full(texture));
            }
            entity.setSprite(std::move(sprite));
        }
    }

    if (!definition.animation.empty()) {
        const AnimationClip* clip = animationRegistry.getClip(definition.animation);
        if (clip != nullptr) {
            auto animatedSprite = std::make_unique<AnimatedSprite>();
            if (entity.getSprite() != nullptr) {
                animatedSprite->setSprite(entity.getSprite());
                animatedSprite->play(clip);
                entity.setAnimatedSprite(std::move(animatedSprite));
            } else {
                VG_ERROR("When setting 'animation' attribute, entity definition file must also include 'texture'");
            }
        }
    }

    entity.getCustomData() = definition.customData;

    std::string definitionId = definitionFile;
    const size_t slash = definitionId.find_last_of("/\\");
    if (slash != std::string::npos)
        definitionId = definitionId.substr(slash + 1);
    const size_t dot = definitionId.find_last_of('.');
    if (dot != std::string::npos)
        definitionId = definitionId.substr(0, dot);

    entity.getCustomData().set("entity_definition", definitionId);

    if (!definition.behaviorScript.empty()) {
        entity.setScript(definition.behaviorScript);
        if (scriptSystem.attachToEntity(entity)) {
            scriptSystem.callEntityOnCreate(entity);
        }
    }

    if (definition.physicsEnabled) {
        glm::vec2 bodyPos = entity.getBoundsPosition();
        glm::vec2 bodySize = entity.getBoundsSize();
        entity.setPhysicsBody(scene.getPhysicsWorld().createDynamicBox(bodyPos, bodySize));
    }

    if (definition.topDownContoller.has_value()) {
        TopDownControllerConfig controllerConfig = *definition.topDownContoller;
        controllerConfig.entityId = entityId;
        entity.setTopDownControllerConfig(controllerConfig);
    }

    return &entity;
}

Entity* EntityFactory::spawnRuntime(const std::string& definitionId,
                        const glm::vec2& position,
                        std::string* outRuntimeId) {
    const std::string& definitionFile = "scripts/entities/" + definitionId + ".lua";
    std::string runtimeId = definitionId + "_" + std::to_string(runtimeIdCounter++);

    Entity* entity = spawnFromDefinition(runtimeId, definitionFile, position);
    if (entity != nullptr && outRuntimeId != nullptr) {
        *outRuntimeId = runtimeId;
    }

    return entity;
}

void EntityFactory::spawnFromMapObjects(const TileMapData& mapData, const std::string& layerName) {
    for (const auto& objectLayer : mapData.objectLayers) {
        if (objectLayer.name != layerName || !objectLayer.visible)
            continue;
        
        for (const auto& object : objectLayer.objects) {
            if (object.shape != MapObjectShape::Point || !object.visible)
                continue;
            
            const std::string* entityId = object.findProperty("entity");
            if (entityId == nullptr || entityId->empty())
                continue;
            // TODO: get rid of hardcoded expected path
            const std::string definitionFile = "scripts/entities/" + *entityId + ".lua";
            glm::vec2 position(
                object.x,
                static_cast<float>(mapData.height * mapData.tileHeight) - object.y
            );

            // For unique Entity identifier we use objects name and "entity" attribute if its not found
            std::string runtimeId = object.name;
            if (runtimeId.empty()) {
                static int spawnedEntityCounter = 0;
                runtimeId = *entityId + "_" + std::to_string(spawnedEntityCounter++);
            }

            Entity* entity = spawnFromDefinition(runtimeId, definitionFile, position);
            if (entity == nullptr)
                continue;

            for (const auto& [key, value] : object.properties) {
                entity->getCustomData().set(key, value);
            }

            // Custom properties
            if (const std::string* animationId = object.findProperty("animation")) {
                if (!animationId->empty()) {
                    if (AnimatedSprite* animatedSprite = entity->getAnimatedSprite()) {
                        if (const AnimationClip* clip = animationRegistry.getClip(*animationId)) {
                            animatedSprite->play(clip);
                        }
                    }
                }
            }
        }
    }
}