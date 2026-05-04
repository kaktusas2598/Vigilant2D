#include "Scene.hpp"

void Scene::update(float dt) {
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
        tileMap->draw(renderer);
    }

    for (auto& entity: entities) {
        entity->render(renderer);
    }
}

void Scene::drawPhysicsDebug(Renderer& renderer) const {
    physicsWorld.drawDebug(renderer);
}

Entity& Scene::createEntity(const std::string& id) {
    entities.push_back(std::make_unique<Entity>());
    Entity& entity = *entities.back();
    entity.setID(id);
    return entity;
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
