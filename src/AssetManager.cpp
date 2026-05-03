#include "AssetManager.hpp"

Texture* AssetManager::loadTexture(const std::string &id,
                     const std::string &filePath, const TextureParams &params) {
    auto it = textures.find(id);
    if (it != textures.end()) {
        return it->second.get();
    }

    auto texture = std::make_unique<Texture>();
    texture->load2D(filePath);

    Texture* result = texture.get();
    textures[id] = std::move(texture);
    return result;
}

Texture* AssetManager::getTexture(const std::string &id) {
    auto it = textures.find(id);
    return it != textures.end() ? it->second.get() : nullptr;
}

const Texture* AssetManager::getTexture(const std::string &id) const {
    auto it = textures.find(id);
    return it != textures.end() ? it->second.get() : nullptr;
}

Shader* AssetManager::loadShader(const std::string &id,
                     const std::string &vertexPath, const std::string &fragmentPath) {
    auto it = shaders.find(id);
    if (it != shaders.end()) {
        return it->second.get();
    }

    auto shader = std::make_unique<Shader>(vertexPath, fragmentPath);
    Shader* result = shader.get();
    shaders[id] = std::move(shader);
    return result;
}

Shader* AssetManager::getShader(const std::string &id) {
    auto it = shaders.find(id);
    return it != shaders.end() ? it->second.get() : nullptr;
}

const Shader* AssetManager::getShader(const std::string &id) const {
    auto it = shaders.find(id);
    return it != shaders.end() ? it->second.get() : nullptr;
}

bool AssetManager::hasTexture(const std::string &id) const {
    return textures.find(id) != textures.end();
}

bool AssetManager::hasShader(const std::string &id) const {
    return shaders.find(id) != shaders.end();
}

std::vector<std::string> AssetManager::getTextureIDs() const {
    std::vector<std::string> ids;
    ids.reserve(textures.size());

    for (const auto& pair : textures) {
        ids.push_back(pair.first);
    }
    return ids;
}

std::vector<std::string> AssetManager::getShaderIDs() const {
    std::vector<std::string> ids;
    ids.reserve(shaders.size());

    for (const auto& pair : shaders) {
        ids.push_back(pair.first);
    }
    return ids;
}

void AssetManager::clear() {
    textures.clear();
    shaders.clear();
}
