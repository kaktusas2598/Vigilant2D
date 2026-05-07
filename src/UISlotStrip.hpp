#pragma once

#include <vector>

#include "UIPrimitives.hpp"
#include "UIStyle.hpp"

class UIRenderer;

struct UISlotItem {
    bool occupied = false;
    TextureRegion icon = TextureRegion::full(nullptr);
    glm::vec4 tint{1.0f, 1.0f, 1.0f, 1.0f};
};

// Reusable UI widget for creating hotbars, item bars, action bars, etc...
class UISlotStrip {
    public:
        void setSlotCount(int count);
        void setSelectedIndex(int index);
        void setSlotItem(int index, const UISlotItem& item);

        int getSlotCount() const { return static_cast<int>(items.size()); }
        int getSelectedIndex() const { return selectedIndex; }

        glm::vec2 measure(const UIStyle& style, const glm::vec2& slotSize) const;

        void draw(UIRenderer& uiRenderer,
                  const UIStyle& style,
                  const glm::vec2& position,
                  const glm::vec2& slotSize) const;
    private:
        std::vector<UISlotItem> items;
        int selectedIndex = 0;
};