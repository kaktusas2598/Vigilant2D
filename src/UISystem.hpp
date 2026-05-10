#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "TextureRegion.hpp"
#include "UIStyle.hpp"
#include "glm/glm.hpp"

class UIRenderer;
class TextRenderer;
class AssetManager;

// Lets scripts modify UI Widgets through records
struct UILabelRecord {
    std::string id;
    std::string group = "default";
    std::string fontId = "ui";

    std::string text;
    glm::vec2 position{0.0f, 0.0f};
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

struct UISlotStripItemRecord {
    bool occupied = false;
    TextureRegion icon = TextureRegion::full(nullptr);
    glm::vec4 tint{1.0f, 1.0f, 1.0f, 1.0f};
};

struct UISlotStripRecord {
    std::string id;
    std::string group = "default";

    glm::vec2 position{0.0f, 0.0f};
    glm::vec2 slotSize{44.0f, 44.0f};
    UIStyle style{};

    int selectedIndex = 0;
    bool visible = true;
    int order = 0;

    std::vector<UISlotStripItemRecord> slots;
};

// Retained UI System
class UISystem {
    public:
        UILabelRecord& createLabel(const std::string& id);
        UISlotStripRecord& createSlotStrip(const std::string& id);

        UILabelRecord* getLabel(const std::string& id);
        const UILabelRecord* getLabel(const std::string& id) const;

        UISlotStripRecord* getSlotStrip(const std::string& id);
        const UISlotStripRecord* getSlotStrip(const std::string& id) const;

        void setGroupVisible(const std::string& group, bool visible);
        bool isGroupVisible(const std::string& group) const;

        void clear();

        // TODO: drawWorld() ??
        void drawScreen(UIRenderer& uiRenderer,
                        TextRenderer& textRenderer,
                        AssetManager& assetManager,
                        int viewportWidth, int viewportHeight) const;
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

        void drawLabel(const UILabelRecord& record,
                       UIRenderer& uiRenderer,
                       TextRenderer& textRenderer,
                       AssetManager& assetManager,
                       int viewportWidth, int viewportHeight) const;
        void drawSlotStrip(const UISlotStripRecord& record, UIRenderer& uiRenderer) const;

        std::unordered_map<std::string, UILabelRecord> labels;
        std::unordered_map<std::string, UISlotStripRecord> slotStrips;
        std::unordered_map<std::string, bool> groupVisibility;
};