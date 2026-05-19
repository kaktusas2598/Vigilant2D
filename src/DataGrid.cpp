#include "DataGrid.hpp"

#include <cstddef>

void DataGrid::init(int w, int h) {
    width = w;
    height = h;
    cells.clear();
    cells.resize(static_cast<size_t>(width * height));
}

void DataGrid::clear() {
    width = 0;
    height = 0;
    cells.clear();
}

bool DataGrid::isInBounds(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
}

PropertyBag* DataGrid::tryGet(int x, int y) {
    if (!isInBounds(x, y))
        return nullptr;

    return &cells[static_cast<size_t>(indexOf(x, y))];
}

const PropertyBag* DataGrid::tryGet(int x, int y) const {
    if (!isInBounds(x, y))
        return nullptr;

    return &cells[static_cast<size_t>(indexOf(x, y))];
}

int DataGrid::indexOf(int x, int y) const {
    return y * width + x;
}