#include "EntityFactory.hpp"

#include "Scene.hpp"
#include "Logger.hpp"
#include "Entity.hpp"
#include "AssetManager.hpp"
#include "ScriptSystem.hpp"
#include "AnimationRegistry.hpp"
#include "Sprite.hpp"
#include "AnimatedSprite.hpp"
#include "Texture.hpp"

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
            // TODO: add ability to sample from texture region optionally
            sprite->setRegion(TextureRegion::full(texture));
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

    return &entity;
}
