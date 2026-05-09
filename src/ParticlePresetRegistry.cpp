#include "ParticlePresetRegistry.hpp"

#include "AssetManager.hpp"
#include "ParticleEmitter.hpp"
#include "Texture.hpp"

bool ParticlePresetRegistry::registerPreset(const std::string& id, const ParticlePreset& preset) {
    presets[id] = preset;
    return true;
}

const ParticlePreset* ParticlePresetRegistry::getPreset(const std::string& id) const {
    auto it = presets.find(id);
    if (it == presets.end())
        return nullptr;
    return &it->second;
}

bool ParticlePresetRegistry::hasPreset(const std::string& id) const {
    return presets.find(id) != presets.end();
}

bool ParticlePresetRegistry::applyPreset(const std::string& id,
    ParticleEmitter& emitter, AssetManager& assetManager) const {

    const ParticlePreset* preset = getPreset(id);
    if (preset == nullptr)
        return false;
        
    Texture* texture = nullptr;
    if (!preset->textureId.empty())         {
        texture = assetManager.getTexture(preset->textureId);
        if (texture == nullptr)
            return false;
    }

    emitter.init(preset->capacity, TextureRegion::full(texture));
    emitter.setBaseColor(preset->baseColor);
    emitter.setBaseSize(preset->baseSize);
    emitter.setBaseLifetime(preset->baseLifetime);
    emitter.setBaseVelocity(preset->baseVelocity);
    emitter.setVelocityVariance(preset->velocityVariance);
    return true;
}
