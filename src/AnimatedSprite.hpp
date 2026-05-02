#pragma once

#include "AnimationClip.hpp"
#include "Sprite.hpp"

class AnimatedSprite {
    public:
        void setSprite(Sprite* newSprite) { sprite = newSprite; };

        void play(const AnimationClip* newClip, bool restart = true);
        void stop() { playing = false; }
        void pause() { playing = false; }
        void resume() { 
            if (clip != nullptr)
                playing = true; 
        }

        void setFrame(size_t frameIndex);
        void update(float dt);

        bool isPlaying() const { return playing; }
        bool isFinished() const { return finished; }
        const AnimationClip* getClip() const { return clip; }
    
    private:
        void applyCurrentFrame();

        Sprite* sprite = nullptr;
        const AnimationClip* clip = nullptr;
        size_t currentFrame = 0;
        float frameTimer = 0.0f;
        bool playing = false;
        bool finished = false;
};