#pragma once

#include "glm/glm.hpp"
#include "Renderer.hpp"
#include "Input.hpp"
#include "Camera2D.hpp"
#include "TileMap.hpp"

// Enables hovering over and selecting tiles
class TileCursor {
public:
    void update(const Input& input, const Camera2D& camera, TileMap* map);
    void draw(Renderer& renderer, TileMap* map) const;

    glm::ivec2 getHoveredTile() const { return hoveredTile; }
    glm::ivec2 getSelectedTile() const { return selectedTile; }

private:
    glm::ivec2 hoveredTile{-1, -1};
    glm::ivec2 selectedTile{-1, -1};
};
