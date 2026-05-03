#include "TileCursor.hpp"

void TileCursor::update(const Input &input, const Camera2D &camera, TileMap *map) {
    if (input.isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
        selectedTile = hoveredTile;
    }

    // Update however tile based on mouse
    if (map != nullptr) {
        glm::vec2 mouseScreen(
            static_cast<float>(input.getMouseX()),
            static_cast<float>(input.getMouseY())
        );

        glm::vec2 mouseWorld = camera.screenToWorld(mouseScreen);
        glm::ivec2 tile = map->worldToTile(mouseWorld);

        if (map->isTileInBounds(tile.x, tile.y)) {
            hoveredTile = tile;
        } else {
            hoveredTile = {-1, -1};
        }
    }
}

void TileCursor::draw(Renderer &renderer, TileMap *map) const {
    if (map != nullptr && hoveredTile.x >= 0 && hoveredTile.y >= 0) {
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
}