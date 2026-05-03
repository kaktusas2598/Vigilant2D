#pragma once

#include <string>
#include "glm/glm.hpp"

// Forward declarations
class Input;
class Camera2D;
class Scene;
class TileMap;
class Renderer;
class Entity;

// Enables hovering over and selecting tiles and/or entities
class SelectionManager {
    public:
        void update(const Input& input, const Camera2D& camera, Scene& scene);
        void draw(Renderer& renderer, Scene& scene) const;

        glm::ivec2 getHoveredTile() const { return hoveredTile; }
        glm::ivec2 getSelectedTile() const { return selectedTile; }

        const std::string& getHoveredEntityId() const { return hoveredEntityId; }
        const std::string& getSelectedEntityId() const { return selectedEntityId; }
    private:
        glm::vec2 getMouseWorld(const Input& input, const Camera2D& camera) const;
        bool pointInEntity(const glm::vec2& point, const Entity& entity) const;
        
        glm::ivec2 hoveredTile{-1, -1};
        glm::ivec2 selectedTile{-1, -1};
        std::string hoveredEntityId;
        std::string selectedEntityId;
};
