#pragma once

#include <vector>

#include "UIPrimitives.hpp"
#include "Camera2D.hpp"

class Renderer;

class UIRenderer {
    public:
        explicit UIRenderer(Renderer& renderer);

        void beginScreen(int viewportWidth, int viewportHeight);
        void beginWorld(const Camera2D& camera);
        void drawQuad(const UIQuad& quad);
        void drawQuadOutline(const UIQuadOutline& outline);
        void end();

        UIRenderSpace getCurrentSpace() const { return currentSpace; }
        const Camera2D& getScreenCamera() const { return uiCamera; }
    private:
        Renderer& renderer;
        UIRenderSpace currentSpace = UIRenderSpace::Screen;

        int viewportWidth = 0;
        int viewportHeight = 0;
        Camera2D uiCamera;
        const Camera2D* worldCamera = nullptr;
};