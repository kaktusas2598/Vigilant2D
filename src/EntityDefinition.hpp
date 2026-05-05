#pragma once

#include <string>
#include "glm/glm.hpp"

// Used by EntityFactory to load Lua-returned table into Entity
struct EntityDefinition {
    std::string texture;
    std::string animation;
    glm::vec2 scale{48.0f, 48.0f};

    glm::vec2 boundsOffset{0.0f, 0.0f};
    glm::vec2 boundsSize{48.0f, 48.0f};
    bool hasBounds = false;

    std::string behaviorScript;
    bool physicsEnabled = false;
};