#include "UISlotStrip.hpp"

#include "UIRenderer.hpp"

#include <algorithm>

void UISlotStrip::setSlotCount(int count) {
    count = std::max(0, count);
    items.resize(static_cast<size_t>(count));

    if (selectedIndex >= count) {
        selectedIndex = count > 0 ? count - 1 : 0;
    }
}

void UISlotStrip::setSelectedIndex(int index) {
    if (items.empty()) {
        selectedIndex = 0;
        return;
    }

    selectedIndex = std::max(0, std::min(index, static_cast<int>(items.size()) - 1));
}

void UISlotStrip::setSlotItem(int index, const UISlotItem& item) {
    if (index < 0 || index >= static_cast<int>(items.size()))
        return;
    
    items[static_cast<size_t>(index)] = item;
}

glm::vec2 UISlotStrip::measure(const UIStyle& style, const glm::vec2& slotSize) const {
    const int count = static_cast<int>(items.size());
    if (count <= 0) {
        return {
            style.slotPadding * 2.0f,
            style.slotPadding * 2.0f
        };
    }

    const float width = style.slotPadding * 2.0f + count * slotSize.x + (count - 1) * style.slotSpacing;
    const float height = style.slotPadding * 2.0f + slotSize.y;
    return {width, height};
}

void UISlotStrip::draw(UIRenderer& uiRenderer,
const UIStyle& style,
const glm::vec2& position,
const glm::vec2& slotSize) const {
    const glm::vec2 panelSize = measure(style, slotSize);

    uiRenderer.drawQuad({
        {position, panelSize},
        TextureRegion::full(nullptr),
        style.panelColor
    });

    uiRenderer.drawQuadOutline({
        {position, panelSize},
        style.borderThickness,
        style.borderColor
    });

    glm::vec2 slotPos = position + glm::vec2(style.slotPadding, style.slotPadding);
    for (int i = 0; i < static_cast<int>(items.size()); ++i) {
        const bool selected = (i == selectedIndex);

        uiRenderer.drawQuad({
            {slotPos, slotSize},
            TextureRegion::full(nullptr),
            selected ? style.selectedSlotColor : style.slotColor
        });

        uiRenderer.drawQuadOutline({
            {slotPos, slotSize},
            1.0f,
            style.borderColor
        });

        const UISlotItem& item = items[static_cast<size_t>(i)];
        if (item.occupied && item.icon.texture != nullptr) {
            const glm::vec2 iconInset(6.0f, 6.0f);
            uiRenderer.drawQuad({
                {slotPos + iconInset, slotSize - iconInset * 2.0f},
                item.icon,
                item.tint
            });
        }

        slotPos.x += slotSize.x + style.slotSpacing;
    }
}
