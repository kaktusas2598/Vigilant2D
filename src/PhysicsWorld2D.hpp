#pragma once

#include <vector>
#include <box2d/box2d.h>
#include "glm/glm.hpp"

#include "Renderer.hpp"
#include "Transform2D.hpp"
#include "TextureRegion.hpp"

struct DebugBodyBox { 
    b2BodyId bodyId = b2_nullBodyId;
    glm::vec2 sizePixels{0.0f};
    glm::vec4 color{1.0f};
    bool dynamic = false;
};

class TileMapData;

class PhysicsWorld2D {
    public:
        PhysicsWorld2D();
        void step(float dt);
        void clear();

        b2BodyId createBox(const glm::vec2& positionPixels,
                    const glm::vec2& sizePixels, b2BodyType bodyType);

        b2BodyId createStaticBox(float centerX, float centerY, float halfWidth, float halfHeight);
        void buildStaticCollisionFromMap(const TileMapData& map);

        b2BodyId createDynamicBox(const glm::vec2& positionPixels, const glm::vec2& sizePixels);
        glm::vec2 getBodyPositionPixels(b2BodyId bodyId) const;
        void setBodyLinearVelocityPixels(b2BodyId bodyId, const glm::vec2& velocityPixelsPerSecond);
        void setBodyPositionPixels(b2BodyId bodyId, const glm::vec2& centrePixels);

        void drawDebug(Renderer& renderer) const;
        int getStaticBodyCount() const { return static_cast<int>(staticBodies.size()); }
        b2WorldId& getWorldId() { return worldId; }
        const std::vector<b2BodyId>& getStaticBodies() const { return staticBodies; }
        void destroyBody(b2BodyId bodyId);

    private:
        void createWorld();
        static constexpr float pixelsPerMeter = 32.0f;
        float toMeters(float pixels) const { return pixels / pixelsPerMeter; }
        float toPixels(float meters) const { return meters * pixelsPerMeter; }

        b2WorldId worldId;
        std::vector<b2BodyId> staticBodies;
        std::vector<DebugBodyBox> debugBodies;
};