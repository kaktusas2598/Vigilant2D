#include "PhysicsWorld2D.hpp"

#include "TileMapData.hpp"

PhysicsWorld2D::PhysicsWorld2D() {
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = {0.0f, 0.0f};
    worldId = b2CreateWorld(&worldDef);
}

void PhysicsWorld2D::step(float dt) {
    const int subSteps = 4;
    b2World_Step(worldId, dt, subSteps);
}

b2BodyId PhysicsWorld2D::createStaticBox(float centerX, float centerY, float halfWidth, float halfHeight) {
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_staticBody;
    bodyDef.position = {centerX, centerY};

    b2BodyId body = b2CreateBody(worldId, &bodyDef);

    b2Polygon polygon = b2MakeBox(halfWidth, halfHeight);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    b2CreatePolygonShape(body, &shapeDef, &polygon);

    staticBodies.push_back(body);
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
