#pragma once

#include "TextureRegion.hpp"

enum class TileVisualKind {
    None,
    StaticRegion
};

// Defines how Tile is looking during runtime
// Higher-level systems like animated tiles can keep updating that region over time.
struct TileVisual {
    TileVisualKind kind = TileVisualKind::None;
    TextureRegion region = TextureRegion::full(nullptr);

    bool isSet() const { return kind != TileVisualKind::None; }

    static TileVisual none() {
        return {};
    }

    static TileVisual fromRegion(const TextureRegion& region) {
        TileVisual visual;
        visual.kind = TileVisualKind::StaticRegion;
        visual.region = region;
        return visual;
    }
};