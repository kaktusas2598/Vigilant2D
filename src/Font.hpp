#pragma once

#include <string>
#include <unordered_map>

#include "glm/glm.hpp"
#include "Texture.hpp"

extern "C" {
    #include <ft2build.h>
    #include FT_FREETYPE_H
}

struct Glyph {
    Texture texture;
    glm::ivec2 size{0, 0};
    glm::ivec2 bearing{0, 0};
    unsigned int advance = 0;
};

class Font {
    public:
        Font() = default;

        bool loadFromFile(const std::string& filePath, int pixelSize);

        const Glyph* getGlyph(char c) const;
        int getPixelSize() const { return pixelSize; }

    private:
        std::unordered_map<char, Glyph> glyphs;
        int pixelSize = 0;
};