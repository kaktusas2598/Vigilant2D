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

    renderer.init();

    // -------- SCENE INIT --------
    auto map = std::make_unique<TileMap>();
    map->loadFromFile("assets/farmMap.tmx");
    scene.setTileMap(std::move(map));

    playerTexture = new Texture("assets/textures/player.png");
    testIdleClip = AnimationClip("idle_down", true);
    for (int x = 0; x < 6; ++x) {
        testIdleClip.addFrame(makeRegionFromGrid(playerTexture, x, 0, 6, 10), 0.12f);
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

    //-------------- TEST CODE
    camera.setPosition({320.0f, 200.0f});
    boxTexture = new Texture("assets/textures/crate.png");
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
    // TODO: already regretting not getting variadic argument support from Villain Logger
    // VA_DEBUG("Mouse X:  %s", x)

    float cameraSpeed = 500.0f * dt;

    // TEST camera
    if (input.isKeyDown(GLFW_KEY_W)) {
        camera.move({0.0f, cameraSpeed});
    } else if (input.isKeyDown(GLFW_KEY_S)) {
        camera.move({0.0f, -cameraSpeed});
    } else if (input.isKeyDown(GLFW_KEY_A)) {
        camera.move({-cameraSpeed, 0.0f});
    } else if (input.isKeyDown(GLFW_KEY_D)) {
        camera.move({cameraSpeed, 0.0f});
    }

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

    if (input.isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
        tileSelected = tileHover;
    }

    scene.update(dt);

    // Update however tile based on mouse
    TileMap* map = scene.getTileMap();
    if (map != nullptr) {
        glm::vec2 mouseScreen(
            static_cast<float>(input.getMouseX()),
            static_cast<float>(input.getMouseY())
        );

        glm::vec2 mouseWorld = camera.screenToWorld(mouseScreen);
        glm::ivec2 tile = map->worldToTile(mouseWorld);

        if (map->isTileInBounds(tile.x, tile.y)) {
            tileHover = tile;
        } else {
            tileHover = {-1, -1};
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
        uiLayer.render((float *)&clearColour, time.getFPS(), time.getFrameTimeMs());

    camera.setViewportSize((float)display_w, (float)display_h);
    renderer.begin(camera);

    // Main scene render
    scene.render(renderer, camera, display_w, display_h);

    // Tile overlay render
    TileMap* map = scene.getTileMap();
    if (map != nullptr && tileHover.x >= 0 && tileHover.y >= 0) {
        Transform2D hoverTransform;
        hoverTransform.position = map->tileToWorld(tileHover.x, tileHover.y);
        hoverTransform.scale = {
            static_cast<float>(map->getTileWidth()),
            static_cast<float>(map->getTileHeight())
        };

        renderer.drawQuad({
            hoverTransform,
            TextureRegion::full(nullptr),
            {0.2f, 0.9f, 0.3f, 0.35f}
        });

        if (tileSelected.x >= 0 && tileSelected.y >= 0) {
            Transform2D selectTransform;
            selectTransform.position = map->tileToWorld(tileSelected.x, tileSelected.y);
            selectTransform.scale = {
                static_cast<float>(map->getTileWidth()),
                static_cast<float>(map->getTileHeight())};

            renderer.drawQuad({selectTransform,
                               TextureRegion::full(nullptr),
                               {0.8f, 0.1f, 0.3f, 0.35f}});
        }
    }

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
