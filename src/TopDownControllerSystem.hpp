#pragma once

#include <string>
#include "TopDownControllerConfig.hpp"

class Scene;
class Input;
class AnimationRegistry;

// Optional way to attach controller to selected entity id for convenience instead of scripting.
class TopDownControllerSystem {
    public:
        TopDownControllerSystem(Scene& scene, Input& input, AnimationRegistry& animationRegistry);
        void setControlledEntity(const TopDownControllerConfig& config);
        bool attachFirstConfiguredEntity();
        void update(float dt);

    private:
        Scene& scene;
        Input& input;
        AnimationRegistry& animationRegistry;
        TopDownControllerConfig controlConfig;
        bool hasController = false;
};