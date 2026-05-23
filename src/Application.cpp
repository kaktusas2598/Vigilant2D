#include "Application.hpp"

#include "core/ErrorHandler.hpp"
#include "core/Logger.hpp"

#include "TileMap.hpp"
#include "EntityFactory.hpp"

void errorCallback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

// Static GLFW callbacks setup here by Application and updating Input service state
static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    app->getInput()->onKeyEvent(key, action);
}

static void mouseMoveCallback(GLFWwindow* window, double xpos, double ypos) {
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    app->getInput()->onMouseMoveEvent(xpos, ypos);
}

static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    app->getInput()->onMouseButtonEvent(button, action);
}

static void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    app->getInput()->onMouseScrollEvent(xoffset, yoffset);
}

static void windowSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

Input* Application::getInput() {
    return &input;
}

bool Application::bootstrapGameplaySession() {
    ScriptInstance bootstrap = scriptSystem.loadScriptTable("scripts/bootstrap.lua");
    if (bootstrap.tableRef == LUA_NOREF) {
        VG_ERROR("Failed to load scripts/bootstrap.lua");
        return false;
    }

    bool ok = true;

    if (!scriptSystem.runScriptInstanceFunction(bootstrap, "start")) {
        VG_ERROR("Failed to run bootstrap.start()");
        ok = false;
    }

    if (ok && scene.getTileMap() != nullptr) {
        entityFactory->spawnFromMapObjects(scene.getTileMap()->getData(), "Entities");
    }

    if (ok && !scriptSystem.runScriptInstanceFunction(bootstrap, "post_start")) {
        VG_ERROR("Failed to run bootstrap.post_start()");
        ok = false;
    }

    scriptSystem.releaseInstance(bootstrap);

    topDownControllerSystem->clearControlledEntity();
    if (!topDownControllerSystem->attachFirstConfiguredEntity()) {
        VG_INFO("No entity with top-down controller config found.");
    }

    return ok;
}

void Application::clearGameplaySession() {
    scriptSystem.cancelAllTasks();

    for (const auto& entityPtr : scene.getEntities()) {
        if (entityPtr != nullptr && entityPtr->hasScript()) {
            scriptSystem.detachFromEntity(*entityPtr);
        }
    }

    scene.clear();
    particleEmitterRegistry.clear();
    particleSystem.clear();
    dataGridRegistry.clear();
    selectionManager.clear();
    topDownControllerSystem->clearControlledEntity();

    cameraFollowState = CameraFollowState{};
    camera.clearTargetPosition();
    camera.setPosition({0.0f, 0.0f});
    postProcessSettings = PostProcessSettings{};
}

void Application::performPendingSessionReset() {
    std::string targetBaseScreenId;
    if (!screenFlowSystem->consumeSessionResetRequest(targetBaseScreenId))
        return;

    clearGameplaySession();

    if (!bootstrapGameplaySession()) {
        VG_ERROR("Failed to rebuild gameplay session during reset.");
        return;
    }

    if (!screenFlowSystem->showBase(targetBaseScreenId)) {
        VG_ERROR("Failed to show base screen '" + targetBaseScreenId + "' after reset.");
    }
}

bool Application::loadMapIntoScene(const std::string& mapPath) {
    auto map = std::make_unique<TileMap>();
    if (!map->loadFromFile(mapPath, assetManager))
        return false;

    scene.setTileMap(std::move(map));
    return true;
}

bool Application::performMapWarp(const std::string& mapPath, const std::string& spawnName) {
    struct PersistedEntityState {
        std::string id;
        std::string definitionId;
        PropertyBag customData;
    };

    std::vector<PersistedEntityState> persisted;
    for (const auto& entityPtr : scene.getEntities()) {
        if (entityPtr == nullptr)
            continue;

        const CustomValue* persistValue = entityPtr->getCustomData().get("persist_across_maps");
        const bool shouldPersist = persistValue != nullptr &&
            std::holds_alternative<bool>(*persistValue) &&
            std::get<bool>(*persistValue);

        if (shouldPersist) {
            PersistedEntityState state;
            state.id = entityPtr->getID();
            state.customData = entityPtr->getCustomData();

            if (const CustomValue* defValue = entityPtr->getCustomData().get("entity_definition");
                defValue != nullptr && std::holds_alternative<std::string>(*defValue)) {
                state.definitionId = std::get<std::string>(*defValue);
            }

            persisted.push_back(std::move(state));
        }

        if (entityPtr->hasScript()) {
            scriptSystem.callEntityOnDestroy(*entityPtr);
            scriptSystem.detachFromEntity(*entityPtr);
        }
    }

    scriptSystem.cancelAllTasks();
    scene.clear();
    selectionManager.clear();
    topDownControllerSystem->clearControlledEntity();

    if (!loadMapIntoScene(mapPath))
        return false;

    if (scene.getTileMap() != nullptr) {
        entityFactory->spawnFromMapObjects(scene.getTileMap()->getData(), "Entities");
    }

    for (const auto& state : persisted) {
        Entity* entity = scene.findEntityByID(state.id);
        if (entity == nullptr && !state.definitionId.empty()) {
            entity = entityFactory->spawnFromDefinition(
                state.id,
                "scripts/entities/" + state.definitionId + ".lua",
                {0.0f, 0.0f}
            );
        }

        if (entity != nullptr) {
            entity->getCustomData() = state.customData;
        }
    }

    if (scene.getTileMap() != nullptr && !spawnName.empty()) {
        glm::vec2 spawnPos;
        if (tryFindPointObjectWorldPosition(scene.getTileMap()->getData(), spawnName, spawnPos)) {
            if (Entity* player = scene.findEntityByID("player")) {
                if (player->hasPhysicsBody()) {
                    const glm::vec2 boundPos = spawnPos + player->getBoundsOffset();
                    const glm::vec2 centre = boundPos + player->getBoundsSize() * 0.5f;
                    scene.getPhysicsWorld().setBodyPositionPixels(player->getPhysicsBody(), centre);
                } else {
                    player->transform.position = spawnPos;
                }
            }
        }
    }

    topDownControllerSystem->clearControlledEntity();
    topDownControllerSystem->attachFirstConfiguredEntity();
    return true;
}

void Application::performPendingMapWarp() {
    std::string mapPath;
    std::string spawnName;
    if (!screenFlowSystem->consumeMapWarpRequest(mapPath, spawnName))
        return;

    if (!performMapWarp(mapPath, spawnName)) {
        VG_ERROR("Failed to warp to map '" + mapPath + "'");
    }
}

void Application::init() {
    window.init(initialWindowWidth, initialWindowHeight);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set GLFW callbacks for input and window events
    glfwSetWindowUserPointer(window.getHandle(), this);
    glfwSetErrorCallback(errorCallback);
    glfwSetKeyCallback(window.getHandle(), keyCallback);
    glfwSetWindowSizeCallback(window.getHandle(), windowSizeCallback);
    glfwSetMouseButtonCallback(window.getHandle(), mouseButtonCallback);
    glfwSetCursorPosCallback(window.getHandle(), mouseMoveCallback);
    glfwSetScrollCallback(window.getHandle(), mouseScrollCallback);

    uiLayer.init(window.getHandle());

    clearColour = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    debugMode = false;

    renderer.init();
    textRenderer.init();
    postProcessPass.init();

    sceneFrameBuffer = std::make_unique<FrameBuffer>();
    sceneFrameBuffer->createColor(initialWindowWidth, initialWindowHeight);

    audioEngine.init();
    audioSystem = std::make_unique<AudioSystem>(audioEngine);
    scriptSystem.init();

    // ------------ PROJECT CONTENT BOOTSTRAPING
    contentLoader = std::make_unique<ContentLoader>(assetManager,
        animationRegistry,
        particlePresetRegistry,
        scriptSystem,
        *audioSystem);
    // TODO: stop hardcoding these paths
    contentLoader->loadAssets("scripts/assets.lua");
    contentLoader->loadAnimations("scripts/animations.lua");
    contentLoader->loadParticlePresets("scripts/emitters.lua");

    screenFlowSystem = std::make_unique<ScreenFlowSystem>(uiSystem, scriptSystem);

    // Call before registering entities so they have scene context in scripts
    scriptSystem.setRuntimeContext({
        &scene,
        entityFactory.get(),
        &animationRegistry,
        &input,
        &camera,
        &particleSystem,
        &particlePresetRegistry,
        &particleEmitterRegistry,
        &assetManager,
        &uiSystem,
        &window,
        audioSystem.get(),
        screenFlowSystem.get(),
        &cameraFollowState,
        &postProcessSettings,
        &dataGridRegistry,
        &dataListRegistry
    });

    // Load and Register screens
    std::vector<ScreenDefinition> screenDefinitions;
    if (!scriptSystem.loadScreenDefinitions("scripts/screens.lua", screenDefinitions)) {
        VG_ERROR("Failed to load screen definitions from scripts/screens.lua");
    }

    if (!screenFlowSystem->loadScreens(screenDefinitions)) {
        VG_ERROR("Failed to initialise screen flow system.");
    }

    // Register entities
    entityFactory = std::make_unique<EntityFactory>(scene, assetManager, scriptSystem, animationRegistry);
    // NOTE: Important to set runtime entity factory before loading entities
    scriptSystem.setRuntimeEntityFactory(entityFactory.get());

    // -------- SCENE SETUP/BOOTSTRAPPING --------
    topDownControllerSystem = std::make_unique<TopDownControllerSystem>(scene, input, animationRegistry);
    if (!bootstrapGameplaySession()) {
        VG_ERROR("Failed to bootstrap initial gameplay session.");
    }

    engineEditor = std::make_unique<EngineEditor>(EngineEditorContext{
        .window = window,
        .time = time,
        .camera = camera,
        .scene = scene,
        .selectionManager = selectionManager,
        .assetManager = assetManager,
        .particleSystem = particleSystem,
        .particlePresetRegistry = particlePresetRegistry,
        .particleEmitterRegistry = particleEmitterRegistry,
        .clearColour = clearColour,
        .showPhysicsDebug = showPhysicsDebug,
        .selectionManagerEnabled = selectionManagerEnabled,
        .cameraFollowState = cameraFollowState,
        .postVignetteStrength = postProcessSettings.vignetteStrength,
        .postContrast = postProcessSettings.contrast,
        .postBrightness = postProcessSettings.brightness,
        .postSaturation = postProcessSettings.saturation,
        .postTint = postProcessSettings.tint,
        .postFadeColor = postProcessSettings.fadeColor,
        .postFadeAmount = postProcessSettings.fadeAmount,
        .entityFactory = *entityFactory
    });
    engineEditor->registerPanels(uiLayer);

    uiRenderer = std::make_unique<UIRenderer>(renderer);
}

void Application::run() {
    time.init(glfwGetTime());

    while (!window.shouldClose()) {
        time.update(glfwGetTime());

        input.beginFrame(); // Reset input
        glfwPollEvents();

        update(time.getDeltaTime());

        render(time.getDeltaTime());
    }
}

void Application::update(float dt) {
    int display_w, display_h;
    glfwGetFramebufferSize(window.getHandle(), &display_w, &display_h);
    camera.setViewportSize((float)display_w, (float)display_h);

    const double scrollY = input.getScrollY();
    if (scrollY != 0.0f) {
        const float zoomPerStep = 1.095f;
        const float factor = std::pow(zoomPerStep, static_cast<float>(scrollY));
        camera.setZoom(camera.getZoom() * factor);
    }

    if ((input.isKeyPressed(GLFW_KEY_GRAVE_ACCENT)))
        debugMode = !debugMode;

    uiSystem.updateScreenInteraction(input, textRenderer, assetManager, display_w, display_h);
    screenFlowSystem->update(dt);
    if (!screenFlowSystem->isGameplayPaused()) {
        topDownControllerSystem->update(dt);
        scene.update(dt);
        for (const auto& entityPtr : scene.getEntities()) {
            if (entityPtr && entityPtr->hasScript()) {
                scriptSystem.callEntityOnUpdate(*entityPtr, dt);
            }
        }

        for (const std::string& id : scene.getPendingDestroyedEntityIds()) {
            if (Entity* entity = scene.findEntityByID(id)) {
                if (entity->hasScript()) {
                    scriptSystem.callEntityOnDestroy(*entity);
                    scriptSystem.detachFromEntity(*entity);
                }
            }

            scene.eraseEntityImmediately(id);
        }
        scene.clearPendingDestroyedEntityIds();
    }
    scriptSystem.updateTasks(dt);
    
    if (cameraFollowState.followEntity) {
        Entity* target = scene.findEntityByID(cameraFollowState.targetEntityId);
        if (target != nullptr) {
            camera.setTargetPosition(target->transform.position + target->transform.scale * 0.5f);
        } else {
            camera.clearTargetPosition();
        }
    }
    camera.updateTarget();

    audioEngine.setListenerPosition(camera.getPosition());
    audioSystem->update();

    particleSystem.update(dt);
    // Only update selected entities/tiles when not using engine editor tools
    if (!uiLayer.wantsMouseCapture() && selectionManagerEnabled) {
        selectionManager.update(input, camera, scene);
    }

    performPendingMapWarp();

    performPendingSessionReset();
}

void Application::render(float dt) {
    int display_w, display_h;
    glfwGetFramebufferSize(window.getHandle(), &display_w, &display_h);

    if (sceneFrameBuffer) {
        sceneFrameBuffer->rescale(display_w, display_h);
        sceneFrameBuffer->bind();
    }

    glViewport(0, 0, display_w, display_h);
    glClearColor(clearColour.x * clearColour.w, clearColour.y * clearColour.w, clearColour.z * clearColour.w, clearColour.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    uiLayer.begin();
    if (debugMode)
        uiLayer.render();

    camera.setViewportSize((float)display_w, (float)display_h);
    renderer.begin(camera);

    // Main Scene Pass
    scene.render(renderer, camera, display_w, display_h);

    if (showPhysicsDebug) {
        scene.drawPhysicsDebug(renderer);
    }

    particleSystem.draw(renderer);
    selectionManager.draw(renderer, scene);

    // UI RENDER 1st Pass(World Space)
    uiRenderer->beginWorld(camera); // renderer begin already called with camera in main pass
    uiSystem.drawWorld(*uiRenderer, textRenderer, assetManager, camera);
    uiRenderer->end();

    // POST PROCESS Pass
    sceneFrameBuffer->unbind();
    glViewport(0, 0, display_w, display_h);
    glClear(GL_COLOR_BUFFER_BIT);
    postProcessPass.draw(
        sceneFrameBuffer->getColorTexture(),
        postProcessSettings.vignetteStrength,
        postProcessSettings.contrast,
        postProcessSettings.brightness,
        postProcessSettings.saturation,
        postProcessSettings.tint,
        postProcessSettings.fadeColor,
        postProcessSettings.fadeAmount
    );

    // UI RENDER 2nd Pass(Screen Space UI) - AFTER post fx so it stays clean
    uiRenderer->beginScreen(display_w, display_h);
    renderer.begin(uiRenderer->getScreenCamera());
    uiSystem.drawScreen(*uiRenderer, textRenderer, assetManager, display_w, display_h);
    uiRenderer->end();
    renderer.end();

    // ImGui/Editor pass
    uiLayer.end();
    window.swapBuffers();
}