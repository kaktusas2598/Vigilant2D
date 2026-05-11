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
    window.init(1024, 768);
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
        .cameraFollowPlayer = cameraFollowPlayer,
        .spawnSlime = [this](const glm::vec2& position) {
            spawnSlime(position);
        },
        .spawnEmptyEntity = [this](const glm::vec2& position) {
            spawnEmptyEntity(position);
        }
    });

    engineEditor->registerPanels(uiLayer);

    renderer.init();

    textRenderer.init();

    scriptSystem.init();
    //Custom game bindings registration
    registerFarmBindings(scriptSystem.getState(), farmWorldState);

    // ------------ PROJECT CONTENT BOOTSTRAPING
    contentLoader = std::make_unique<ContentLoader>(assetManager, animationRegistry, particlePresetRegistry, scriptSystem);
    // TODO: stop hardcoding these paths and map below!
    contentLoader->loadAssets("scripts/assets.lua");
    contentLoader->loadAnimations("scripts/animations.lua");
    contentLoader->loadParticlePresets("scripts/emitters.lua");

    // -------- SCENE SETUP --------
    auto map = std::make_unique<TileMap>();
    map->loadFromFile("assets/farmMap.tmx", assetManager);
    scene.setTileMap(std::move(map));

    // Iinitialise farm grid
    if (scene.getTileMap() != nullptr) {
        const TileMapData& mapData = scene.getTileMap()->getData();
        farmWorldState.init(mapData.width, mapData.height);
    }

    // Call before registering entities so they have scene context in scripts
    scriptSystem.setRuntimeContext({
        &scene,
        &animationRegistry,
        &input,
        &camera,
        &particleEmitterRegistry,
        &assetManager,
        &uiSystem
    });

    // Register entities
    entityFactory = std::make_unique<EntityFactory>(scene, assetManager, scriptSystem, animationRegistry);
    if (scene.getTileMap() != nullptr)
        entityFactory->spawnFromMapObjects(scene.getTileMap()->getData(), "Entities");

    // Setup controller system by providing controller config
    // TODO: set from script, similar for particle emitters
    topDownControllerSystem = std::make_unique<TopDownControllerSystem>(scene, input, animationRegistry);
    topDownControllerSystem->setControlledEntity({
        .entityId = "player",
        .moveSpeed = 120.0f,
        .idleAnimation = "player_idle",
        .walkUpAnimation = "player_walk_up",
        .walkDownAnimation = "player_walk_down",
        .walkRightAnimation = "player_walk_right",
        .allowFlipX = true
    });

    //-------------- Custom Scene Setup Code
    camera.setZoom(4.0f); // set appropriate zoom for current game im working, probably better to be configured or scripted

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

    //-------------- UI TEST
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

    if ((input.isKeyPressed(GLFW_KEY_GRAVE_ACCENT))) {
        debugMode = !debugMode;
        if (debugMode) VG_INFO("Debug Mode ON");
    }

    if (input.isKeyPressed(GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(window.getHandle(), GLFW_TRUE);
    }

    topDownControllerSystem->update(dt);
    scene.update(dt);
    for (const auto& entityPtr : scene.getEntities()) {
        if (entityPtr && entityPtr->hasScript()) {
            scriptSystem.callEntityOnUpdate(*entityPtr, dt);
        }
    }

    // TODO: probably best done from player on_update script func?
    Entity *player = scene.findEntityByID("player");
    if (cameraFollowPlayer && player != nullptr) {
        // Centre camera on entity's centre
        camera.setPosition(player->transform.position + player->transform.scale * 0.5f);
    }

    particleSystem.update(dt);
    // Only update selected entities/tiles when not using engine editor tools
    if (!uiLayer.wantsMouseCapture() && selectionManagerEnabled) {
        selectionManager.update(input, camera, scene);
    }
}

void Application::render(float dt) {
    int display_w, display_h;
    glfwGetFramebufferSize(window.getHandle(), &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clearColour.x * clearColour.w, clearColour.y * clearColour.w, clearColour.z * clearColour.w, clearColour.w);
    glClear(GL_COLOR_BUFFER_BIT);

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

    uiLayer.end();
    window.swapBuffers();
}

void Application::exit() {
}

void Application::switchDebugMode() {
    debugMode = !debugMode;
}

bool Application::isDebugModeEnabled() {
    return debugMode;
}

// Temporary method for testing
void Application::spawnSlime(const glm::vec2& position) {
    static int slimeIdPostfix = 0;
    entityFactory->spawnFromDefinition("slime_" + std::to_string(slimeIdPostfix++), "scripts/entities/slime.lua", position);
}

void Application::spawnEmptyEntity(const glm::vec2& position) {
    static int entityIdPostfix = 0;
    std::string entityId = "Entity_" + std::to_string(entityIdPostfix++);
    Entity &entity = scene.createEntity(entityId);
}
