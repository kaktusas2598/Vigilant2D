#pragma once

#include <vector>
#include <memory>
#include <string>

#include "TileMapData.hpp"
#include "TileLayer.hpp"
#include "TileLayer.hpp"
#include "TileRuntime.hpp"
#include "Texture.hpp"
#include "Renderer.hpp"
#include "Camera2D.hpp"
#include "AssetManager.hpp"

class TileMap {
    public:
        bool loadFromFile(const std::string& fileName, AssetManager& assets);

        void rebuildVisibleLayers(const Camera2D& camera, int viewportWidth, int viewportHeight);
        void drawBackgroundLayers(Renderer& renderer) const;
        void drawForegroundLayers(Renderer& renderer) const;
        void update(float dt);
        bool isLoaded() const { return loaded; }
        const std::string &getSourcePath() const { return sourcePath; }

        glm::ivec2 worldToTile(const glm::vec2& worldPosition) const;
        glm::vec2 tileToWorld(int tileX, int tileY) const;
        bool isTileInBounds(int tileX, int tileY) const;

        int getTileWidth() const { return mapData.tileWidth; }
        int getTileHeight() const { return mapData.tileHeight; }

        const TileMapData& getData() const { return mapData; }
        const std::vector<ObjectLayerData>& getObjectLayers() const { return mapData.objectLayers; }

        TileRuntime& getRuntime() { return runtime; }
        const TileRuntime& getRuntime() const { return runtime; }

        bool tryMakeRegionForGid(int gid, TextureRegion& outRegion) const;
        bool tryMakeRegionForTilesetTileId(const std::string& tilesetName, int localTileId, TextureRegion& outRegion) const;
    private:
        std::unique_ptr<TileLayer> buildTileLayer(const TileLayerData& layerData);
        Texture* resolveTextureForTileset(const TilesetData* tileset);
        const TilesetData* findTilesetByName(const std::string& name) const;

        std::string sourcePath;
        bool loaded = false;
        TileMapData mapData;
        TileRuntime runtime;
        std::vector<Texture*> tilesetTextures;
        std::vector<std::unique_ptr<TileLayer>> layers;

        struct AnimatedTileInstance {
            std::string layerName;
            int tileX = 0;
            int tileY = 0;
            const TilesetData* tileset = nullptr;
            std::vector<AnimatedTileFrameData> frames;
            int currentFrame = 0;
            float elapsedMs = 0.0f;
        };

        void rebuildAnimatedTileInstances();
        void applyAnimatedTileFrame(const AnimatedTileInstance& instance);
        std::vector<AnimatedTileInstance> animatedTiles;
};