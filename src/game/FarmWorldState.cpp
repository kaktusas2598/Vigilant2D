#include "FarmWorldState.hpp"

#include <cstddef>

void FarmWorldState::init(int newWidth, int newHeight) {
    width = newWidth;
    height = newHeight;
    tiles.clear();
    tiles.resize(static_cast<size_t>(width * height));
}

void FarmWorldState::clear() {
    width = 0;
    height = 0;
    tiles.clear();
}

bool FarmWorldState::isInBounds(int x, int y) const {
    return x >= 0 && y >= 0 && x < width && y < height;
}

int FarmWorldState::indexOf(int x, int y) const {
    return y * width + x;
}

FarmTileState* FarmWorldState::tryGet(int x, int y) {
    if (!isInBounds(x, y))
        return nullptr;
    return &tiles[static_cast<size_t>(indexOf(x, y))];
}

const FarmTileState* FarmWorldState::tryGet(int x, int y) const {
    if (!isInBounds(x, y))
        return nullptr;
    return &tiles[static_cast<size_t>(indexOf(x, y))];
}

bool FarmWorldState::isTilled(int x, int y) const {
    const FarmTileState* tile = tryGet(x, y);
    return tile != nullptr && tile->tilled;
}

bool FarmWorldState::isWatered(int x, int y) const {
    const FarmTileState* tile = tryGet(x, y);
    return tile != nullptr && tile->watered;
}

bool FarmWorldState::isOccupied(int x, int y) const {
    const FarmTileState* tile = tryGet(x, y);
    return tile != nullptr && tile->occupied;
}

bool FarmWorldState::setTilled(int x, int y, bool value) {
    FarmTileState* tile = tryGet(x, y);
    if (tile == nullptr)
        return false;
    tile->tilled = value;
    return true;
}

bool FarmWorldState::setWatered(int x, int y, bool value) {
    FarmTileState* tile = tryGet(x, y);
    if (tile == nullptr)
        return false;
    tile->watered = value;
    return true;
}

bool FarmWorldState::setOccupied(int x, int y, bool value) {
    FarmTileState* tile = tryGet(x, y);
    if (tile == nullptr)
        return false;
    tile->occupied = value;
    return true;
}
