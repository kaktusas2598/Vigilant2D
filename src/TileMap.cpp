#include "TileMap.hpp"

#include "TiledMapLoader.hpp"

bool TileMap::loadFromFile(const std::string &fileName, AssetManager& assets) {
    TiledMapLoader loader;
    mapData = loader.loadFromFile(fileName);

    tilesetTextures.clear();
    layers.clear();

    for (const auto &tileset : mapData.tilesets) {
        Texture* texture = assets.loadTexture(tileset.name, tileset.imagePath);
        tilesetTextures.push_back(texture);
    }

    for (const auto &layerData : mapData.layers) {
        layers.push_back(buildTileLayer(layerData));
    }

    runtime.initFromMapData(mapData);
    rebuildAnimatedTileInstances();
    sourcePath = fileName;
    loaded = true;
    return true;
}


void TileMap::rebuildVisibleLayers(const Camera2D &camera, int viewportWidth, int viewportHeight) {
    if (!loaded)
        return;
    
    for (size_t i = 0; i < layers.size(); ++i) {
        const std::string& layerName = mapData.layers[i].name;
        const TileVisualOverrideLayer* overrides = runtime.getOverrideLayer(layerName);

        layers[i]->rebuildVisibleMesh(camera, viewportWidth, viewportHeight, overrides);
    }
}

void TileMap::drawForegroundLayers(Renderer &renderer) const {
    if (!loaded)
        return;
 
    for (const auto& layer : layers) {
        if (layer->shouldDrawAboveEntities()) {
        layer->draw(renderer);
        }
    }
}

void TileMap::drawBackgroundLayers(Renderer &renderer) const {
    if (!loaded)
        return;

    for (const auto& layer : layers) {
        if (!layer->shouldDrawAboveEntities()) {
            layer->draw(renderer);
        }
    }
}

void TileMap::update(float dt) {
    if (!loaded)
        return;

    for (auto& instance : animatedTiles) {
        if (instance.tileset == nullptr || instance.frames.empty())
            continue;
        
        instance.elapsedMs += dt * 1000.0f;
        const int frameDurationMs = instance.frames[instance.currentFrame].durationMs;
        if (frameDurationMs > 0 && instance.elapsedMs >= static_cast<float>(frameDurationMs)) {
            instance.elapsedMs -= static_cast<float>(frameDurationMs);
            instance.currentFrame = (instance.currentFrame + 1) % static_cast<int>(instance.frames.size());
        }   

        applyAnimatedTileFrame(instance);
    }
} 

void TileMap::rebuildAnimatedTileInstances() {
    animatedTiles.clear();

    for (const auto& layerData : mapData.layers) {
        for (int y = 0; y < layerData.height; ++y) {
            for (int x = 0; x < layerData.width; ++x) {
                const int gid = layerData.getTileId(x, y);
                if (gid == 0)
                    continue;

                const TilesetData* tileset = findTilesetForGid(mapData, gid);
                if (tileset == nullptr)
                    continue;

                const int localTileId = gid - tileset->firstGid;
                auto animIt = tileset->animatedTiles.find(localTileId);
                if (animIt == tileset->animatedTiles.end())
                    continue;

                AnimatedTileInstance instance;
                instance.layerName = layerData.name;
                instance.tileX = x;
                instance.tileY = layerData.height - 1 - y; // Flip Y to match rendering
                instance.tileset = tileset;
                instance.frames = animIt->second.frames;
                instance.currentFrame = 0;
                instance.elapsedMs = 0.0f;

                applyAnimatedTileFrame(instance);
                animatedTiles.push_back(std::move(instance));
            }
        }
    }
}

void TileMap::applyAnimatedTileFrame(const AnimatedTileInstance& instance) {
    if (instance.tileset == nullptr || instance.frames.empty())
        return;
    
    const int frameTileId = instance.frames[instance.currentFrame].tileId;
    const int frameGid = instance.tileset->firstGid + frameTileId;

    TextureRegion region;
    if (!tryMakeRegionForGid(frameGid, region))
        return;
    
    TileVisualOverrideLayer* overrides = runtime.getOverrideLayer(instance.layerName);
    if (overrides != nullptr)
        overrides->set(instance.tileX, instance.tileY, TileVisual::fromRegion(region));
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
        glm::vec2(static_cast<float>(mapData.tileWidth), static_cast<float>(mapData.tileHeight)),
        layerData.visible,
        layerData.drawAboveEntities
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

bool TileMap::tryMakeRegionForGid(int gid, TextureRegion& outRegion) const {
    const TilesetData* tileset = findTilesetForGid(mapData, gid);
    if (tileset == nullptr)
        return false;

    Texture* texture = nullptr;
    for (size_t i = 0; i < mapData.tilesets.size(); ++i) {
        if (&mapData.tilesets[i] == tileset) {
            texture = tilesetTextures[i];
            break;
        }
    }

    if (texture == nullptr)
        return false;

    outRegion = makeRegionForGid(*tileset, texture, gid);
    return true;
}

const TilesetData* TileMap::findTilesetByName(const std::string& name) const {
    for (const auto& tileset : mapData.tilesets) {
        if (tileset.name == name)
            return &tileset;
    }
    return nullptr;
}

bool TileMap::tryMakeRegionForTilesetTileId(const std::string& tilesetName,
                                            int localTileId,
                                            TextureRegion& outRegion) const {
    const TilesetData* tileset = findTilesetByName(tilesetName);
    if (tileset == nullptr)
        return false;

    const int gid = tileset->firstGid + localTileId;
    return tryMakeRegionForGid(gid, outRegion);
}
