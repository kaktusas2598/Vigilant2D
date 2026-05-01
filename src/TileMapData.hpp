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
