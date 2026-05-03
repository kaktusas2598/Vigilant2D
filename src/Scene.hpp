#pragma once

#include <vector>
#include <memory>

#include "Entity.hpp"
#include "TileMap.hpp"
#include "Renderer.hpp"
#include "Camera2D.hpp"
#include "PhysicsWorld2D.hpp"

class Scene {
    public:
        void update(float dt);
        void render(Renderer& renderer, const Camera2D& camera, int viewportWidth, int viewportHeight);

        Entity& createEntity(const std::string& id);
        void setTileMap(std::unique_ptr<TileMap> newMap);
    
        Entity* findEntityByID(const std::string& id);
        const Entity* findEntityByID(const std::string& id) const;

        TileMap* getTileMap() { return tileMap.get(); }
        const TileMap* getTileMap() const { return tileMap.get(); }

        PhysicsWorld2D& getPhysicsWorld() { return physicsWorld; }
        const PhysicsWorld2D &getPhysicsWorld() const { return physicsWorld; }

        const std::vector<std::unique_ptr<Entity>>& getEntities() const { return entities; }
        unsigned int getEntityCount() const { return entities.size(); }
    private:
        // TODO: might want make more than 1 map available
        std::unique_ptr<TileMap> tileMap;
        std::vector<std::unique_ptr<Entity>> entities;
        PhysicsWorld2D physicsWorld;

};