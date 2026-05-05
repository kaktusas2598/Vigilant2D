#include "AnimationRegistry.hpp"

#include "AssetManager.hpp"
#include "ScriptSystem.hpp"
#include "Texture.hpp"
#include "TileMapData.hpp" // for makeRegionFromGrid

bool AnimationRegistry::loadClip(const std::string &clipId, const std::string &definitionFile,
              AssetManager &assetManager, ScriptSystem &scriptSystem) {
    AnimationDefinition definition;
    if (!scriptSystem.loadAnimationDefinition(definitionFile, definition))
        return false;

    Texture* texture = assetManager.getTexture(definition.texture);
    if (texture == nullptr)
        return false;
    
    AnimationClip clip(clipId, definition.looping);
    for (int frame : definition.frames) {
        // Convert from Lua's 1 to normal 0-based indexing
        const int frameIndex = frame - 1;
        const int frameX = frameIndex;
        clip.addFrame(
            makeRegionFromGrid(texture, frameX, definition.row, definition.columns, definition.rows),
            definition.frameDuration
        );
    }

    clips[clipId] = std::move(clip);
    return true;
}

const AnimationClip* AnimationRegistry::getClip(const std::string &clipId) const {
    auto it = clips.find(clipId);
    if (it == clips.end())
        return nullptr;

    return &it->second;
}

bool AnimationRegistry::hasClip(const std::string &clipId) const {
    return clips.find(clipId) != clips.end();
}
