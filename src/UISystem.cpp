#include "UISystem.hpp"

#include <algorithm>

#include "AssetManager.hpp"
#include "UILabel.hpp"
#include "UISlotStrip.hpp"
#include "TextRenderer.hpp"
#include "UIRenderer.hpp"

UILabelRecord& UISystem::createLabel(const std::string& id) {
    auto& label = labels[id];
    label.id = id;
    return label;
}

UISlotStripRecord& UISystem::createSlotStrip(const std::string& id) {
    auto& strip = slotStrips[id];
    strip.id = id;
    return strip;
}

UILabelRecord* UISystem::getLabel(const std::string& id) {
    auto it = labels.find(id);
    return it != labels.end() ? &it->second : nullptr;
}

const UILabelRecord* UISystem::getLabel(const std::string& id) const {
    auto it = labels.find(id);
    return it != labels.end() ? &it->second : nullptr;
}

UISlotStripRecord* UISystem::getSlotStrip(const std::string& id) {
    auto it = slotStrips.find(id);
    return it != slotStrips.end() ? &it->second : nullptr;
}

const UISlotStripRecord* UISystem::getSlotStrip(const std::string& id) const {
    auto it = slotStrips.find(id);
    return it != slotStrips.end() ? &it->second : nullptr;
}

void UISystem::setGroupVisible(const std::string& group, bool visible) {
    groupVisibility[group] = visible;
}

bool UISystem::isGroupVisible(const std::string& group) const {
    auto it = groupVisibility.find(group);
    if (it == groupVisibility.end())
        return true;
    return it->second;
}

bool UISystem::isWidgetVisible(const std::string& group, bool visible) const {
    return visible && isGroupVisible(group);
}

void UISystem::clear() {
    labels.clear();
    slotStrips.clear();
    groupVisibility.clear();
}

void UISystem::drawLabelGeometry(const UILabelRecord& record,
                         UIRenderer& uiRenderer,
                         TextRenderer& textRenderer,
                         AssetManager& assetManager) const {
    Font* font = assetManager.getFont(record.fontId);
    if (font == nullptr)
        return;

    UILabel label;
    label.setText(record.text);
    label.setPosition(record.position);
    label.setScale(record.scale);
    label.setTextColor(record.textColor);
    label.setPadding(record.padding);
    label.setBackgroundEnabled(record.backgroundEnabled);
    label.setBackgroundColor(record.backgroundColor);
    label.setBorderEnabled(record.borderEnabled);
    label.setBorderColor(record.borderColor);

    label.drawScreenGeometry(uiRenderer, textRenderer, *font);
}

void UISystem::drawLabelText(const UILabelRecord& record, TextRenderer& textRenderer, AssetManager& assetManager) const {
    Font* font = assetManager.getFont(record.fontId);
    if (font == nullptr)
        return;

    UILabel label;
    label.setText(record.text);
    label.setPosition(record.position);
    label.setScale(record.scale);
    label.setTextColor(record.textColor);
    label.setPadding(record.padding);
    label.setBackgroundEnabled(record.backgroundEnabled);
    label.setBackgroundColor(record.backgroundColor);
    label.setBorderEnabled(record.borderEnabled);
    label.setBorderColor(record.borderColor);

    label.drawScreenText(textRenderer, *font);
}


void UISystem::drawSlotStrip(const UISlotStripRecord& record,
                             UIRenderer& uiRenderer) const {
    UISlotStrip strip;
    strip.setSlotCount(static_cast<int>(record.slots.size()));
    strip.setSelectedIndex(record.selectedIndex);

    for (int i = 0; i < static_cast<int>(record.slots.size()); ++i) {
        const UISlotStripItemRecord& source = record.slots[static_cast<size_t>(i)];

        UISlotItem item;
        item.occupied = source.occupied;
        item.icon = source.icon;
        item.tint = source.tint;
        strip.setSlotItem(i, item);
    }

    strip.draw(uiRenderer, record.style, record.position, record.slotSize);
}

void UISystem::drawScreen(UIRenderer& uiRenderer,
                          TextRenderer& textRenderer,
                          AssetManager& assetManager,
                          int viewportWidth,
                          int viewportHeight) const {
    std::vector<const UISlotStripRecord*> visibleSlotStrips;
    std::vector<const UILabelRecord*> visibleLabels;

    // FIXME: Currently labels are drawn last because they are binding and unbinding different("text") shader
    // This is less than ideal
    for (const auto& pair : slotStrips) {
        const UISlotStripRecord& record = pair.second;
        if (isWidgetVisible(record.group, record.visible)) {
            visibleSlotStrips.push_back(&record);
        }
    }

    for (const auto& pair : labels) {
        const UILabelRecord& record = pair.second;
        if (isWidgetVisible(record.group, record.visible)) {
            visibleLabels.push_back(&record);
        }
    }

    std::sort(visibleSlotStrips.begin(), visibleSlotStrips.end(),
        [](const UISlotStripRecord* a, const UISlotStripRecord* b) {
            return a->order < b->order;
        });

    std::sort(visibleLabels.begin(), visibleLabels.end(),
        [](const UILabelRecord* a, const UILabelRecord* b) {
            return a->order < b->order;
        });


    // Phase 1: render all geometry first
    for (const UISlotStripRecord* record : visibleSlotStrips) {
        drawSlotStrip(*record, uiRenderer);
    }
    for (const UILabelRecord* record : visibleLabels) {
        drawLabelGeometry(*record, uiRenderer, textRenderer, assetManager);
    }

    // Phase 2: render all text using different shader
    if (!visibleLabels.empty()) {
        textRenderer.beginScreen(viewportWidth, viewportHeight);

        for (const UILabelRecord* record : visibleLabels) {
            drawLabelText(*record, textRenderer, assetManager);
        }

        textRenderer.end();
    }
}
