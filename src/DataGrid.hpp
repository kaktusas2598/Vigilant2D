#pragma once

#include <vector>
#include "PropertyBag.hpp"

// Generic 2D Grid Structure for storing arbitrary properties for each cell
// which can be set from Lua scripts
class DataGrid {
    public:
        void init(int w, int h);
        void clear();

        bool isInBounds(int x, int y) const;

        PropertyBag* tryGet(int x, int y);
        const PropertyBag* tryGet(int x, int y) const;

        int getWidth() const { return width; }
        int getHeight() const { return height; }
    private:
        int indexOf(int x, int y) const;

        int width = 0;
        int height = 0;
        std::vector<PropertyBag> cells;
};