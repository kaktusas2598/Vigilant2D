#pragma once

#include "Transform2D.hpp"

#include "AnimatedSprite.hpp";

// TODO: implement
class Entity {
    public:
        Transform2D transform;
        AnimatedSprite* animatedSprite = nullptr;
        Sprite* sprite = nullptr;

        void update(float dt) {
            if (animatedSprite)
                animatedSprite->update(dt);
        }

        void render(float dt) {
            if (sprite)
                sprite->draw(dt);
        }
};