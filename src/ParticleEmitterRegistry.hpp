#pragma once

#include <string>
#include <vector>
#include <unordered_map>

class ParticleEmitter;
class ParticleSystem;
class ParticlePresetRegistry;
class AssetManager;

// Runtime register of active particle emitters created from particle presets
class ParticleEmitterRegistry {
    public:
        ParticleEmitter* createEmitter(const std::string& id, ParticleSystem& particleSystem);
        ParticleEmitter* createEmitterFromPreset(const std::string& id,
                                                 const std::string& presetId,
                                                 ParticleSystem& particleSystem,
                                                 const ParticlePresetRegistry& presetRegistry,
                                                 AssetManager& assetManager);

        ParticleEmitter* getEmitter(const std::string& id);
        const ParticleEmitter* getEmitter(const std::string& id) const;
        bool hasEmitter(const std::string& id) const;
        std::vector<std::string> getEmitterIDs() const;

    private:
        std::unordered_map<std::string, ParticleEmitter*> emitters;
};