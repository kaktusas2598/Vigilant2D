#include "SelectionManager.hpp"

#include "Renderer.hpp"
#include "Input.hpp"
#include "Camera2D.hpp"
#include "Scene.hpp"
#include "Entity.hpp"
#include "TileMap.hpp"
#include "TextureRegion.hpp"
#include "Transform2D.hpp"

void SelectionManager::update(const Input &input, const Camera2D &camera, Scene& scene) {
    hoveredTile = {-1, -1};
    hoveredEntityId.clear();

    const glm::vec2 mouseWorld = getMouseWorld(input, camera);
    if (TileMap* map = scene.getTileMap()) {
        glm::ivec2 tile = map->worldToTile(mouseWorld);
        if (map->isTileInBounds(tile.x, tile.y)) {
            hoveredTile = tile;
        }
    }

    for (const auto& entityPtr : scene.getEntities()) {
        if (entityPtr && pointInEntity(mouseWorld, *entityPtr)) {
            hoveredEntityId = entityPtr->getID();
        }
    }
    
    // Update however tile and entity based on mouse
    if (input.isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
        selectedTile = hoveredTile;
        selectedEntityId = hoveredEntityId;
    }

}

void SelectionManager::draw(Renderer &renderer, Scene& scene) const {
    if (TileMap* map = scene.getTileMap()) {
    // if (map != nullptr && hoveredTile.x >= 0 && hoveredTile.y >= 0) {
        if (hoveredTile.x >= 0 && hoveredTile.y >= 0) {
            Transform2D hoverTransform;
            hoverTransform.position = map->tileToWorld(hoveredTile.x, hoveredTile.y);
            hoverTransform.scale = {
                static_cast<float>(map->getTileWidth()),
                static_cast<float>(map->getTileHeight())
            };

            renderer.drawQuad({
                hoverTransform,
                TextureRegion::full(nullptr),
                {0.2f, 0.9f, 0.3f, 0.35f}
            });
        }

        if (selectedTile.x >= 0 && selectedTile.y >= 0) {
            Transform2D selectTransform;
            selectTransform.position = map->tileToWorld(selectedTile.x, selectedTile.y);
            selectTransform.scale = {
                static_cast<float>(map->getTileWidth()),
                static_cast<float>(map->getTileHeight())};

            renderer.drawQuad({selectTransform,
                               TextureRegion::full(nullptr),
                               {0.8f, 0.1f, 0.3f, 0.35f}});
        }
    }

    if (!selectedEntityId.empty()) {
        Entity* entity = scene.findEntityByID(selectedEntityId);
        if (entity) {
            Transform2D t = entity->transform;
            renderer.drawQuad({
                t,
                TextureRegion::full(nullptr),
                {0.2f, 0.5f, 1.0f, 0.25f}
            });
        }
    }
}

glm::vec2 SelectionManager::getMouseWorld(const Input& input, const Camera2D& camera) const {
    return camera.screenToWorld({
        static_cast<float>(input.getMouseX()),
        static_cast<float>(input.getMouseY())
    });
}

bool SelectionManager::pointInEntity(const glm::vec2& point, const Entity& entity) const {
    const glm::vec2 pos = entity.transform.position;
    const glm::vec2 size = entity.transform.scale;

    return point.x >= pos.x &&
           point.x <= pos.x + size.x &&
           point.y >= pos.y &&
           point.y <= pos.y + size.y;
}
