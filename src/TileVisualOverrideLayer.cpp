#include "TileVisualOverrideLayer.hpp"

void TileVisualOverrideLayer::init(int newWidth, int newHeight) {
    width = newWidth;
    height = newHeight;
    visuals.clear();
    visuals.resize(static_cast<size_t>(width * height), TileVisual::none());
}

void TileVisualOverrideLayer::clear() {
    width = 0;
    height = 0;
    visuals.clear();
}

bool TileVisualOverrideLayer::isInBounds(int x, int y) const {
    return x>= 0 && y >= 0 && x < width && y < height;
}

const TileVisual* TileVisualOverrideLayer::tryGet(int x, int y) const {
    if (!isInBounds(x, y))
        return nullptr;
    return &visuals[static_cast<size_t>(indexOf(x, y))];
}

TileVisual* TileVisualOverrideLayer::tryGet(int x, int y) {
    if (!isInBounds(x, y))
        return nullptr;
    return &visuals[static_cast<size_t>(indexOf(x, y))];
}

bool TileVisualOverrideLayer::set(int x, int y, const TileVisual& visual) {
    TileVisual* cell = tryGet(x, y);
    if (cell == nullptr)
        return false;

    *cell = visual;
    return true;

}
bool TileVisualOverrideLayer::clearAt(int x, int y) {
    TileVisual* cell = tryGet(x, y);
    if (cell == nullptr)
        return false;

    *cell = TileVisual::none();
    return true;
}

bool TileVisualOverrideLayer::hasOverride(int x, int y) const {
    const TileVisual* cell = tryGet(x, y);
    return cell != nullptr && cell->isSet();
}

int TileVisualOverrideLayer::indexOf(int x, int y) const {
    return y * width + x;
}
