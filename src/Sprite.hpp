#pragma once

#include "TextureRegion.hpp"
#include "Transform2D.hpp"
#include "Renderer.hpp"

class Sprite {
public:
    void setRegion(const TextureRegion& newRegion) { region = newRegion; }
    const TextureRegion& getRegion() const { return region; }

    void setColor(const glm::vec4& newColor) { color = newColor; }
    const glm::vec4& getColor() const { return color; }

    void setFlipX(bool value) { flipX = value; }
    void setFlipY(bool value) { flipY = value; }

    bool isFlippedX() const { return flipX; }
    bool isFlippedY() const { return flipY; }

    void draw(Renderer& renderer, const Transform2D& transform) const {
        renderer.drawQuad({transform, region, color, flipX, flipY});
    }

private:
    TextureRegion region;
    glm::vec4 color{1.0f, 1.0f, 1.0f, 1.0f};
    bool flipX = false;
    bool flipY = false;
};
