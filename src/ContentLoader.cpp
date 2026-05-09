#include "ContentLoader.hpp"

#include "AssetManager.hpp"
#include "AnimationRegistry.hpp"
#include "ParticlePresetRegistry.hpp"
#include "ScriptSystem.hpp"
#include "Logger.hpp"

ContentLoader::ContentLoader(AssetManager& assetManager,  AnimationRegistry& animationRegistry,
                      ParticlePresetRegistry& particlePresetRegistry, ScriptSystem& scriptSystem)
    : assetManager(assetManager), animationRegistry(animationRegistry), particlePresetRegistry(particlePresetRegistry), scriptSystem(scriptSystem) {
}

bool ContentLoader::loadAssets(const std::string& manifestFile) {
    std::vector<TextureManifestEntry> textures;
    std::vector<FontManifestEntry> fonts;
    if (!scriptSystem.loadAssetManifest(manifestFile, textures, fonts)) {
        VG_ERROR("Failed to load asset manifest: " + manifestFile);
        return false;
    }

    bool allLoaded = true;
    // Bootstrap textures
    for (const auto& texture: textures) {
        if (texture.id.empty() || texture.path.empty()) {
            VG_ERROR("Texture Asset manifest entry is midding id or path in: " + manifestFile);
            allLoaded = false;
            continue;
        }

        if (assetManager.loadTexture(texture.id, texture.path) == nullptr) {
            VG_ERROR("Failed to load texture '" + texture.id + "' from '" + texture.path + "'");
            allLoaded = false;
        } 
    }

    // Bootstrap fonts
    for (const auto& font: fonts) {
        if (font.id.empty() || font.path.empty()) {
            VG_ERROR("Font Asset manifest entry is midding id or path in: " + manifestFile);
            allLoaded = false;
            continue;
        }

        if (assetManager.loadFont(font.id, font.path, font.pixelSize) == nullptr) {
            VG_ERROR("Failed to load font '" + font.id + "' from '" + font.path + "'");
            allLoaded = false;
        } 
    }

    return allLoaded;
}

bool ContentLoader::loadAnimations(const std::string& manifestFile) {
    std::vector<AnimationManifestEntry> animations;
    if (!scriptSystem.loadAnimationManifest(manifestFile, animations)) {
        VG_ERROR("Failed to load animation manifest: " + manifestFile);
        return false;
    }

    bool allLoaded = true;
    for (const auto& animation: animations) {
        if (animation.id.empty() || animation.path.empty()) {
            VG_ERROR("Animation manifest entry is midding id or path in: " + manifestFile);
            allLoaded = false;
            continue;
        }

        if (!animationRegistry.loadClip(animation.id, animation.path, assetManager, scriptSystem)) {
            VG_ERROR("Failed to load animation '" + animation.id + "' from '" + animation.path + "'");
            allLoaded = false;
        } 
    }

    return allLoaded;
}

bool ContentLoader::loadParticlePresets(const std::string& manifestFile) {
    std::vector<ParticlePresetManifestEntry> presets;
    if (!scriptSystem.loadParticlePresetManifest(manifestFile, presets)) {
        VG_ERROR("Failed to load particle emitter preset manifest: " + manifestFile);
        return false;
    }

    bool allLoaded = true;
    for (const auto& presetEntry: presets) {
        if (presetEntry.id.empty() || presetEntry.path.empty()) {
            VG_ERROR("Particle emitter preset manifest entry is midding id or path in: " + manifestFile);
            allLoaded = false;
            continue;
        }

        ParticlePreset preset;
        if (!scriptSystem.loadParticlePresetDefinition(presetEntry.path, preset)) {
            VG_ERROR("Failed to load particle preset '" + presetEntry.id + "' from '" + presetEntry.path + "'");
            allLoaded = false;
            continue;
        }

        if (!particlePresetRegistry.registerPreset(presetEntry.id, preset)) {
            VG_ERROR("Failed to register particle preset '" + presetEntry.id + "' from '" + presetEntry.path + "'");
            allLoaded = false;
        } 
    }

    return allLoaded;
}
