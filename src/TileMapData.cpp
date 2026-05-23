#include "TileMapData.hpp"

const TilesetData* findTilesetForGid(const TileMapData& map, int gid) {
    const TilesetData* result = nullptr;

    for (const auto& tileset : map.tilesets) {
        if (gid >= tileset.firstGid) {
            result = &tileset;
        } else {
            break;
        }
    }

    return result;
}

TextureRegion makeRegionForGid(const TilesetData& tileset, Texture* texture,  int gid) {
    const int localId = gid - tileset.firstGid;
    const int column = localId % tileset.columns;
    const int row = localId / tileset.columns;

    const float pixelX = static_cast<float>(tileset.margin + column * (tileset.tileWidth + tileset.spacing));
    const float pixelY = static_cast<float>(tileset.margin + row * (tileset.tileHeight + tileset.spacing));

    const glm::vec2 uvMin(
        pixelX / static_cast<float>(tileset.imageWidth),
        1.0f - ((pixelY + tileset.tileHeight) / static_cast<float>(tileset.imageHeight))
    );

    const glm::vec2 uvMax(
        (pixelX + tileset.tileWidth) / static_cast<float>(tileset.imageWidth),
        1.0f - (pixelY / static_cast<float>(tileset.imageHeight))
    );


    return {texture, uvMin, uvMax};
}

bool tryFindPointObjectWorldPosition(const TileMapData& map,
                                     const std::string& objectName,
                                     glm::vec2& outWorldPosition) {
    const float mapPixelHeight = static_cast<float>(map.height * map.tileHeight);

    for (const auto& layer : map.objectLayers) {
        for (const auto& object : layer.objects) {
            if (!object.visible || object.shape != MapObjectShape::Point || object.name != objectName)
                continue;

            outWorldPosition = {object.x, mapPixelHeight - object.y};
            return true;
        }
    }

    return false;
}