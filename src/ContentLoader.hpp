#pragma once

#include <string>

class AssetManager;
class AnimationRegistry;
class ScriptSystem;

// Helps bootstrap project assets(textures, fonts, sounds) and animations and load them in the engine
class ContentLoader {
    public:
        ContentLoader(AssetManager& assetManager, 
                      AnimationRegistry& animationRegistry, 
                      ScriptSystem& scriptSystem);
        
        bool loadAssets(const std::string& manifestFile);
        bool loadAnimations(const std::string& manifestFile);

    private:
            AssetManager& assetManager;
            AnimationRegistry& animationRegistry;
            ScriptSystem& scriptSystem;
};