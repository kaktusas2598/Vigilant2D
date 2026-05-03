#include "Scene.hpp"

void Scene::update(float dt) {
    physicsWorld.step(dt);
    for (auto& entity: entities) {
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
