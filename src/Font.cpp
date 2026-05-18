#include "Font.hpp"

#include "core/Logger.hpp"

bool Font::loadFromFile(const std::string& filePath, int newPixelSize) {
    glyphs.clear();
    pixelSize = newPixelSize;

    FT_Library ft = nullptr;
    if (FT_Init_FreeType(&ft)) {
        VG_ERROR("Failed to initialize FreeType.");
        return false;
    }

    FT_Face face = nullptr;
    if (FT_New_Face(ft, filePath.c_str(), 0, &face)) {
        VG_ERROR("Failed to load font: " + filePath);
        FT_Done_FreeType(ft);
        return false;
    }

    FT_Set_Pixel_Sizes(face, 0, pixelSize);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    TextureParams glyphParams;
    glyphParams.wrapS = GL_CLAMP_TO_EDGE;
    glyphParams.wrapT = GL_CLAMP_TO_EDGE;
    glyphParams.minFilter = GL_LINEAR;
    glyphParams.magFilter = GL_LINEAR;
    glyphParams.generateMipmaps = false;
    glyphParams.flipY = false;

    for (unsigned char c = 0; c < 128; ++c) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            VG_ERROR("Failed to load glyph: " + std::to_string(c));
            continue;
        }

        Glyph glyph;
        glyph.texture.initFromBuffer2D(
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            GL_R8,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer,
            glyphParams
        );

        glyph.size = {
            static_cast<int>(face->glyph->bitmap.width),
            static_cast<int>(face->glyph->bitmap.rows)
        };

        glyph.bearing = {
            face->glyph->bitmap_left,
            face->glyph->bitmap_top
        };

        glyph.advance = static_cast<unsigned int>(face->glyph->advance.x);

        glyphs.emplace(static_cast<char>(c), std::move(glyph));
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    return true;
}

const Glyph* Font::getGlyph(char c) const {
    auto it = glyphs.find(c);
    if (it == glyphs.end()) {
        return nullptr;
    }
    return &it->second;
}
