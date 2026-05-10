#include "TileLayer.hpp"

#include "Transform2D.hpp"

void TileLayer::setTile(int x, int y, const TextureRegion& region) {
    Tile& tile = tiles[y * width + x];
    tile.region = region;
    tile.empty = false;
}

void TileLayer::rebuildVisibleMesh(const Camera2D& camera,
     int viewportWidth, int viewportHeight, const TileVisualOverrideLayer* overrides) {
    if (!visible)
        return;

    for (auto& batch : batches) {
        batch.vertices.clear();
        batch.indices.clear();
    }

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

    for (int y = startY; y < endY; ++y) {
        for (int x = startX; x < endX; ++x) {
            const Tile& tile = tiles[y * width + x];

            TextureRegion region = tile.region;
            bool empty = tile.empty;

            // Check if tile is overriden and needs to be rendererd differently            
            if (overrides != nullptr) {
                const TileVisual* visual = overrides->tryGet(x, y);
                if (visual != nullptr && visual->isSet()) {
                    if (visual->kind == TileVisualKind::StaticRegion) {
                        region = visual->region;
                        empty = (region.texture == nullptr);
                    }
                }
            }

            if (empty || region.texture == nullptr)
                continue;

            TileRenderBatch* batch = findOrCreateBatch(region.texture);
            const unsigned int baseIndex = static_cast<unsigned int>(batch->vertices.size());

            const float x0 = x * tileSize.x;
            const float y0 = y * tileSize.y;
            const float x1 = x0 + tileSize.x;
            const float y1 = y0 + tileSize.y;

            const glm::vec2 uvMin = region.uvMin;
            const glm::vec2 uvMax = region.uvMax;

            batch->vertices.push_back({{x0, y0, 0.0f}, {uvMin.x, uvMin.y}});
            batch->vertices.push_back({{x1, y0, 0.0f}, {uvMax.x, uvMin.y}});
            batch->vertices.push_back({{x1, y1, 0.0f}, {uvMax.x, uvMax.y}});
            batch->vertices.push_back({{x0, y1, 0.0f}, {uvMin.x, uvMax.y}});

            batch->indices.push_back(baseIndex + 0);
            batch->indices.push_back(baseIndex + 1);
            batch->indices.push_back(baseIndex + 2);
            batch->indices.push_back(baseIndex + 2);
            batch->indices.push_back(baseIndex + 3);
            batch->indices.push_back(baseIndex + 0);
        }
    }

    for (auto& batch : batches) {
        batch.mesh.upload(batch.vertices, batch.indices);
    }
}

void TileLayer::draw(Renderer& renderer) const {
    if (!visible)
        return;

    for (const auto& batch : batches) {
        if (batch.texture == nullptr)
            continue;
        renderer.drawMesh(batch.mesh, batch.texture);
    }
}

TileRenderBatch* TileLayer::findOrCreateBatch(Texture *texture) {
    for (auto& batch : batches) {
        if (batch.texture == texture) {
            return &batch;
        }
    }

    batches.push_back({});
    TileRenderBatch& batch = batches.back();
    batch.texture = texture;

    const size_t maxVisibleTiles = 8192;
    batch.mesh.initDynamic(maxVisibleTiles * 4, maxVisibleTiles * 6);
    batch.vertices.reserve(maxVisibleTiles * 4);
    batch.indices.reserve(maxVisibleTiles * 6);
    
    return &batch;
}