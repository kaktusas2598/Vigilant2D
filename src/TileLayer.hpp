#pragma once

#include <vector>

#include "Renderer.hpp"
#include "TextureRegion.hpp"
#include "Mesh.hpp"

struct Tile {
    TextureRegion region;
    bool empty = true;
};

struct TileRenderBatch {
    Texture* texture = nullptr;
    Mesh mesh;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};

class TileLayer {
    public:
        TileLayer(int width, int height, glm::vec2 tileSize, bool visible = true)
            : width(width), height(height), tileSize(tileSize), tiles(width * height), visible(visible) {
                // Reduce realocations and allow early 8 textures per layer
                batches.reserve(8);
            }

        void setTile(int x, int y, const TextureRegion& region);
        bool isVisible() const { return visible; }

        // TODO: Later might want to refactor this to the TileMapRenderer or sth
        void rebuildVisibleMesh(const Camera2D& camera, int viewportWidth, int viewportHeight);
        void draw(Renderer& renderer) const;

        TileRenderBatch* findOrCreateBatch(Texture* texture);

    private:
        int width;
        int height;
        glm::vec2 tileSize;
        // TODO: might want to store gid instead of region in Tile and resolve regions during draw?
        std::vector<Tile> tiles;

        // Render batches for each texture
        std::vector<TileRenderBatch> batches;

        // TODO: is it good idea to store it here as well because it is already stored in TIleLayerData
        bool visible = true;
};
