#include "ParticleEmitter.hpp"

#include <cstdlib>

namespace {
    float randomRange(float minValue, float maxValue) {
        const float t = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        return minValue + (maxValue - minValue) * t;
    }

    void defaultParticleUpdate(Particle& particle, float dt) {
        particle.position += particle.velocity * dt;

        // BLOOD
        // particle.velocity.y -= 120.0f * dt;
        // particle.color.a = particle.life / particle.maxLife;
    }
}
void ParticleEmitter::init(size_t maxParticles, const TextureRegion &region, ParticleUpdateFn updateFn) {
    this->maxParticles = maxParticles;
    this->region = region;
    this->updateFn = updateFn ? std::move(updateFn) : defaultParticleUpdate;

    particles.clear();
    particles.resize(maxParticles);
    vertices.reserve(maxParticles * 4);
    indices.reserve(maxParticles * 6);
    mesh.initDynamic(maxParticles * 4, maxParticles * 6);

    lastFreeParticle = 0;
    dirty = true;
}

void ParticleEmitter::update(float dt) {
    bool changed = false;
    for (auto& particle : particles) {
        if (!particle.active)
            continue;
        
            particle.life -= dt;
            if (particle.life <= 0.0f) {
                particle.active = false;
                changed = true;
                continue;
            }

            updateFn(particle, dt);
            changed = true;
    }

    if (changed) {
        dirty = true;
    }
}

void ParticleEmitter::emit(const glm::vec2 &position, int count) {
    for (int i = 0; i < count; ++i) {
        const int index = findFreeParticle();
        Particle& particle = particles[index];

        particle.active = true;
        particle.position = position;
        particle.velocity = {
            baseVelocity.x + randomRange(-velocityVariance.x, velocityVariance.x),
            baseVelocity.y + randomRange(-velocityVariance.y, velocityVariance.y)
        };
        particle.color = baseColor;
        particle.size = baseSize;
        particle.maxLife = baseLifetime;
        particle.life = baseLifetime;
        particle.rotation = 0.0f;
    }
    dirty = true;
}

void ParticleEmitter::draw(Renderer &renderer) {
    if (dirty) {
        rebuildMesh();
        dirty = false;
    }

    renderer.drawMesh(mesh,region.texture);
}

int ParticleEmitter::findFreeParticle() {
    for (size_t i = lastFreeParticle; i < particles.size(); ++i) {
        if (!particles[i].active) {
            lastFreeParticle = static_cast<int>(i);
            return static_cast<int>(i);
        }
    }

    for (int i = 0; i < lastFreeParticle; ++i) {
        if (!particles[i].active) {
            lastFreeParticle = i;
            return i;
        }
    }

    lastFreeParticle = 0;
    return 0;
}

void ParticleEmitter::rebuildMesh() {
    vertices.clear();
    indices.clear();

    const glm::vec2 uvMin = region.uvMin;
    const glm::vec2 uvMax = region.uvMax;
    for (const auto& particle : particles) {
        if (!particle.active)
            continue;
        
        const float halfSize = particle.size * 0.5f;
        const float x0 = particle.position.x - halfSize;
        const float y0 = particle.position.y - halfSize;
        const float x1 = particle.position.x + halfSize;
        const float y1 = particle.position.y + halfSize;
        const unsigned int baseIndex = static_cast<unsigned int>(vertices.size());

        vertices.push_back({{x0, y0, 0.0f}, {uvMin.x, uvMin.y}, particle.color});
        vertices.push_back({{x1, y0, 0.0f}, {uvMax.x, uvMin.y}, particle.color});
        vertices.push_back({{x1, y1, 0.0f}, {uvMax.x, uvMax.y}, particle.color});
        vertices.push_back({{x0, y1, 0.0f}, {uvMin.x, uvMax.y}, particle.color});

        indices.push_back(baseIndex + 0);
        indices.push_back(baseIndex + 1);
        indices.push_back(baseIndex + 2);
        indices.push_back(baseIndex + 2);
        indices.push_back(baseIndex + 3);
        indices.push_back(baseIndex + 0);
    }

    mesh.upload(vertices, indices);
}
