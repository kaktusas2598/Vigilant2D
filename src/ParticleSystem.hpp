#pragma once

#include <vector>
#include <memory>

#include "ParticleEmitter.hpp"
#include "Renderer.hpp"

class ParticleSystem {
    public:
        ParticleEmitter& createEmitter();
        void update(float dt);
        void draw(Renderer& renderer) const;
        void clear();

        size_t getEmitterCount() const { return emitters.size(); }
    
    private:
        std::vector<std::unique_ptr<ParticleEmitter>> emitters;
};
