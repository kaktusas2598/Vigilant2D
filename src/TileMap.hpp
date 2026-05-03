#pragma once

#include <vector>
#include <memory>
#include <string>

#include "TileMapData.hpp"
#include "TileLayer.hpp"
#include "TileLayer.hpp"
#include "Texture.hpp"
#include "Renderer.hpp"
#include "Camera2D.hpp"
#include "AssetManager.hpp"

class TileMap {
    public:
        bool loadFromFile(const std::string& fileName, AssetManager& assets);

        void rebuildVisibleLayers(const Camera2D& camera, int viewportWidth, int viewportHeight);
        void draw(Renderer& renderer) const;
        bool isLoaded() const { return loaded; }

        glm::ivec2 worldToTile(const glm::vec2& worldPosition) const;
        glm::vec2 tileToWorld(int tileX, int tileY) const;
        bool isTileInBounds(int tileX, int tileY) const;

        int getTileWidth() const { return mapData.tileWidth; }
        int getTileHeight() const { return mapData.tileHeight; }
    private:
        std::unique_ptr<TileLayer> buildTileLayer(const TileLayerData& layerData);
        Texture* resolveTextureForTileset(const TilesetData* tileset);

        bool loaded = false;
        TileMapData mapData;
        std::vector<Texture*> tilesetTextures;
        std::vector<std::unique_ptr<TileLayer>> layers;

};