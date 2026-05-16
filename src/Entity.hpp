#pragma once

#include <memory>
#include <string>

#include "Transform2D.hpp"
#include "Sprite.hpp"
#include "AnimatedSprite.hpp"
#include "Renderer.hpp"
#include <box2d/box2d.h>
#include "PropertyBag.hpp"
#include <optional>
#include "TopDownControllerConfig.hpp"

// TODO: I don't like Entity owning Sprites, animated sprites, physics handles, script etc.
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

        void setPhysicsBody(b2BodyId newBody) { physicsBody = newBody; }
        b2BodyId getPhysicsBody() const { return physicsBody; }
        bool hasPhysicsBody() const { return B2_IS_NON_NULL(physicsBody); }

        void setScript(const std::string& fileName) { scriptName = fileName; }
        const std::string& getScriptName() const { return scriptName; }
        bool hasScript() const { return !scriptName.empty(); }

        // For defining entity bounding box, used in collision, selection
        void setBounds(const glm::vec2& offset, const glm::vec2& size) {
            boundsOffset = offset;
            boundsSize = size;
            useCustomBounds = true;
        }
        glm::vec2 getBoundsOffset() const { return boundsOffset; }
        glm::vec2 getBoundsSize() const { return useCustomBounds ? boundsSize : transform.scale; }
        glm::vec2 getBoundsPosition() const { return transform.position + boundsOffset; }

        PropertyBag& getCustomData() {return customData; }
        const PropertyBag& getCustomData() const {return customData; }

        void setTopDownControllerConfig(const TopDownControllerConfig& config) {
            topDownControllerConfig = config;
        }
        bool hasTopDownController() const { return topDownControllerConfig.has_value(); }
        const std::optional<TopDownControllerConfig>& getTopDownControllerConfig() const { return topDownControllerConfig; }

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
        b2BodyId physicsBody = b2_nullBodyId;
        std::string scriptName;

        glm::vec2 boundsOffset{0.0f, 0.0f};
        glm::vec2 boundsSize{0.0f, 0.0f};
        bool useCustomBounds = false;

        std::optional<TopDownControllerConfig> topDownControllerConfig;
        PropertyBag customData;
};