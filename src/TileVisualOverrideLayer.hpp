#pragma once

#include <vector>
#include "TileVisual.hpp"

// Defines optional visual overrides for tiles in a layer of tiled map
class TileVisualOverrideLayer {
    public:
        void init(int newWidth, int newHeight);
        void clear();

        bool isInBounds(int x, int y) const;

        const TileVisual* tryGet(int x, int y) const;
        TileVisual* tryGet(int x, int y);

        bool set(int x, int y, const TileVisual& visual);
        bool clearAt(int x, int y);
        bool hasOverride(int x, int y) const;

    private:
        int indexOf(int x, int y) const;

        int width = 0;
        int height = 0;
        std::vector<TileVisual> visuals;
};