#pragma once

#include <vector>
#include <box2d/box2d.h>

class TileMapData;

class PhysicsWorld2D {
    public:
        PhysicsWorld2D();
        void step(float dt);

        b2BodyId createStaticBox(float centerX, float centerY, float halfWidth, float halfHeight);
        void buildStaticCollisionFromMap(const TileMapData& map);

        b2WorldId& getWorld() { return worldId; }
        const std::vector<b2BodyId>& getStaticBodies() const { return staticBodies; }

    private:
        b2WorldId worldId;
        std::vector<b2BodyId> staticBodies;
};