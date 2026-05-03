#pragma once

#include <vector>
#include <box2d/box2d.h>
#include "glm/glm.hpp"

class TileMapData;

class PhysicsWorld2D {
    public:
        PhysicsWorld2D();
        void step(float dt);

        b2BodyId createStaticBox(float centerX, float centerY, float halfWidth, float halfHeight);
        void buildStaticCollisionFromMap(const TileMapData& map);

        b2BodyId createDynamicBox(const glm::vec2& positionPixels, const glm::vec2& sizePixels);
        glm::vec2 getBodyPositionPixels(b2BodyId bodyId) const;
        void setBodyLinearVelocityPixels(b2BodyId bodyId, const glm::vec2& velocityPixelsPerSecond);

        b2WorldId& getWorldId() { return worldId; }
        const std::vector<b2BodyId>& getStaticBodies() const { return staticBodies; }

    private:
        static constexpr float pixelsPerMeter = 32.0f;
        float toMeters(float pixels) const { return pixels / pixelsPerMeter; }
        float toPixels(float meters) const { return meters * pixelsPerMeter; }

        b2WorldId worldId;
        std::vector<b2BodyId> staticBodies;
};