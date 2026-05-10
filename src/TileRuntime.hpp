#pragma once

#include <string>
#include <unordered_map>
#include "TileVisualOverrideLayer.hpp"

class TileMapData;

// Owned by TileMap, contains runtime visual override for map layers
class TileRuntime {
    public:
        void initFromMapData(const TileMapData& mapData);
        void clear();

        TileVisualOverrideLayer* getOverrideLayer(const std::string& layerName);
        const TileVisualOverrideLayer* getOverrideLayer(const std::string& layerName) const;

    private:
        std::unordered_map<std::string, TileVisualOverrideLayer> overrideLayers;
};