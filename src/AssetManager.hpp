#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Shader.hpp"
#include "Texture.hpp"

class AssetManager {
    public:
        Texture* loadTexture(const std::string& id,
             const std::string& filePath, const TextureParams& params = PixelArt());

        Texture* getTexture(const std::string& id);
        const Texture* getTexture(const std::string& id) const;

        Shader* loadShader(const std::string& id,
             const std::string& vertexPath, const std::string& fragmentPath);

        Shader* getShader(const std::string& id);
        const Shader* getShader(const std::string& id) const; 

        bool hasTexture(const std::string& id) const;
        bool hasShader(const std::string& id) const;

        std::vector<std::string> getTextureIDs() const;
        std::vector<std::string> getShaderIDs() const;

        void clear();

    private:
        std::unordered_map<std::string, std::unique_ptr<Texture>> textures;
        std::unordered_map<std::string, std::unique_ptr<Shader>> shaders;
};