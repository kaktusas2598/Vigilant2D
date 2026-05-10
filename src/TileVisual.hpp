#pragma once

#include "TextureRegion.hpp"

enum class TileVisualKind {
    None,
    StaticRegion,
    AnimatedSequence // TODO: implement usage
};

// Defines how Tile is looking during runtime, currently only static runtime overrides allowed
// TODO: add support for animating tile, maybe tint and more params in the future
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