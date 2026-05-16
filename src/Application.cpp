#include "Application.hpp"

#include "ErrorHandler.hpp"
#include "Logger.hpp"
#include "Input.hpp"

#include "TileMap.hpp"
#include "EntityFactory.hpp"

#include "game/FarmBindings.hpp"

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

    //Custom game bindings registration
    registerFarmBindings(scriptSystem.getState(), farmWorldState);

    // ------------ PROJECT CONTENT BOOTSTRAPING
    contentLoader = std::make_unique<ContentLoader>(assetManager,
        animationRegistry,
        particlePresetRegistry,
        scriptSystem,
        *audioSystem);
    // TODO: stop hardcoding these paths and map below!
    contentLoader->loadAssets("scripts/assets.lua");
    contentLoader->loadAnimations("scripts/animations.lua");
    contentLoader->loadParticlePresets("scripts/emitters.lua");

    // -------- SCENE SETUP --------
    auto map = std::make_unique<TileMap>();
    map->loadFromFile("assets/farmMap.tmx", assetManager);
    scene.setTileMap(std::move(map));

    screenFlowSystem = std::make_unique<ScreenFlowSystem>(uiSystem, scriptSystem);

    // Call before registering entities so they have scene context in scripts
    scriptSystem.setRuntimeContext({
        &scene,
        &animationRegistry,
        &input,
        &camera,
        &particleEmitterRegistry,
        &assetManager,
        &uiSystem,
        audioSystem.get(),
        screenFlowSystem.get(),
        &cameraFollowState,
        &postFadeAmount
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
    if (scene.getTileMap() != nullptr)
        entityFactory->spawnFromMapObjects(scene.getTileMap()->getData(), "Entities");

    engineEditor = std::make_unique<EngineEditor>(EngineEditorContext{
        .window = window,
        .time = time,
        .camera = camera,
        .scene = scene,
        .selectionManager = selectionManager,
        .assetManager = assetManager,
        .particleSystem = particleSystem,
        .particleEmitterRegistry = particleEmitterRegistry,
        .clearColour = clearColour,
        .showPhysicsDebug = showPhysicsDebug,
        .selectionManagerEnabled = selectionManagerEnabled,
        .cameraFollowState = cameraFollowState,
        .postVignetteStrength = postVignetteStrength,
        .postContrast = postContrast,
        .postBrightness = postBrightness,
        .postSaturation = postSaturation,
        .postTint = postTint,
        .postFadeColor = postFadeColor,
        .postFadeAmount = postFadeAmount,
        .entityFactory = *entityFactory
    });
    engineEditor->registerPanels(uiLayer);

    // Setup optional controller system by trying to find controller config
    topDownControllerSystem = std::make_unique<TopDownControllerSystem>(scene, input, animationRegistry);
    if (!topDownControllerSystem->attachFirstConfiguredEntity()) {
        VG_INFO("No entity with top-down controller config found.");
    }

    //-------------- Custom Scene Setup Code
    // TODO: move custom scene setup to scripting
    camera.setZoom(4.0f); 
    // Custom global automation/task/coroutine test
    scriptSystem.runGlobalScriptFunction("scripts/automations/intro.lua", "start");

    // Initialise farm grid
    if (scene.getTileMap() != nullptr) {
        const TileMapData& mapData = scene.getTileMap()->getData();
        farmWorldState.init(mapData.width, mapData.height);
    }

    //-------------- Particle emitter initialisation
    particleEmitterRegistry.createEmitterFromPreset(
        "blood_0",
        "blood",
        particleSystem,
        particlePresetRegistry,
        assetManager
    );
    particleEmitterRegistry.createEmitterFromPreset(
        "crates_0",
        "crate_burst",
        particleSystem,
        particlePresetRegistry,
        assetManager
    );

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

    screenFlowSystem->update(dt);
    if (!screenFlowSystem->isGameplayPaused()) {
        topDownControllerSystem->update(dt);
        scene.update(dt);
        for (const auto& entityPtr : scene.getEntities()) {
            if (entityPtr && entityPtr->hasScript()) {
                scriptSystem.callEntityOnUpdate(*entityPtr, dt);
            }
        }
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

    // UI RENDER 2nd Pass(Screen Space UI)
    uiRenderer->beginScreen(display_w, display_h);
    renderer.begin(uiRenderer->getScreenCamera());
    uiSystem.drawScreen(*uiRenderer, textRenderer, assetManager, display_w, display_h);
    uiRenderer->end();

    renderer.end();

    // POST PROCESS Pass
    sceneFrameBuffer->unbind();
    glViewport(0, 0, display_w, display_h);
    glClear(GL_COLOR_BUFFER_BIT);
    postProcessPass.draw(
        sceneFrameBuffer->getColorTexture(),
        postVignetteStrength,
        postContrast,
        postBrightness,
        postSaturation,
        postTint,
        postFadeColor,
        postFadeAmount
    );

    // ImGui/Editor pass
    uiLayer.end();
    window.swapBuffers();
}