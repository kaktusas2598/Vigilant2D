#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "TextureRegion.hpp"

struct AnimatedTileFrameData {
    int tileId = 0; // Local tileset tile id
    int durationMs = 0;
};

struct AnimatedTileDefinition {
    std::vector<AnimatedTileFrameData> frames;
};

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
    std::unordered_map<int, AnimatedTileDefinition> animatedTiles;
};

struct TileLayerData {
    std::string name;
    int width = 0;
    int height = 0;
    bool visible = true;
    bool collidable = false;
    bool drawAboveEntities = false;

    std::vector<int> tileIds;

    int getTileId(int x, int y) const {
        return tileIds[y * width + x];
    }
};

// Stuff to do with Object layer
enum class MapObjectShape {
    Point,
    Rectangle
};

struct MapObjectData {
    std::string name;
    std::string type;

    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;

    bool visible = true;
    bool collidable = false;

    MapObjectShape shape = MapObjectShape::Rectangle;
    std::unordered_map<std::string, std::string> properties;

    const std::string* findProperty(const std::string& key) const {
        auto it = properties.find(key);
        if (it == properties.end())
            return nullptr;
        return &it->second;
    }
};

struct ObjectLayerData {
    std::string name;
    bool visible = true;
    bool collidable = false;

    std::vector<MapObjectData> objects;
};

struct TileMapData {
    int tileWidth = 0;
    int tileHeight = 0;
    int width = 0;
    int height = 0;

    std::vector<TilesetData> tilesets;
    std::vector<TileLayerData> layers;
    std::vector<ObjectLayerData> objectLayers;
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

inline TextureRegion makeRegionFromPixels(
    Texture* texture,
    int x,
    int y,
    int width,
    int height
) {
    if (texture == nullptr || width <= 0 || height <= 0) {
        return TextureRegion::full(nullptr);
    }

    const float textureWidth = static_cast<float>(texture->getWidth());
    const float textureHeight = static_cast<float>(texture->getHeight());

    const glm::vec2 uvMin(
        static_cast<float>(x) / textureWidth,
        1.0f - (static_cast<float>(y + height) / textureHeight)
    );

    const glm::vec2 uvMax(
        static_cast<float>(x + width) / textureWidth,
        1.0f - (static_cast<float>(y) / textureHeight)
    );

    return {texture, uvMin, uvMax};
}