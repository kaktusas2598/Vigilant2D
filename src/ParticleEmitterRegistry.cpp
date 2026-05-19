#include "ParticleEmitterRegistry.hpp"

#include <algorithm>
#include "ParticleEmitter.hpp"
#include "ParticleSystem.hpp"
#include "ParticlePresetRegistry.hpp"
#include "AssetManager.hpp"

ParticleEmitter* ParticleEmitterRegistry::createEmitter(const std::string& id, ParticleSystem& particleSystem) {
    auto it = emitters.find(id);
    if (it != emitters.end())
        return it->second;
    
    ParticleEmitter& emitter = particleSystem.createEmitter();
    emitters[id] = &emitter;
    return &emitter;
}

ParticleEmitter* ParticleEmitterRegistry::createEmitterFromPreset(const std::string& id,
                                            const std::string& presetId,
                                            ParticleSystem& particleSystem,
                                            const ParticlePresetRegistry& presetRegistry,
                                            AssetManager& assetManager) {
    
    ParticleEmitter* emitter = createEmitter(id, particleSystem);
    if (emitter == nullptr)
        return nullptr;

    if (!presetRegistry.applyPreset(presetId, *emitter, assetManager))
        return nullptr;

    return emitter;
}

ParticleEmitter* ParticleEmitterRegistry::getEmitter(const std::string& id) {
    auto it = emitters.find(id);
    return it != emitters.end() ? it->second : nullptr;
}

const ParticleEmitter* ParticleEmitterRegistry::getEmitter(const std::string& id) const {
    auto it = emitters.find(id);
    return it != emitters.end() ? it->second : nullptr;
}

bool ParticleEmitterRegistry::hasEmitter(const std::string& id) const {
    return emitters.find(id) != emitters.end();
}

std::vector<std::string> ParticleEmitterRegistry::getEmitterIDs() const {
    std::vector<std::string> ids;
    ids.reserve(emitters.size());

    for (const auto& pair : emitters) {
        ids.push_back(pair.first);
    }

    std::sort(ids.begin(), ids.end());
    return ids;
}

void ParticleEmitterRegistry::clear() {
    emitters.clear();
}