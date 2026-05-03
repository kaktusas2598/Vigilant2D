#pragma once

#include <memory>
#include <string>

#include "Transform2D.hpp"
#include "Sprite.hpp"
#include "AnimatedSprite.hpp"
#include "Renderer.hpp"

class Entity {
    public:
        Transform2D transform;

        void setID(const std::string& newID) { id = newID; }
        const std::string& getID() const { return id; }

        void setSprite(std::unique_ptr<Sprite> newSprite) {
            sprite = std::move(newSprite);
            if (animatedSprite && sprite) {
                animatedSprite->setSprite(sprite.get());
            }
        }

        void setAnimatedSprite(std::unique_ptr<AnimatedSprite> newSprite) {
            animatedSprite = std::move(newSprite);
            if (animatedSprite && sprite) {
                animatedSprite->setSprite(sprite.get());
            }
        }

        Sprite* getSprite() { return sprite.get(); }
        const Sprite *getSprite() const { return sprite.get(); }

        AnimatedSprite *getAnimatedSprite() { return animatedSprite.get(); }
        const AnimatedSprite *getAnimatedSprite() const { return animatedSprite.get(); }

        void update(float dt) {
            if (animatedSprite)
                animatedSprite->update(dt);
        }

        void render(Renderer& renderer) {
            if (sprite)
                sprite->draw(renderer, transform);
        }

    private:
        std::string id;
        std::unique_ptr<AnimatedSprite> animatedSprite = nullptr;
        std::unique_ptr<Sprite> sprite = nullptr;
};