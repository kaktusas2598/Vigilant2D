#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "TextureRegion.hpp"
#include "UIPrimitives.hpp"
#include "UIStyle.hpp"
#include "glm/glm.hpp"

class UIRenderer;
class TextRenderer;
class AssetManager;
class Camera2D;
class Input;

struct UIScreenLayout {
    bool enabled = false;
    glm::vec2 anchor{0.0f, 0.0f}; // Where the widget is attached in the viewport.
    glm::vec2 pivot{0.0f, 0.0f}; // Which point on the widget matches the anchor.
};

struct UIButtonVisualState {
    glm::vec4 backgroundColor{0.12f, 0.12f, 0.14f, 0.95f};
    glm::vec4 borderColor{0.85f, 0.80f, 0.55f, 1.0f};
    glm::vec4 textColor{1.0f, 1.0f, 1.0f, 1.0f};
    glm::vec4 iconTint{1.0f, 1.0f, 1.0f, 1.0f};
};

// Lets scripts modify UI Widgets through records
struct UILabelRecord {
    std::string id;
    std::string group = "default";
    std::string fontId = "ui";

    std::string text;
    UIRenderSpace renderSpace = UIRenderSpace::Screen;
    glm::vec2 position{0.0f, 0.0f};
    UIScreenLayout screenLayout{};
    float scale = 1.0f;
    glm::vec4 textColor{1.0f, 1.0f, 1.0f, 1.0f};

    glm::vec2 padding{6.0f, 4.0f};
    bool backgroundEnabled = false;
    glm::vec4 backgroundColor{0.08f, 0.08f, 0.10f, 0.90f};

    bool borderEnabled = false;
    glm::vec4 borderColor{0.85f, 0.80f, 0.55f, 1.0f};

    bool visible = true;
    int order = 0;
};

struct UIButtonRecord {
    std::string id;
    std::string group = "default";
    std::string fontId = "ui";

    glm::vec2 position{0.0f, 0.0f};
    UIScreenLayout screenLayout{};
    glm::vec2 size{160.0f, 44.0f};

    std::string text;
    float textScale = 1.0f;

    bool iconEnabled = false;
    TextureRegion icon = TextureRegion::full(nullptr);
    glm::vec2 iconSize{16.0f, 16.0f};

    bool borderEnabled = true;
    bool visible = true;
    int order = 0;

    UIButtonVisualState normal{};
    UIButtonVisualState hovered{};
    UIButtonVisualState pressed{};

    bool hoveredNow = false;
    bool pressedNow = false;
    bool clicked = false;
};

struct UISlotStripItemRecord {
    bool occupied = false;
    TextureRegion icon = TextureRegion::full(nullptr);
    glm::vec4 tint{1.0f, 1.0f, 1.0f, 1.0f};
};

struct UISlotStripRecord {
    std::string id;
    std::string group = "default";

    UIRenderSpace renderSpace = UIRenderSpace::Screen;
    glm::vec2 position{0.0f, 0.0f};
    glm::vec2 slotSize{44.0f, 44.0f};
    UIStyle style{};

    int selectedIndex = 0;
    bool visible = true;
    int order = 0;

    std::vector<UISlotStripItemRecord> slots;
};

struct UIProgressBarRecord {
    std::string id;
    std::string group = "default";

    UIRenderSpace renderSpace = UIRenderSpace::Screen;
    glm::vec2 position{0.0f, 0.0f};
    glm::vec2 size{32.0f, 5.0f};
    glm::vec2 fillInset{1.0f, 1.0f};

    float minValue = 0.0f;
    float maxValue = 100.0f;
    float value = 100.0f;

    bool visible = true;
    int order = 0;

    glm::vec4 backgroundColor{0.15f, 0.15f, 0.15f, 0.95f};
    glm::vec4 fillColor{0.25f, 0.85f, 0.35f, 1.0f};
    glm::vec4 borderColor{0.0f, 0.0f, 0.0f, 0.0f};
    bool borderEnabled = false;
    float borderThickness = 1.0f;
};

// Retained UI System
class UISystem {
    public:
        UILabelRecord& createLabel(const std::string& id);
        UISlotStripRecord& createSlotStrip(const std::string& id);
        UIProgressBarRecord& createProgressBar(const std::string& id);
        UIButtonRecord& createButton(const std::string& id);

        UILabelRecord* getLabel(const std::string& id);
        const UILabelRecord* getLabel(const std::string& id) const;

        UISlotStripRecord* getSlotStrip(const std::string& id);
        const UISlotStripRecord* getSlotStrip(const std::string& id) const;

        UIProgressBarRecord* getProgressBar(const std::string& id);
        const UIProgressBarRecord* getProgressBar(const std::string& id) const;

        UIButtonRecord* getButton(const std::string& id);
        const UIButtonRecord* getButton(const std::string& id) const;

        void setGroupVisible(const std::string& group, bool visible);
        bool isGroupVisible(const std::string& group) const;

        void updateScreenInteraction(const Input& input,
                             TextRenderer& textRenderer,
                             AssetManager& assetManager,
                             int viewportWidth,
                             int viewportHeight);

        void clear();

        void drawScreen(UIRenderer& uiRenderer,
                        TextRenderer& textRenderer,
                        AssetManager& assetManager,
                        int viewportWidth, int viewportHeight) const;
        void drawWorld(UIRenderer& uiRenderer,
                        TextRenderer& textRenderer,
                        AssetManager& assetManager,
                        const Camera2D& camera) const;
    private:
        enum class WidgetType {
            Label,
            SlotStrip
        };

        struct DrawRef {
            WidgetType type;
            std::string id;
            int order = 0;
        };

        bool isWidgetVisible(const std::string& group, bool visible) const;

        void drawLabelWorldGeometry(const UILabelRecord& record,
                            UIRenderer& uiRenderer,
                            TextRenderer& textRenderer,
                            AssetManager& assetManager) const;
        void drawLabelWorldText(const UILabelRecord& record,
                        TextRenderer& textRenderer,
                        AssetManager& assetManager) const;

        void drawLabelScreenGeometry(const UILabelRecord& record,
                       UIRenderer& uiRenderer,
                       TextRenderer& textRenderer,
                       AssetManager& assetManager,
                       int viewportWidth, int viewportHeight) const;
        void drawLabelScreenText(const UILabelRecord& record,
                       TextRenderer& textRenderer,
                       AssetManager& assetManager,
                       int viewportWidth, int viewportHeight) const;

        void drawButtonScreenGeometry(const UIButtonRecord& record,
                        UIRenderer& uiRenderer,
                        int viewportWidth,
                        int viewportHeight) const;
        void drawButtonScreenText(const UIButtonRecord& record,
                        TextRenderer& textRenderer,
                        AssetManager& assetManager,
                        int viewportWidth,
                        int viewportHeight) const;
        void drawButtonScreenIcon(const UIButtonRecord& record,
                        UIRenderer& uiRenderer,
                        int viewportWidth,
                        int viewportHeight) const;

        void drawSlotStrip(const UISlotStripRecord& record, UIRenderer& uiRenderer) const;
        void drawProgressBar(const UIProgressBarRecord& record, UIRenderer& uiRenderer) const;

        std::unordered_map<std::string, UILabelRecord> labels;
        std::unordered_map<std::string, UISlotStripRecord> slotStrips;
        std::unordered_map<std::string, UIProgressBarRecord> progressBars;
        std::unordered_map<std::string, UIButtonRecord> buttons;
        std::string activeScreenButtonId;
        std::unordered_map<std::string, bool> groupVisibility;
};