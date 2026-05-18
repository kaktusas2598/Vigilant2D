#pragma once

#include <functional>
#include <vector>

#include "Mesh.hpp"
#include "Particle.hpp"
#include "Renderer.hpp"
#include "TextureRegion.hpp"

class ParticleEmitter {
    public:
        using ParticleUpdateFn = std::function<void(Particle&, float)>;

        ParticleEmitter() = default;

        void init(size_t maxParticles,
            const TextureRegion& region = TextureRegion::full(nullptr),
            ParticleUpdateFn updateFn = nullptr);

        void update(float dt);
        void emit(const glm::vec2& position, int count = 1);
        void draw(Renderer& renderer);

        void setTextureRegion(const TextureRegion& newRegion) { region = newRegion; }
        void setBaseVelocity(const glm::vec2& velocity) { baseVelocity = velocity; }
        void setVelocityVariance(const glm::vec2& newVariance) { velocityVariance = newVariance; }
        void setBaseColor(const glm::vec4& newColor) { baseColor = newColor; }
        void setBaseSize(float newSize) { baseSize = newSize; }
        void setBaseLifetime(float newLifetime) { baseLifetime = newLifetime; }
        void setEnabled(bool value) { enabled = value; }

        const glm::vec2& getBaseVelocity() const { return baseVelocity; }
        const glm::vec2& getVelocityVariance() const { return velocityVariance; }
        const glm::vec4& getBaseColor() const { return baseColor; }
        float getBaseSize() const { return baseSize; }
        float getBaseLifetime() const { return baseLifetime; }
        bool isEnabled() const { return enabled; }

    private:
        int findFreeParticle();
        void rebuildMesh();

        std::vector<Particle> particles;
        size_t maxParticles = 0;
        int lastFreeParticle = 0;

        TextureRegion region;
        ParticleUpdateFn updateFn;

        glm::vec2 baseVelocity{0.0f, 50.0f};
        glm::vec2 velocityVariance{20.0f, 20.0f};
        glm::vec4 baseColor{1.0f};
        float baseSize = 16.0f;
        float baseLifetime = 1.0f;

        Mesh mesh;
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        bool dirty = false;
        bool enabled = true;
};
