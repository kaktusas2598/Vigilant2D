#include "ContentLoader.hpp"

#include "AssetManager.hpp"
#include "AnimationRegistry.hpp"
#include "ScriptSystem.hpp"
#include "Logger.hpp"

ContentLoader::ContentLoader(AssetManager& assetManager,  AnimationRegistry& animationRegistry, ScriptSystem& scriptSystem)
    : assetManager(assetManager), animationRegistry(animationRegistry), scriptSystem(scriptSystem) {
}

bool ContentLoader::loadAssets(const std::string& manifestFile) {
    std::vector<AssetManifestEntry> textures;
    if (!scriptSystem.loadAssetManifest(manifestFile, textures)) {
        VG_ERROR("Failed to load asset manifest: " + manifestFile);
        return false;
    }

    bool allLoaded = true;
    for (const auto& texture: textures) {
        if (texture.id.empty() || texture.path.empty()) {
            VG_ERROR("Asset manifest entry is midding id or path in: " + manifestFile);
            allLoaded = false;
            continue;
        }

        if (assetManager.loadTexture(texture.id, texture.path) == nullptr) {
            VG_ERROR("Failed to load texture '" + texture.id + "' from '" + texture.path + "'");
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
