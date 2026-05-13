#include "EngineEditor.hpp"

#include "ImGuiLayer.hpp"

#include "Window.hpp"
#include "Time.hpp"
#include "Camera2D.hpp"
#include "Scene.hpp"
#include "SelectionManager.hpp"
#include "AssetManager.hpp"
#include "ParticleSystem.hpp"
#include "ParticleEmitterRegistry.hpp"
#include "Entity.hpp"
#include "TileMap.hpp"

#include "EntityFactory.hpp"

EngineEditor::EngineEditor(EngineEditorContext context) : context{context} {}

void EngineEditor::registerPanels(ImGuiLayer& uiLayer) {
    uiLayer.addPanel("Renderer", [this]() {
        ImGui::ColorEdit4("Clear Color", (float*)&context.clearColour);
        ImGui::Text("FPS: %.1f", context.time.getFPS());
        ImGui::Text("Frame: %.3f ms", context.time.getFrameTimeMs());

        bool fullscreen = context.window.isFullscreen();
        if (ImGui::Checkbox("Fullscreen", &fullscreen)) {
            context.window.setFullscreen(fullscreen);
        }
    });

    uiLayer.addPanel("Camera", [this]() {
        glm::vec2 position = context.camera.getPosition();
        float zoom = context.camera.getZoom();

        if (ImGui::DragFloat2("Position", &position.x, 1.0f)) {
            context.camera.setPosition(position);
        }

        if (ImGui::SliderFloat("Zoom", &zoom, 1.0f, 8.0f)) {
            context.camera.setZoom(zoom);
        }
        ImGui::Checkbox("Follow Player", &context.cameraFollowPlayer);
    });

    uiLayer.addPanel("Hierarchy", [this]() {
        ImGui::Text("Entities: %d", context.scene.getEntityCount());
        ImGui::Separator();

        const std::string& selectedId = context.selectionManager.getSelectedEntityId();

        for (const auto& entityPtr : context.scene.getEntities()) {
            if (entityPtr == nullptr)
                continue;

            const bool isSelected = (selectedId == entityPtr->getID());
            if (ImGui::Selectable(entityPtr->getID().c_str(), isSelected)) {
                context.selectionManager.setSelectedEntityId(entityPtr->getID());
                context.selectionManager.clearSelectedTile();
            }
        }

        ImGui::Separator();

        static float spawnPos[2] = {200.0f, 200.0f};
        ImGui::DragFloat2("Spawn Position", spawnPos, 1.0f);

        if (ImGui::Button("Spawn Slime")) {
            spawnSlime({spawnPos[0], spawnPos[1]});
        }

        if (ImGui::Button("Spawn Empty Entity")) {
            spawnEmptyEntity({spawnPos[0], spawnPos[1]});
        }
    });

    uiLayer.addPanel("Inspector", [this]() {
        const std::string& selectedId = context.selectionManager.getSelectedEntityId();
        Entity* entity = context.scene.findEntityByID(selectedId);

        if (entity != nullptr) {
            ImGui::Text("Entity: %s", selectedId.c_str());
            ImGui::Separator();

            glm::vec2 pos = entity->transform.position;
            glm::vec2 scale = entity->transform.scale;
            glm::vec2 boundsOffset = entity->getBoundsOffset();
            glm::vec2 boundsSize = entity->getBoundsSize();

            if (ImGui::DragFloat2("Position", &pos.x, 1.0f)) {
                if (entity->hasPhysicsBody()) {
                    const glm::vec2 updatedBoundsSize = entity->getBoundsSize();
                    const glm::vec2 boundsPos = pos + entity->getBoundsOffset();
                    const glm::vec2 centre = boundsPos + updatedBoundsSize * 0.5f;
                    context.scene.getPhysicsWorld().setBodyPositionPixels(entity->getPhysicsBody(), centre);
                } else {
                    entity->transform.position = pos;
                }
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

            ImGui::Separator();
            ImGui::Text("Physics: %s", entity->hasPhysicsBody() ? "Yes" : "No");
            ImGui::Text("Script: %s", entity->hasScript() ? entity->getScriptName().c_str() : "None");
            ImGui::Text("Animated Sprite: %s", entity->getAnimatedSprite() ? "Yes" : "No");
            ImGui::Text("Sprite: %s", entity->getSprite() ? "Yes" : "No");
        } else {
            const glm::ivec2 selectedTile = context.selectionManager.getSelectedTile();

            if (selectedTile.x >= 0 && selectedTile.y >= 0) {
                ImGui::Text("Tile: %d, %d", selectedTile.x, selectedTile.y);

                if (TileMap* map = context.scene.getTileMap()) {
                    const glm::vec2 worldPos = map->tileToWorld(selectedTile.x, selectedTile.y);
                    ImGui::Text("World Position: %.1f, %.1f", worldPos.x, worldPos.y);
                    ImGui::Text("Tile Size: %d x %d", map->getTileWidth(), map->getTileHeight());
                }
            } else {
                ImGui::Text("Nothing selected");
            }
        }
    });


    uiLayer.addPanel("Assets", [this]() {
        ImGui::Text("Textures: %d", static_cast<int>(context.assetManager.getTextureIDs().size()));

        ImGui::Separator();
        ImGui::Text("Fonts: %d", static_cast<int>(context.assetManager.getFontIDs().size()));

        ImGui::Separator();
        ImGui::Text("Shaders: %d", static_cast<int>(context.assetManager.getShaderIDs().size()));
        for (const auto& id : context.assetManager.getShaderIDs()) {
            ImGui::BulletText("%s", id.c_str());
        }
    });
    uiLayer.addPanel("Physics", [this]() {
        ImGui::Checkbox("Show Physics Debug", &context.showPhysicsDebug);
        ImGui::Text("Static Bodies: %d", context.scene.getPhysicsWorld().getStaticBodyCount());
    });
    uiLayer.addPanel("Particles", [this]() {
        ImGui::Text("Emitters: %d", static_cast<int>(context.particleSystem.getEmitterCount()));

        ParticleEmitter* bloodEmitter = context.particleEmitterRegistry.getEmitter("blood_0");
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
        ImGui::Checkbox("Selection manager enabled", &context.selectionManagerEnabled);
        const glm::ivec2 hoverTile = context.selectionManager.getHoveredTile();
        const glm::ivec2 selectedTile = context.selectionManager.getSelectedTile();

        ImGui::Text("Hovered Tile: %d, %d", hoverTile.x, hoverTile.y);
        ImGui::Text("Selected Tile: %d, %d", selectedTile.x, selectedTile.y);
    });
}

// Temporary method for testing
void EngineEditor::spawnSlime(const glm::vec2& position) {
    static int slimeIdPostfix = 0;
    context.entityFactory.spawnFromDefinition("slime_" + std::to_string(slimeIdPostfix++), "scripts/entities/slime.lua", position);
}

void EngineEditor::spawnEmptyEntity(const glm::vec2& position) {
    static int entityIdPostfix = 0;
    std::string entityId = "Entity_" + std::to_string(entityIdPostfix++);
    Entity &entity = context.scene.createEntity(entityId);
}