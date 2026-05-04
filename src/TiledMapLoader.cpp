#include "TiledMapLoader.hpp"

#include <sstream>
#include <stdexcept>

TileMapData TiledMapLoader::loadFromFile(const std::string& fileName) {
    tinyxml2::XMLDocument document;
    if (document.LoadFile(fileName.c_str()) != tinyxml2::XML_SUCCESS) {
        throw std::runtime_error("Failed to load TMX file: " + fileName);
    }

    tinyxml2::XMLElement* mapElement = document.FirstChildElement("map");
    if (mapElement == nullptr) {
        throw std::runtime_error("TMX missint <map> attrivute: " + fileName);
    }

    TileMapData map;
    parseMapAttributes(mapElement, map);

    const std::string mapDirectory = getDirectory(fileName);

    for (tinyxml2::XMLElement* element = mapElement->FirstChildElement();
         element != nullptr; element = element->NextSiblingElement()) {
        const std::string elementName = element->Name();

        if (elementName == "tileset") {
            parseTilesetReference(element, mapDirectory, map);
        } else if (elementName == "layer") {
            parseTileLayer(element, map);
        } else if (elementName == "objectgroup") {
            parseObjectLayer(element, map);
        }
    }

    return map;
}

void TiledMapLoader::parseMapAttributes(tinyxml2::XMLElement *mapElement, TileMapData &map) {
    mapElement->QueryIntAttribute("width", &map.width);
    mapElement->QueryIntAttribute("height", &map.height);
    mapElement->QueryIntAttribute("tilewidth", &map.tileWidth);
    mapElement->QueryIntAttribute("tileheight", &map.tileHeight);
}

void TiledMapLoader::parseTilesetReference(tinyxml2::XMLElement *tilesetElement,
                           const std::string &mapDirectory,
                           TileMapData &map) {
    int firstGid = 0;
    tilesetElement->QueryIntAttribute("firstgid", &firstGid);

    const char* source = tilesetElement->Attribute("source");
    if (source != nullptr) {
        parseExternalTileset(joinPath(mapDirectory, source), firstGid, map);
        return;
    }

    // TODO: Later implement embedded tileset support as well
    throw std::runtime_error("Embedded tilesets aren't supported yet.");
}

void TiledMapLoader::parseExternalTileset(const std::string &tsxPath,
                          int firstGid,
                          TileMapData &map) {
    tinyxml2::XMLDocument document;
    if (document.LoadFile(tsxPath.c_str()) != tinyxml2::XML_SUCCESS) {
        throw std::runtime_error("Failed to load TSX file: " + tsxPath);
    }

    tinyxml2::XMLElement* tilesetElement = document.FirstChildElement("tileset");
    if (tilesetElement == nullptr) {
        throw std::runtime_error("TSX missing <tileset> attrivute: " + tsxPath);
    }

    TilesetData tileset;
    tileset.firstGid = firstGid;
    tilesetElement->QueryIntAttribute("tilewidth", &tileset.tileWidth);
    tilesetElement->QueryIntAttribute("tileheight", &tileset.tileHeight);
    tilesetElement->QueryIntAttribute("spacing", &tileset.spacing);
    tilesetElement->QueryIntAttribute("margin", &tileset.margin);
    tilesetElement->QueryIntAttribute("columns", &tileset.columns);

    if (const char* name = tilesetElement->Attribute("name")) {
        tileset.name = name;
    }

    tinyxml2::XMLElement* imageElement = tilesetElement->FirstChildElement("image");
    if (imageElement == nullptr) {
        throw std::runtime_error("TSX missing <image>: " + tsxPath);
    }

    if (const char* imageSource = imageElement->Attribute("source")) {
        tileset.imagePath = joinPath(getDirectory(tsxPath), imageSource);
    }
    imageElement->QueryIntAttribute("width", &tileset.imageWidth);
    imageElement->QueryIntAttribute("height", &tileset.imageHeight);

    // What is this???
    if (tileset.columns == 0 && tileset.tileWidth > 0) {
        const int stride = tileset.tileWidth + tileset.spacing;
        if (stride > 0) {
            tileset.columns = (tileset.imageWidth - tileset.margin * 2 + tileset.spacing) / stride;
        }
    }

    map.tilesets.push_back(tileset);
}

void TiledMapLoader::parseTileLayer(tinyxml2::XMLElement *layerElement, TileMapData &map) {
    TileLayerData layer;

    if (const char* name = layerElement->Attribute("name")) {
        layer.name = name;
    }

    layerElement->QueryIntAttribute("width", &layer.width);
    layerElement->QueryIntAttribute("height", &layer.height);

    int visible = 1;
    layerElement->QueryIntAttribute("visible", &visible);
    layer.visible = (visible != 0);

    layer.collidable = parseLayerPropertyBool(layerElement, "collidable", false);

    tinyxml2::XMLElement* dataElement = layerElement->FirstChildElement("data");
    if (dataElement == nullptr) {
        throw std::runtime_error("Layer missing <data>: " + layer.name);
    }

    const char* encoding = dataElement->Attribute("encoding");
    if (encoding == nullptr || std::string(encoding) != "csv") {
        throw std::runtime_error("Only CSV tile data is supported in TMX files at the moment.");
    }

    const char* csvText = dataElement->GetText();
    if (csvText == nullptr) {
        throw std::runtime_error("Layer CSV is empty: " + layer.name);
    }

    layer.tileIds = parseCsvTileData(csvText, layer.width, layer.height);
    map.layers.push_back(std::move(layer));
}

void TiledMapLoader::parseObjectLayer(tinyxml2::XMLElement* objectGroupElement, TileMapData& map) {
    ObjectLayerData layer;

    if (const char* name = objectGroupElement->Attribute("name")) {
        layer.name = name;
    }

    int visible = 1;
    objectGroupElement->QueryIntAttribute("visible", &visible);
    layer.visible = (visible != 0);

    layer.collidable = parseLayerPropertyBool(objectGroupElement, "collidable", false);

    for (tinyxml2::XMLElement* objectElement = objectGroupElement->FirstChildElement("object");
         objectElement != nullptr;
         objectElement = objectElement->NextSiblingElement("object")) {
        MapObjectData object;

        if (const char* name = objectElement->Attribute("name")) {
            object.name = name;
        }

        if (const char* type = objectElement->Attribute("type")) {
            object.type = type;
        }

        int visibleObject = 1;
        objectElement->QueryIntAttribute("visible", &visibleObject);
        object.visible = (visibleObject != 0);

        objectElement->QueryFloatAttribute("x", &object.x);
        objectElement->QueryFloatAttribute("y", &object.y);
        objectElement->QueryFloatAttribute("width", &object.width);
        objectElement->QueryFloatAttribute("height", &object.height);

        object.collidable = layer.collidable;

        tinyxml2::XMLElement* pointElement = objectElement->FirstChildElement("point");
        if (pointElement != nullptr || (object.width == 0.0f && object.height == 0.0f)) {
            object.shape = MapObjectShape::Point;
        } else {
            object.shape = MapObjectShape::Rectangle;
        }

        layer.objects.push_back(std::move(object));
    }

    map.objectLayers.push_back(std::move(layer));
}

std::vector<int> TiledMapLoader::parseCsvTileData(const std::string &csv,
                                  int expectedWidth,
                                  int expectedHeight) const {
    std::vector<int> tileIds;
    tileIds.reserve(expectedHeight * expectedWidth);

    std::stringstream csvStream(csv);
    std::string token;

    while (std::getline(csvStream, token, ',')) {
        std::stringstream valueStream(token);
        int gid = 0;
        valueStream >> gid;
        tileIds.push_back(gid);
    }

    if (static_cast<int>(tileIds.size()) != expectedHeight * expectedWidth) {
        throw std::runtime_error("CSV tile count does not match with layer dimensions.");
    }

    return tileIds;
}

bool TiledMapLoader::parseLayerPropertyBool(tinyxml2::XMLElement *layerElement,
                            const char *propertyName,
                            bool defaultValue) const {
    tinyxml2::XMLElement* propertiesElement = layerElement->FirstChildElement("properties");
    if (propertiesElement == nullptr) {
        return defaultValue;
    }

    for (tinyxml2::XMLElement* property = propertiesElement->FirstChildElement("property");
         property != nullptr; property = property->NextSiblingElement("property")) {
        const char* name = property->Attribute("name");
        if (name == nullptr || std::string(name) != propertyName) {
            continue;
        }

        bool value = defaultValue;
        property->QueryBoolAttribute("value", &value);
        return value;
    }

    return defaultValue;
}

std::string TiledMapLoader::getDirectory(const std::string& filePath) const {
    const size_t slash = filePath.find_last_of("/\\");
    if (slash == std::string::npos) {
        return ".";
    }
    return filePath.substr(0, slash);
}

std::string TiledMapLoader::joinPath(const std::string& baseDir,
                                     const std::string& relativePath) const {
    if (baseDir.empty() || baseDir == ".") {
        return relativePath;
    }
    return baseDir + "/" + relativePath;
}

