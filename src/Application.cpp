#include "Application.hpp"

#include "ErrorHandler.hpp"
#include "Logger.hpp"
#include "Input.hpp"

#include "TileMap.hpp"
#include "Sprite.hpp"
#include "AnimatedSprite.hpp"

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

void Application::init() {
    window.init(1024, 768);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


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

    // --------- DEBUG PANEL INIT
    uiLayer.addPanel("Renderer", [this]() {
        ImGui::ColorEdit4("Clear Color", (float*)&clearColour);
        ImGui::Text("FPS: %.1f", time.getFPS());
        ImGui::Text("Frame: %.3f ms", time.getFrameTimeMs());

        bool fullscreen = window.isFullscreen();
        if (ImGui::Checkbox("Fullscreen", &fullscreen)) {
            window.setFullscreen(fullscreen);
        }
    });

    uiLayer.addPanel("Camera", [this]() {
        glm::vec2 position = camera.getPosition();
        float zoom = camera.getZoom();

        if (ImGui::DragFloat2("Position", &position.x, 1.0f)) {
            camera.setPosition(position);
        }

        if (ImGui::SliderFloat("Zoom", &zoom, 1.0f, 8.0f)) {
            camera.setZoom(zoom);
        }
    });

    uiLayer.addPanel("Scene", [this]() {
        ImGui::Text("Entities: %d", scene.getEntityCount());
        const std::string& selectedId = selectionManager.getSelectedEntityId();
        if (selectedId.empty()) {
            ImGui::Text("No entity selected");
        } else {
            ImGui::Text("Selected entity: %s", selectedId.c_str());
        }
        Entity* entity = scene.findEntityByID(selectedId);
        if (entity == nullptr) {
            ImGui::Text("Selected entity not found");
        } else {
            glm::vec2 pos = entity->transform.position;
            glm::vec2 scale = entity->transform.scale;
            glm::vec2 boundsOffset = entity->getBoundsOffset();
            glm::vec2 boundsSize = entity->getBoundsSize();

            if (ImGui::DragFloat2("Position", &pos.x, 1.0f)) {
                entity->transform.position = pos;
            }
            if (ImGui::DragFloat2("Scale", &scale.x, 1.0f)) {
                entity->transform.scale = scale;
            }
            if (ImGui::DragFloat2("Bounding Box Offset", &boundsOffset.x, 1.0f)) {
                entity->setBounds(boundsOffset, boundsSize);
            }
            if (ImGui::DragFloat2("Bounding Box Size", &boundsSize.x, 1.0f)) {
                entity->setBounds(boundsOffset, boundsSize);
            }

            ImGui::Text("Physics: %s", entity->hasPhysicsBody() ? "Yes" : "No");
        }
        ImGui::Separator();

        static float spawnPos[2] = {200.0f, 200.0f};
        ImGui::DragFloat2("Entity spawn position", spawnPos, 1.0f);
        if (ImGui::Button("Spawn Slime")) {
            spawnSlime({spawnPos[0], spawnPos[1]});
        }
        if (ImGui::Button("Spawn Empty Entity")) {
            spawnEmptyEntity({spawnPos[0], spawnPos[1]});
        }
    });

    uiLayer.addPanel("Assets", [this]() {
        ImGui::Text("Textures: %d", static_cast<int>(assetManager.getTextureIDs().size()));
        for (const auto& id : assetManager.getTextureIDs()) {
            ImGui::BulletText("%s", id.c_str());
        }

        ImGui::Separator();

        ImGui::Text("Shaders: %d", static_cast<int>(assetManager.getShaderIDs().size()));
        for (const auto& id : assetManager.getShaderIDs()) {
            ImGui::BulletText("%s", id.c_str());
        }
    });
    uiLayer.addPanel("Physics", [this]() {
        ImGui::Checkbox("Show Physics Debug", &showPhysicsDebug);
        ImGui::Text("Static Bodies: %d", scene.getPhysicsWorld().getStaticBodyCount());
    });
    uiLayer.addPanel("Particles", [this]() {
        ImGui::Text("Emitters: %d", static_cast<int>(particleSystem.getEmitterCount()));

        if (bloodEmitter) {
            bool enabled = bloodEmitter->isEnabled();
            if (ImGui::Checkbox("Blood particle enabled", &enabled)) {
                bloodEmitter->setEnabled(enabled);
            }

            glm::vec4 color = bloodEmitter->getBaseColor();
            if (ImGui::ColorEdit4("Particle color", &color.x)) {
                bloodEmitter->setBaseColor(color);
            }

            glm::vec2 velocity = bloodEmitter->getBaseVelocity();
            if (ImGui::DragFloat2("Base velocity", &velocity.x, 1.0f)) {
                bloodEmitter->setBaseVelocity(velocity);
            }

            float size = bloodEmitter->getBaseSize();
            if (ImGui::SliderFloat("Size", &size, 1.0f, 8.0f)) {
                bloodEmitter->setBaseSize(size);
            }

            float lifetime = bloodEmitter->getBaseLifetime();
            if (ImGui::SliderFloat("Lifetime", &lifetime, 1.0f, 8.0f)) {
                bloodEmitter->setBaseLifetime(lifetime);
            }
        }
    });
    uiLayer.addPanel("Selection", [this]() {
        const glm::ivec2 hoverTile = selectionManager.getHoveredTile();
        const glm::ivec2 selectedTile = selectionManager.getSelectedTile();

        ImGui::Text("Hovered Tile: %d, %d", hoverTile.x, hoverTile.y);
        ImGui::Text("Selected Tile: %d, %d", selectedTile.x, selectedTile.y);
    });
    // --------------------------

    renderer.init();

    // ------------ RESOURCES INIT
    assetManager.loadTexture("player", "assets/textures/player.png");
    assetManager.loadTexture("slime", "assets/textures/slime.png");

    // -------- SCENE INIT --------
    auto map = std::make_unique<TileMap>();
    map->loadFromFile("assets/farmMap.tmx", assetManager);
    scene.setTileMap(std::move(map));

    // ------------ LUA SCRIPTING TEST
    scriptSystem.init();

    // Register animations
    std::string slimeJumpPath = "scripts/animations/slime_jump.lua";
    animationRegistry.loadClip("slime_jump", slimeJumpPath, assetManager, scriptSystem);
    std::string slimeIdlePath = "scripts/animations/slime_idle.lua";
    animationRegistry.loadClip("slime_idle", slimeIdlePath, assetManager, scriptSystem);
    std::string slimeDiePath = "scripts/animations/slime_death.lua";
    animationRegistry.loadClip("slime_death", slimeDiePath, assetManager, scriptSystem);

    std::string playerIdlePath = "scripts/animations/player_idle.lua";
    animationRegistry.loadClip("player_idle", playerIdlePath, assetManager, scriptSystem);
    std::string playerWalkUpPath = "scripts/animations/player_walk_up.lua";
    animationRegistry.loadClip("player_walk_up", playerWalkUpPath, assetManager, scriptSystem);
    std::string playerWalkDownPath = "scripts/animations/player_walk_down.lua";
    animationRegistry.loadClip("player_walk_down", playerWalkDownPath, assetManager, scriptSystem);
    std::string playerWalkRightPath = "scripts/animations/player_walk_right.lua";
    animationRegistry.loadClip("player_walk_right", playerWalkRightPath, assetManager, scriptSystem);

    // Register entities
    entityFactory = std::make_unique<EntityFactory>(scene, assetManager, scriptSystem, animationRegistry);
    if (scene.getTileMap() != nullptr)
        entityFactory->spawnFromMapObjects(scene.getTileMap()->getData(), "Entities");

    // Setup controller system by providing controller config
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

    //-------------- TEST CODE
    camera.setZoom(4.0f); // set appropriate zoom for current game im working, probably better to be configured or scripted
    boxTexture = assetManager.loadTexture("crate", "assets/textures/crate.png");

    // Particle system test
    bloodEmitter = &particleSystem.createEmitter();
    bloodEmitter->init(512, TextureRegion::full(nullptr));
    bloodEmitter->setBaseColor({0.8f, 0.1f, 0.1f, 0.9f});
    bloodEmitter->setBaseSize(10.0f);
    bloodEmitter->setBaseLifetime(0.6f);
    bloodEmitter->setBaseVelocity({0.0f, 50.0f});
    bloodEmitter->setVelocityVariance({80.0f, 80.0f});

    textureEmitter = &particleSystem.createEmitter();
    textureEmitter->init(256, TextureRegion::full(boxTexture));
    textureEmitter->setBaseColor({1.0f, 1.0f, 1.0f, 0.9f});
    textureEmitter->setBaseSize(14.0f);
    textureEmitter->setBaseLifetime(1.0f);
    textureEmitter->setBaseVelocity({0.0f, 30.0f});
    textureEmitter->setVelocityVariance({50.0f, 50.0f});
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

    // TODO: already regretting not getting variadic argument support from Villain Logger
    // VA_DEBUG("Mouse X:  %s", x)

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

    Entity *player = scene.findEntityByID("player");
    if (player != nullptr)
        // Centre camera on entity's centre
        camera.setPosition(player->transform.position + player->transform.scale * 0.5f);

    particleSystem.update(dt);
    // Only update selected entities/tiles when not using engine editor tools
    // Also same for emitting particles
    if (!uiLayer.wantsMouseCapture()) {
        selectionManager.update(input, camera, scene);

        glm::vec2 mouseWorld = camera.screenToWorld({
        static_cast<float>(input.getMouseX()),
        static_cast<float>(input.getMouseY())
        });

        if (input.isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
            if (bloodEmitter) {
                bloodEmitter->emit(mouseWorld, 20);
            }
        }
        if (input.isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
            if (textureEmitter) {
                textureEmitter->emit(mouseWorld, 12);
            }
        }
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

    // Main scene render
    scene.render(renderer, camera, display_w, display_h);

    if (showPhysicsDebug) {
        scene.drawPhysicsDebug(renderer);
    }

    particleSystem.draw(renderer);

    selectionManager.draw(renderer, scene);

    // TEST textured quad render code
    renderer.drawQuad({boxTexture, {0.0f, 0.0f}, {1.0f, 1.0f}}, {{0.0f, 0.0f}, {100.0f, 100.0f}, 0.0f});
    // TEST coloured quad render code
    renderer.drawQuad(
        {{{200.0f, 0.0f}, {100.0f, 100.0f}, 0.0f},
        TextureRegion::full(nullptr),
        {0.6f, 0.2f, 0.1f, 0.5f}}
    );

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
