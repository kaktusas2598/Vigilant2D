#include "TileLayer.hpp"

#include "Transform2D.hpp"

void TileLayer::setTile(int x, int y, const TextureRegion& region) {
    Tile& tile = tiles[y * width + x];
    tile.region = region;
    tile.empty = false;
}

void TileLayer::draw(
    Renderer &renderer, const Camera2D &camera, int viewportWidth, int viewportHeight) const {

    // Camera culling to save on performance
    // TODO: Use batch rendering
    const float halfWidth = viewportWidth * 0.5f / camera.getZoom();
    const float halfHeight = viewportHeight * 0.5f / camera.getZoom();
    const glm::vec2 cameraPos = camera.getPosition();

    const float left = cameraPos.x - halfWidth;
    const float right = cameraPos.x + halfWidth;
    const float bottom = cameraPos.y - halfHeight;
    const float top = cameraPos.y + halfHeight;

    const int startX = std::max(0, static_cast<int>(std::floor(left / tileSize.x)));
    const int endX =   std::min(width, static_cast<int>(std::ceil(right / tileSize.x)) + 1);
    const int startY = std::max(0, static_cast<int>(std::floor(bottom / tileSize.y)));
    const int endY =   std::min(height, static_cast<int>(std::ceil(top / tileSize.y)) + 1);

    // Actual render part
    // for (int y = 0; y < height; ++y) {
        // for (int x = 0; x < width; ++x) {
    for (int y = startY; y < endY; ++y) {
        for (int x = startX; x < endX; ++x) {

            const Tile& tile = tiles[y * width + x];
            if (tile.empty)
                continue;

            Transform2D transform;
            transform.position = {x * tileSize.x, y * tileSize.y};
            transform.scale = tileSize;

            renderer.drawQuad(tile.region, transform);
        }
    }
}
