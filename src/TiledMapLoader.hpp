#pragma once

#include <string>
#include <vector>
#include "tinyxml2/tinyxml2.h"

#include "TileMapData.hpp"

// Renderer agnostic Tiled 2D (.TMX) map loader
// Only responsible for loading Tiled2D maps with external tilesets into
// render agnostic data definition of TileMap data defined in TileMapData.hpp
class TiledMapLoader {
    public:
        TileMapData loadFromFile(const std::string& fileName);

    private:
        void parseMapAttributes(tinyxml2::XMLElement* mapElement, TileMapData& map);
        void parseTilesetReference(tinyxml2::XMLElement *tilesetElement,
                                const std::string &mapDirectory,
                                TileMapData &map);
        void parseExternalTileset(const std::string &tsxPath,
                                int firstGid,
                                TileMapData &map);
        void parseTileLayer(tinyxml2::XMLElement *layerElement, TileMapData &map);

        std::vector<int> parseCsvTileData(const std::string &csv,
                                        int expectedWidth,
                                        int expectedHeight) const;

        bool parseLayerPropertyBool(tinyxml2::XMLElement *layerElement,
                                    const char *propertyName,
                                    bool defaultValue) const;

        // TODO: move these to FileUtils as a static helpers??
        std::string getDirectory(const std::string &filePath) const;
        std::string joinPath(const std::string &baseDir, const std::string &relativePath) const;
};
