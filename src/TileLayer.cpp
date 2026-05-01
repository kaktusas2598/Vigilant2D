#include "TileLayer.hpp"

#include "Transform2D.hpp"

void TileLayer::setTile(int x, int y, const TextureRegion& region) {
    Tile& tile = tiles[y * width + x];
    tile.region = region;
    tile.empty = false;

    if (layerTexture == nullptr)
        layerTexture = region.texture;
}

void TileLayer::rebuildVisibleMesh(const Camera2D& camera, int viewportWidth, int viewportHeight) {
    batchVertices.clear();
    batchIndices.clear();

    // Camera culling to save on performance
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

    unsigned int baseIndex = 0;
    for (int y = startY; y < endY; ++y) {
        for (int x = startX; x < endX; ++x) {
            const Tile& tile = tiles[y * width + x];
            if (tile.empty || tile.region.texture == nullptr)
                continue;

            const float x0 = x * tileSize.x;
            const float y0 = y * tileSize.y;
            const float x1 = x0 + tileSize.x;
            const float y1 = y0 + tileSize.y;

            const glm::vec2 uvMin = tile.region.uvMin;
            const glm::vec2 uvMax = tile.region.uvMax;

            batchVertices.push_back({{x0, y0, 0.0f}, {uvMin.x, uvMin.y}});
            batchVertices.push_back({{x1, y0, 0.0f}, {uvMax.x, uvMin.y}});
            batchVertices.push_back({{x1, y1, 0.0f}, {uvMax.x, uvMax.y}});
            batchVertices.push_back({{x0, y1, 0.0f}, {uvMin.x, uvMax.y}});

            batchIndices.push_back(baseIndex + 0);
            batchIndices.push_back(baseIndex + 1);
            batchIndices.push_back(baseIndex + 2);
            batchIndices.push_back(baseIndex + 2);
            batchIndices.push_back(baseIndex + 3);
            batchIndices.push_back(baseIndex + 0);

            baseIndex += 4;
        }
    }

    batchMesh.upload(batchVertices, batchIndices);
}

void TileLayer::draw(Renderer& renderer) const {
    if (layerTexture == nullptr)
        return;

    renderer.drawMesh(batchMesh, layerTexture);
}