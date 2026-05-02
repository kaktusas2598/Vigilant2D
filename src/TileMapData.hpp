#pragma once

#include <string>
#include <vector>

#include "TextureRegion.hpp"

struct TilesetData {
    int firstGid = 0;
    int tileWidth = 0;
    int tileHeight = 0;
    int spacing = 0;
    int margin = 0;
    int imageWidth = 0;
    int imageHeight = 0;
    int columns = 0;

    std::string name;
    std::string imagePath;
};

struct TileLayerData {
    std::string name;
    int width = 0;
    int height = 0;
    bool visible = true;
    bool collidable = false;

    std::vector<int> tileIds;

    int getTileId(int x, int y) const {
        return tileIds[y * width + x];
    }
};

struct TileMapData {
    int tileWidth = 0;
    int tileHeight = 0;
    int width = 0;
    int height = 0;

    std::vector<TilesetData> tilesets;
    std::vector<TileLayerData> layers;
};

// TODO: Don't like these global methods just sittin there below data definitions
const TilesetData* findTilesetForGid(const TileMapData& map, int gid);
TextureRegion makeRegionForGid(const TilesetData& tileset, Texture* texture,  int gid);
inline TextureRegion makeRegionFromGrid(
    Texture* texture,
    int frameX,
    int frameY,
    int columns,
    int rows
) {
    const float frameWidth = 1.0f / static_cast<float>(columns);
    const float frameHeight = 1.0f / static_cast<float>(rows);

    const glm::vec2 uvMin(
        frameX * frameWidth,
        1.0f - ((frameY + 1) * frameHeight)
    );

    const glm::vec2 uvMax(
        (frameX + 1) * frameWidth,
        1.0f - (frameY * frameHeight)
    );

    return {texture, uvMin, uvMax};
}
