#include "TopDownControllerSystem.hpp"

TopDownControllerSystem::TopDownControllerSystem(Scene& scene, Input& input, AnimationRegistry& animationRegistry)
    : scene(scene), input(input), animationRegistry(animationRegistry) {
}

void TopDownControllerSystem::setControlledEntity(const TopDownControllerConfig& config) {
    controlConfig = config;
    hasController = true;
}

void TopDownControllerSystem::update(float dt) {
    if (!hasController)
        return;

    Entity *controlledEntity = scene.findEntityByID(controlConfig.entityId);
    if (controlledEntity != nullptr) {
        glm::vec2 movement(0.0f);
        const float controlledEntitySpeed = controlConfig.moveSpeed;

        if (input.isKeyDown(GLFW_KEY_W)) movement.y += 1.0f;
        if (input.isKeyDown(GLFW_KEY_S)) movement.y -= 1.0f;
        if (input.isKeyDown(GLFW_KEY_A)) movement.x -= 1.0f;
        if (input.isKeyDown(GLFW_KEY_D)) movement.x += 1.0f;

        AnimatedSprite* anim = controlledEntity->getAnimatedSprite();
        if (movement.x != 0.0f || movement.y != 0.0f) {
            movement = glm::normalize(movement);

            if (controlledEntity->hasPhysicsBody()) {
                scene.getPhysicsWorld().setBodyLinearVelocityPixels(
                    controlledEntity->getPhysicsBody(),
                    movement * controlledEntitySpeed
                );
            } else {
                controlledEntity->transform.position += movement * controlledEntitySpeed * dt;
            }

            if (anim != nullptr) {
                if (std::abs(movement.x) > std::abs(movement.y)) {
                    if (movement.x > 0.0f) {
                        if (controlledEntity->getSprite()) controlledEntity->getSprite()->setFlipX(false);
                        anim->play(animationRegistry.getClip(controlConfig.walkRightAnimation), false);
                    } else {
                        if (controlledEntity->getSprite()) controlledEntity->getSprite()->setFlipX(true);
                        anim->play(animationRegistry.getClip(controlConfig.walkRightAnimation), false);
                    }
                } else {
                    if (movement.y > 0.0f) {
                        anim->play(animationRegistry.getClip(controlConfig.walkUpAnimation), false);
                    } else {
                        anim->play(animationRegistry.getClip(controlConfig.walkDownAnimation), false);
                    }

                }
            }
        } else {
            if (controlledEntity->hasPhysicsBody()) {
                scene.getPhysicsWorld().setBodyLinearVelocityPixels(
                    controlledEntity->getPhysicsBody(),
                    {0.0f, 0.0f}
                );
            }

            // FIXME: Add animation locking mechanism to allow animations to be played and not
            // imediately overriden the next frame by controlled system if its used alongside Lua
            if (anim != nullptr) {
                anim->play(animationRegistry.getClip(controlConfig.idleAnimation), false);
            }
        }
    }
}
