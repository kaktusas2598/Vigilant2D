#include "ParticleSystem.hpp"

ParticleEmitter& ParticleSystem::createEmitter() {
    emitters.push_back(std::make_unique<ParticleEmitter>());
    return *emitters.back();
}

void ParticleSystem::update(float dt) {
    for (auto& emitter : emitters) {
        emitter->update(dt);
    }
}

void ParticleSystem::draw(Renderer &renderer) const {
    for (auto& emitter : emitters) {
        emitter->draw(renderer);
    }
}
