#include "AnimatedSprite.hpp"

void AnimatedSprite::play(const AnimationClip *newClip, bool restart) {
    if (newClip == nullptr)
        return;

    if (clip == newClip && !restart)
        return;

    clip = newClip;
    currentFrame = 0;
    frameTimer = 0.0f;
    playing = true;
    finished = false;

    applyCurrentFrame();
}

void AnimatedSprite::setFrame(size_t frameIndex) {
    if (clip == nullptr || frameIndex >= clip->getFrameCount())
        return;
    
    currentFrame = frameIndex;
    frameTimer = 0.0f;
    applyCurrentFrame();
}

void AnimatedSprite::update(float dt) {
    if (!playing || clip == nullptr || sprite == nullptr || clip->getFrameCount() == 0)
        return;

    frameTimer += dt;

    while (frameTimer >= clip->getFrame(currentFrame).duration) {
        frameTimer -= clip->getFrame(currentFrame).duration;
        ++currentFrame;

        if (currentFrame >= clip->getFrameCount()) {
            if (clip->isLooping()) {
                currentFrame = 0;
            } else {
                currentFrame = clip->getFrameCount() - 1;
                playing = false;
                finished = true;
                break;
            }
        }

        applyCurrentFrame();
    }
}

void AnimatedSprite::applyCurrentFrame() {
    if (clip == nullptr || sprite == nullptr || clip->getFrameCount() == 0)
        return;

    sprite->setRegion(clip->getFrame(currentFrame).region);
}
