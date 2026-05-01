#pragma once

#include <vector>

#include "Renderer.hpp"
#include "TextureRegion.hpp"
#include "Mesh.hpp"

struct Tile {
    TextureRegion region;
    bool empty = true;
};

class TileLayer {
    public:
        TileLayer(int width, int height, glm::vec2 tileSize)
            : width(width), height(height), tileSize(tileSize), tiles(width * height) {

                // Witch batching
                const size_t maxVisibleTiles = 8192;
                batchMesh.initDynamic(maxVisibleTiles * 4, maxVisibleTiles * 6);
                batchVertices.reserve(maxVisibleTiles * 4);
                batchIndices.reserve(maxVisibleTiles * 6);
            }

        void setTile(int x, int y, const TextureRegion& region);

        // TODO: Later might want to refactor this to the TileMapRenderer or sth
        void rebuildVisibleMesh(const Camera2D& camera, int viewportWidth, int viewportHeight);
        void draw(Renderer& renderer) const;

    private:
        int width;
        int height;
        glm::vec2 tileSize;
        // TODO: might want to store gid instead of region in Tile and resolve regions during draw?
        std::vector<Tile> tiles;

        Texture* layerTexture = nullptr;
        Mesh batchMesh;
        std::vector<Vertex> batchVertices;
        std::vector<unsigned int> batchIndices;
};
