#pragma once

#include <vector>
#include <memory>

#include "ImGuiLayer.hpp"
#include "Window.hpp"
#include "Renderer.hpp"
#include "Input.hpp"
#include "Logger.hpp"
#include "Camera2D.hpp"
#include "Time.hpp"

#include "Texture.hpp"
#include "TileLayer.hpp"
#include "TileMapData.hpp"

class Application {
    public:
        void init();
        void run();
        void exit();

        void switchDebugMode();
        bool isDebugModeEnabled();

        Input* getInput();

        // TEMPORARY for test
        std::unique_ptr<TileLayer> buildTileLayer(const TileMapData& map, const TileLayerData& layerData,
            const std::vector<Texture*>& tilesetTextures);
    private:
        void update(float dt);
        void render(float dt);

        Window window;
        ImGuiLayer uiLayer;
        Renderer renderer;

        ImVec4 clearColour;
        bool debugMode;

        Input input;
        Time time;

        // TODO: research whats best for Camera - probably not storing single insance in Application class!
        Camera2D camera;


        // TEMPORARY textures
        Texture* boxTexture = nullptr;

        // For tiled map testing
        TileMapData testMap;
        Texture* atlasTexture = nullptr;
        std::vector<Texture*> tilesetTextures;
        std::vector<std::unique_ptr<TileLayer>> tileLayers;
};
