#include "TopDownControllerSystem.hpp"

#include "Scene.hpp"
#include "core/Input.hpp"
#include "AnimationRegistry.hpp"

TopDownControllerSystem::TopDownControllerSystem(Scene& scene, Input& input, AnimationRegistry& animationRegistry)
    : scene(scene), input(input), animationRegistry(animationRegistry) {
}

void TopDownControllerSystem::setControlledEntity(const TopDownControllerConfig& config) {
    controlConfig = config;
    hasController = true;
}

bool TopDownControllerSystem::attachFirstConfiguredEntity() {
    for (const auto& entityPtr : scene.getEntities()) {
        if (entityPtr == nullptr || !entityPtr->hasTopDownController())
            continue;
        
        setControlledEntity(*entityPtr->getTopDownControllerConfig());
        return true;
    }

    return false;
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
        const bool animationLocked = controlledEntity->isAnimationLocked();
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

            if (!animationLocked && anim != nullptr) {
                if (std::abs(movement.x) > std::abs(movement.y)) {
                    if (movement.x > 0.0f) {
                        if (controlConfig.allowFlipX && controlledEntity->getSprite())
                            controlledEntity->getSprite()->setFlipX(false);
                        anim->play(animationRegistry.getClip(controlConfig.walkRightAnimation), false);
                    } else {
                        if (controlConfig.allowFlipX && controlledEntity->getSprite())
                            controlledEntity->getSprite()->setFlipX(true);
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

            if (!animationLocked && anim != nullptr) {
                anim->play(animationRegistry.getClip(controlConfig.idleAnimation), false);
            }
        }
    }
}

void TopDownControllerSystem::clearControlledEntity() {
    controlConfig = TopDownControllerConfig{};
    hasController = false;
}