#pragma once

class Time {
    public:
        void init(double startTimeSeconds) {
            lastTime = startTimeSeconds;
        }

        void update(double currentTimeSeconds) {
            deltaTime = static_cast<float>(currentTimeSeconds - lastTime);
            lastTime = currentTimeSeconds;

            frameCount++;
            fpsAccumTime += deltaTime;

            if (fpsAccumTime >= fpsUpdateInterval) {
                fps = static_cast<float>(frameCount) / fpsAccumTime;
                frameTimeMs = fps > 0.0f ? 1000.0f / fps : 0.0f;

                frameCount = 0;
                fpsAccumTime = 0.0f;
            }
        }

        float getDeltaTime() const { return deltaTime; }
        float getFPS() const { return fps; }
        float getFrameTimeMs() const { return frameTimeMs; }

    private:
        double lastTime = 0.0f;
        double deltaTime = 0.0f;
        int frameCount = 0;
        float fpsAccumTime = 0.0f;
        float fps = 0.0f;
        float frameTimeMs = 0.0f;
        float fpsUpdateInterval = 0.25f; // Update interval set to reduce jitter while displaying FPS in imgui
};