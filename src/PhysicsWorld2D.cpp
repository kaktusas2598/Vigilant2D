#include "PhysicsWorld2D.hpp"

#include <algorithm>
#include "TileMapData.hpp"

PhysicsWorld2D::PhysicsWorld2D() {
    createWorld();
}

void PhysicsWorld2D::step(float dt) {
    const int subSteps = 4;
    b2World_Step(worldId, dt, subSteps);
}

void PhysicsWorld2D::clear() {
    if (B2_IS_NON_NULL(worldId)) {
        b2DestroyWorld(worldId);
        worldId = b2_nullWorldId;
    }

    staticBodies.clear();
    debugBodies.clear();
    createWorld();
}

b2BodyId PhysicsWorld2D::createBox(const glm::vec2& positionPixels,
                                   const glm::vec2& sizePixels,
                                   b2BodyType bodyType) {
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = bodyType;
    bodyDef.position = {
        toMeters(positionPixels.x + sizePixels.x * 0.5f),
        toMeters(positionPixels.y + sizePixels.y * 0.5f)
    };

    if (bodyType == b2_dynamicBody) {
        bodyDef.gravityScale = 0.0f;
        bodyDef.motionLocks.angularZ = true;
        bodyDef.linearDamping = 8.0f;
    } else if (bodyType == b2_kinematicBody) {
        bodyDef.gravityScale = 0.0f;
        bodyDef.motionLocks.angularZ = true;
    }

    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

    b2Polygon polygon = b2MakeBox(
        toMeters(sizePixels.x * 0.5f),
        toMeters(sizePixels.y * 0.5f)
    );

    b2ShapeDef shapeDef = b2DefaultShapeDef();
    if (bodyType == b2_dynamicBody) {
        shapeDef.density = 1.0f;
    }
    shapeDef.material.friction = 0.0f;
    b2CreatePolygonShape(bodyId, &shapeDef, &polygon);

    const bool dynamicLike = bodyType != b2_staticBody;
    debugBodies.push_back({
        bodyId,
        sizePixels,
        dynamicLike ? glm::vec4{1.0f, 0.3f, 0.2f, 0.28f}
                    : glm::vec4{0.2f, 0.8f, 1.0f, 0.22f},
        dynamicLike
    });

    return bodyId;
}

b2BodyId PhysicsWorld2D::createStaticBox(float centerX, float centerY, float halfWidth, float halfHeight) {
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_staticBody;
    bodyDef.position = {toMeters(centerX), toMeters(centerY)};

    b2BodyId body = b2CreateBody(worldId, &bodyDef);

    b2Polygon polygon = b2MakeBox(toMeters(halfWidth), toMeters(halfHeight));
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    b2CreatePolygonShape(body, &shapeDef, &polygon);

    staticBodies.push_back(body);
    debugBodies.push_back({
        body,
        {halfWidth * 2.0f, halfHeight * 2.0f},
        {0.2f, 0.8f, 1.0f, 0.22f},
        false
    });

    return body;
}

void PhysicsWorld2D::buildStaticCollisionFromMap(const TileMapData &map) {
    const float tileWidth = static_cast<float>(map.tileWidth);
    const float tileHeight = static_cast<float>(map.tileHeight);
    
    // First check for collidable leayers
    for (const auto& layer : map.layers) {
        if (!layer.collidable)
            continue;
    
        for (int y = 0; y < layer.height; ++y) {
            for (int x = 0; x < layer.width; ++x) {
                if (layer.getTileId(x, y) == 0)
                    continue;
                const float centerX = x * tileWidth + tileWidth * 0.5f;
                const float centerY = (layer.height - 1 - y) * tileHeight + tileHeight * 0.5f;
                createStaticBox(centerX, centerY, tileWidth * 0.5f, tileHeight * 0.5f);
            }
        }
    }

    // Then check for rectangles defined in object layers
    for (const auto& objectLayer : map.objectLayers) {
        if (!objectLayer.collidable)
            continue;
        
        for (const auto& object : objectLayer.objects) {
            if (object.shape != MapObjectShape::Rectangle)
                continue;
            
            const float centerX = object.x + object.width * 0.5f;
            // Tiled object Y is from top; convert to world bottom-up space
            const float centerY = (map.height * tileHeight - object.y) - object.height * 0.5f;
            createStaticBox(centerX, centerY, object.width * 0.5f, object.height * 0.5f);
        }
    }
}

b2BodyId PhysicsWorld2D::createDynamicBox(const glm::vec2 &positionPixels, const glm::vec2 &sizePixels) {
    return createBox(positionPixels, sizePixels, b2_dynamicBody);
}

glm::vec2 PhysicsWorld2D::getBodyPositionPixels(b2BodyId bodyId) const {
    b2Vec2 pos = b2Body_GetPosition(bodyId);
    return {toPixels(pos.x), toPixels(pos.y)};
}

void PhysicsWorld2D::setBodyLinearVelocityPixels(b2BodyId bodyId, const glm::vec2 &velocityPixelsPerSecond) {
    b2Body_SetLinearVelocity(bodyId, {
        toMeters(velocityPixelsPerSecond.x),
        toMeters(velocityPixelsPerSecond.y)
    });
}

void PhysicsWorld2D::setBodyPositionPixels(b2BodyId bodyId, const glm::vec2& centrePixels) {
    b2Body_SetTransform(bodyId, {toMeters(centrePixels.x), toMeters(centrePixels.y)}, b2Rot_identity);
}

void PhysicsWorld2D::drawDebug(Renderer& renderer) const {
    for (const auto& debugBody : debugBodies) {
        if (B2_IS_NULL(debugBody.bodyId) || !b2Body_IsValid(debugBody.bodyId))
            continue;

        const glm::vec2 centre = getBodyPositionPixels(debugBody.bodyId);
        Transform2D transform;
        transform.position = centre - debugBody.sizePixels * 0.5f;
        transform.scale = debugBody.sizePixels;

        renderer.drawQuad({
            transform,
            TextureRegion::full(nullptr),
            debugBody.color
        });
    }
}

void PhysicsWorld2D::destroyBody(b2BodyId bodyId) {
    if (B2_IS_NULL(bodyId) || !b2Body_IsValid(bodyId)) {
        return;
    }

    b2DestroyBody(bodyId);

    staticBodies.erase(
        std::remove_if(staticBodies.begin(), staticBodies.end(),
            [bodyId](b2BodyId id) { return B2_ID_EQUALS(id, bodyId); }),
        staticBodies.end()
    );

    debugBodies.erase(
        std::remove_if(debugBodies.begin(), debugBodies.end(),
            [bodyId](const DebugBodyBox& debugBody) { return B2_ID_EQUALS(debugBody.bodyId, bodyId); }),
        debugBodies.end()
    );
}

void PhysicsWorld2D::createWorld() {
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = {0.0f, 0.0f};
    worldId = b2CreateWorld(&worldDef);
}