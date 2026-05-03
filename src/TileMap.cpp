#include "TileMap.hpp"

#include "TiledMapLoader.hpp"

TileMap::~TileMap() {
    for (Texture* texture : tilesetTextures) {
        delete texture;
    }
}

bool TileMap::loadFromFile(const std::string &fileName) {
    TiledMapLoader loader;
    mapData = loader.loadFromFile(fileName);

    tilesetTextures.clear();
    layers.clear();

    for (const auto &tileset : mapData.tilesets) {
        auto texture = new Texture();
        texture->load2D(tileset.imagePath);
        tilesetTextures.push_back(texture);
    }

    for (const auto &layerData : mapData.layers) {
        layers.push_back(buildTileLayer(layerData));
    }

    loaded = true;
    return true;
}


void TileMap::rebuildVisibleLayers(const Camera2D &camera, int viewportWidth, int viewportHeight) {
    if (!loaded)
        return;
    
    for (const auto& layer : layers) {
        layer->rebuildVisibleMesh(camera, viewportWidth, viewportHeight);
    }
}

void TileMap::draw(Renderer &renderer) const {
    if (!loaded)
        return;
 
    for (const auto& layer : layers) {
        layer->draw(renderer);
    }
}

glm::ivec2 TileMap::worldToTile(const glm::vec2 &worldPosition) const {
    return {
        static_cast<int>(std::floor(worldPosition.x / static_cast<float>(mapData.tileWidth))),
        static_cast<int>(std::floor(worldPosition.y / static_cast<float>(mapData.tileHeight)))
    };
}

glm::vec2 TileMap::tileToWorld(int tileX, int tileY) const {
    return {
        tileX * static_cast<float>(mapData.tileWidth),
        tileY * static_cast<float>(mapData.tileHeight)
    };
}

bool TileMap::isTileInBounds(int tileX, int tileY) const {
    return tileX >= 0 &&
           tileY >= 0 &&
           tileX < mapData.width &&
           tileY < mapData.height;
}

std::unique_ptr<TileLayer> TileMap::buildTileLayer(const TileLayerData &layerData) {
    auto layer = std::make_unique<TileLayer>(
        layerData.width,
        layerData.height,
        glm::vec2(static_cast<float>(mapData.tileWidth), static_cast<float>(mapData.tileHeight))
    );

    for (int y = 0; y < layerData.height; ++y) {
        for (int x = 0; x < layerData.width; ++x) {
            const int gid = layerData.getTileId(x, y);
            if (gid == 0)
                continue;
            
            const TilesetData* tileset = findTilesetForGid(mapData, gid);
            if (tileset == nullptr)
                continue;
            
            Texture* texture = resolveTextureForTileset(tileset);
            if (texture == nullptr)
                continue;

            const int flippedY = layerData.height - 1 - y;
            layer->setTile(x, flippedY, makeRegionForGid(*tileset, texture, gid));
        }
    }

    return layer;
}

Texture* TileMap::resolveTextureForTileset(const TilesetData *tileset) {
    if (tileset == nullptr)
        return nullptr;

    for (size_t i = 0; i < mapData.tilesets.size(); ++i) {
        if (&mapData.tilesets[i] == tileset) {
            return tilesetTextures[i];
        }
    }

    return nullptr;
}
