#pragma once

#include <vector>
#include "Renderer.hpp"
#include "TextureRegion.hpp"

struct Tile {
    TextureRegion region;
    bool empty = true;
};

class TileLayer {
    public:
        TileLayer(int width, int height, glm::vec2 tileSize)
            : width(width), height(height), tileSize(tileSize), tiles(width * height) {}

        void setTile(int x, int y, const TextureRegion& region);

        // TODO: Later might want to refactor this to the TileMapRenderer or sth, 
        // not sure yet, will need batching support at the very least for performance!!!
        void draw(Renderer& renderer, const Camera2D& camera, int viewportWidth, int viewportHeight) const;

    private:
        int width;
        int height;
        glm::vec2 tileSize;
        // TODO: might want to store gid instead of region in Tile and resolve regions during draw?
        std::vector<Tile> tiles;
};
