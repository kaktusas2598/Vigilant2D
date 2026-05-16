#pragma once

#include <string>

struct TopDownControllerConfig {
    std::string entityId;

    float moveSpeed = 120.0f;
    std::string idleAnimation;
    std::string walkUpAnimation;
    std::string walkDownAnimation;
    std::string walkRightAnimation;
    bool allowFlipX = true;
};