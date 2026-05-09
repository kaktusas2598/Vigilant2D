#pragma once

#include <string>
#include <unordered_map>

#include "ParticlePreset.hpp"

class AssetManager;
class ParticleEmitter;

class ParticlePresetRegistry {
    public:
        bool registerPreset(const std::string& id, const ParticlePreset& preset);
        const ParticlePreset* getPreset(const std::string& id) const;
        bool hasPreset(const std::string& id) const;

        bool applyPreset(const std::string& id,
            ParticleEmitter& emitter, AssetManager& assetManager) const;

    private:
        std::unordered_map<std::string, ParticlePreset> presets;
};