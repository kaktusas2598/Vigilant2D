#include "TileRuntime.hpp"
#include "TileMapData.hpp"

void TileRuntime::initFromMapData(const TileMapData& mapData) {
    overrideLayers.clear();

    for (const auto& layer : mapData.layers) {
        TileVisualOverrideLayer overrides;
        overrides.init(layer.width, layer.height);
        overrideLayers.emplace(layer.name, std::move(overrides));
    }
}

void TileRuntime::clear() {
    overrideLayers.clear();
}

TileVisualOverrideLayer* TileRuntime::getOverrideLayer(const std::string& layerName) {
    auto it = overrideLayers.find(layerName);
    return it != overrideLayers.end() ? &it->second : nullptr;
}

const TileVisualOverrideLayer* TileRuntime::getOverrideLayer(const std::string& layerName) const {
    auto it = overrideLayers.find(layerName);
    return it != overrideLayers.end() ? &it->second : nullptr;
}
