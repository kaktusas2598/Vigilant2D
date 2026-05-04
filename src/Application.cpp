#include "Application.hpp"

#include "ErrorHandler.hpp"
#include "Logger.hpp"
#include "Input.hpp"

#include "TileMap.hpp"
#include "Sprite.hpp"
#include "AnimatedSprite.hpp"

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
    // ---------



    renderer.init();

    // -------- SCENE INIT --------
    auto map = std::make_unique<TileMap>();
    map->loadFromFile("assets/farmMap.tmx", assetManager);
    scene.setTileMap(std::move(map));

    playerTexture = assetManager.loadTexture("player", "assets/textures/player.png");
    slimeTexture = assetManager.loadTexture("slime", "assets/textures/slime.png");
    testIdleClip = AnimationClip("idle_down", true);
    for (int x = 0; x < 6; ++x) {
        testIdleClip.addFrame(makeRegionFromGrid(playerTexture, x, 0, 6, 10), 0.12f);
    }
    testWalkDownClip = AnimationClip("walk_down", true);
    for (int x = 0; x < 6; ++x) {
        testWalkDownClip.addFrame(makeRegionFromGrid(playerTexture, x, 3, 6, 10), 0.12f);
    }
    testWalkUpClip = AnimationClip("walk_up", true);
    for (int x = 0; x < 6; ++x) {
        testWalkUpClip.addFrame(makeRegionFromGrid(playerTexture, x, 2, 6, 10), 0.12f);
    }
    testWalkRightClip = AnimationClip("walk_right", true);
    for (int x = 0; x < 6; ++x) {
        testWalkRightClip.addFrame(makeRegionFromGrid(playerTexture, x, 4, 6, 10), 0.12f);
    }


    testSlimeClip = AnimationClip("testSlime", true);
    for (int x = 0; x < 7; ++x) {
        testSlimeClip.addFrame(makeRegionFromGrid(slimeTexture, x, 2, 7, 5), 0.12f);
    }

    Entity& playerEntity = scene.createEntity("player");
    playerEntity.transform.position = {150.0f, 150.0f};
    playerEntity.transform.scale = {48.0f, 48.0f};

    auto sprite = std::make_unique<Sprite>();
    auto animatedSprite = std::make_unique<AnimatedSprite>();
    animatedSprite->setSprite(sprite.get());
    animatedSprite->play(&testIdleClip);
    playerEntity.setSprite(std::move(sprite));
    playerEntity.setAnimatedSprite(std::move(animatedSprite));

    playerEntity.setBounds({20.0f, 5.0f}, {14.0f, 22.0f});
    playerEntity.setPhysicsBody(
        scene.getPhysicsWorld().createDynamicBox(
            playerEntity.getBoundsPosition(),
            playerEntity.getBoundsSize()
        )
    );

    Entity& slimeEntity = scene.createEntity("slime");
    slimeEntity.transform.position = {300.0f, 550.0f};
    slimeEntity.transform.scale = {48.0f, 48.0f};

    auto slimeSprite = std::make_unique<Sprite>();
    auto slimeAnimSprite = std::make_unique<AnimatedSprite>();
    slimeAnimSprite->setSprite(slimeSprite.get());
    slimeAnimSprite->play(&testSlimeClip);
    slimeEntity.setSprite(std::move(slimeSprite));
    slimeEntity.setAnimatedSprite(std::move(slimeAnimSprite));

    //-------------- TEST CODE
    camera.setZoom(4.0f);
    boxTexture = assetManager.loadTexture("crate", "assets/textures/crate.png");

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

    movePlayer();
    scene.update(dt);

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

void Application::movePlayer() {
    Entity *player = scene.findEntityByID("player");
    if (player != nullptr) {
        glm::vec2 movement(0.0f);
        const float playerSpeed = 120.0f;

        if (input.isKeyDown(GLFW_KEY_W)) movement.y += 1.0f;
        if (input.isKeyDown(GLFW_KEY_S)) movement.y -= 1.0f;
        if (input.isKeyDown(GLFW_KEY_A)) movement.x -= 1.0f;
        if (input.isKeyDown(GLFW_KEY_D)) movement.x += 1.0f;

        AnimatedSprite* anim = player->getAnimatedSprite();
        if (movement.x != 0.0f || movement.y != 0.0f) {
            movement = glm::normalize(movement);

            if (player->hasPhysicsBody()) {
                scene.getPhysicsWorld().setBodyLinearVelocityPixels(
                    player->getPhysicsBody(),
                    movement * playerSpeed
                );
            }
            // player->transform.position += movement * playerSpeed * dt;

            if (anim != nullptr) {
                if (std::abs(movement.x) > std::abs(movement.y)) {
                    if (movement.x > 0.0f) {
                        if (player->getSprite()) player->getSprite()->setFlipX(false);
                        anim->play(&testWalkRightClip, false);
                    } else {
                        if (player->getSprite()) player->getSprite()->setFlipX(true);
                        anim->play(&testWalkRightClip, false);
                    }
                } else {
                    if (movement.y > 0.0f) {
                        anim->play(&testWalkUpClip, false);
                    } else {
                        anim->play(&testWalkDownClip, false);
                    }

                }
            }
        } else {
            if (player->hasPhysicsBody()) {
                scene.getPhysicsWorld().setBodyLinearVelocityPixels(
                    player->getPhysicsBody(),
                    {0.0f, 0.0f}
                );
            }

            if (anim != nullptr) {
                anim->play(&testIdleClip, false);
            }
        }
    }
}

void Application::spawnSlime(const glm::vec2& position) {
    static int slimeIdPostfix = 0;
    std::string slimeId = "Slime_" + slimeIdPostfix++;
    Entity &slimeEntity = scene.createEntity(slimeId);
    slimeEntity.transform.position = position;
    slimeEntity.transform.scale = {48.0f, 48.0f};

    auto slimeSprite = std::make_unique<Sprite>();
    auto slimeAnimSprite = std::make_unique<AnimatedSprite>();
    slimeAnimSprite->setSprite(slimeSprite.get());
    slimeAnimSprite->play(&testSlimeClip);
    slimeEntity.setSprite(std::move(slimeSprite));
    slimeEntity.setAnimatedSprite(std::move(slimeAnimSprite));
}

void Application::spawnEmptyEntity(const glm::vec2& position) {
    static int entityIdPostfix = 0;
    std::string entityId = "Slime_" + entityIdPostfix++;
    Entity &entity = scene.createEntity(entityId);
}
