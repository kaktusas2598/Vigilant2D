#include "Scene.hpp"

#include <algorithm>

void Scene::update(float dt) {
    if (tileMap)
        tileMap->update(dt);

    physicsWorld.step(dt);

    for (auto& entity: entities) {
        if (entity->hasPhysicsBody()) {
            const glm::vec2 centre = physicsWorld.getBodyPositionPixels(entity->getPhysicsBody());
            // Only if body/bound size is same as sprite size
            // entity->transform.position = centre - entity->transform.scale * 0.5f;
            const glm::vec2 boundsSize = entity->getBoundsSize();
            const glm::vec2 boundsPos = centre - boundsSize * 0.5f;
            entity->transform.position = boundsPos - entity->getBoundsOffset();
        }

        entity->update(dt);
    }
}

void Scene::render(Renderer &renderer, const Camera2D &camera, int viewportWidth, int viewportHeight) {
    if (tileMap) {
        tileMap->rebuildVisibleLayers(camera, viewportWidth, viewportHeight);
        tileMap->drawBackgroundLayers(renderer);
    }

    for (auto& entity: entities) {
        entity->render(renderer);
    }

    if (tileMap) {
        tileMap->drawForegroundLayers(renderer);
    }
}

void Scene::clear() {
    tileMap.reset();
    entities.clear();
    physicsWorld.clear();
    pendingDestroyedEntityIds.clear();
}

void Scene::drawPhysicsDebug(Renderer& renderer) const {
    physicsWorld.drawDebug(renderer);
}

Entity& Scene::createEntity(const std::string& id) {
    // TODO: We want to make sure ids are unique!! Not to be confused by definitionIds which refer
    // to entity definitions to spawn many entities of same type
    entities.push_back(std::make_unique<Entity>());
    Entity& entity = *entities.back();
    entity.setID(id);
    return entity;
}

bool Scene::destroyEntity(const std::string& id) {
    // Make sure item does not get request to be destroyed more than once in quick succession
    if (std::find(pendingDestroyedEntityIds.begin(), pendingDestroyedEntityIds.end(), id) != pendingDestroyedEntityIds.end()) {
        return true;
    }

    pendingDestroyedEntityIds.push_back(id);
    return true;
}

void Scene::clearPendingDestroyedEntityIds() {
    pendingDestroyedEntityIds.clear();
}

bool Scene::eraseEntityImmediately(const std::string& id) {
    for (auto it = entities.begin(); it != entities.end(); ++it) {
        if (*it && (*it)->getID() == id) {
            entities.erase(it);
            return true;
        }
    }

    return false;
}

void Scene::setTileMap(std::unique_ptr<TileMap> newMap) {
    tileMap = std::move(newMap);
    if (tileMap) {
        physicsWorld.buildStaticCollisionFromMap(tileMap->getData());
    }
}

Entity* Scene::findEntityByID(const std::string &id) {
    for (auto& entity : entities) {
        if (entity->getID() == id) {
            return entity.get();
        }
    }
    return nullptr;
}

const Entity* Scene::findEntityByID(const std::string &id) const {
    for (const auto& entity : entities) {
        if (entity->getID() == id) {
            return entity.get();
        }
    }
    return nullptr;
}
