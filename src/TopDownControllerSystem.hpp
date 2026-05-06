#pragma once

#include <string>

#include "Scene.hpp"
#include "Input.hpp"
#include "AnimationRegistry.hpp"

struct TopDownControllerConfig {
    std::string entityId;

    float moveSpeed = 120.0f;
    std::string idleAnimation;
    std::string walkUpAnimation;
    std::string walkDownAnimation;
    std::string walkRightAnimation;
    bool allowFlipX = true;
};

// Optional way to attach controller to selected entity id for convenience instead of scripting.
class TopDownControllerSystem {
    public:
        TopDownControllerSystem(Scene& scene, Input& input, AnimationRegistry& animationRegistry);
        void setControlledEntity(const TopDownControllerConfig& config);
        void update(float dt);

    private:
        Scene& scene;
        Input& input;
        AnimationRegistry& animationRegistry;
        TopDownControllerConfig controlConfig;
        bool hasController = false;
};