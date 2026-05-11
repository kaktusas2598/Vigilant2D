#include "UISystem.hpp"

#include <algorithm>

#include "AssetManager.hpp"
#include "UILabel.hpp"
#include "UIProgressBar.hpp"
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

UIProgressBarRecord& UISystem::createProgressBar(const std::string& id) {
    auto& bar = progressBars[id];
    bar.id = id;
    return bar;
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

UIProgressBarRecord* UISystem::getProgressBar(const std::string& id) {
    auto it = progressBars.find(id);
    return it != progressBars.end() ? &it->second : nullptr;
}

const UIProgressBarRecord* UISystem::getProgressBar(const std::string& id) const {
    auto it = progressBars.find(id);
    return it != progressBars.end() ? &it->second : nullptr;
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
    progressBars.clear();
    groupVisibility.clear();
}

void UISystem::drawLabelWorldGeometry(const UILabelRecord& record,
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

    label.drawWorldGeometry(uiRenderer, textRenderer, *font);
}

void UISystem::drawLabelWorldText(const UILabelRecord& record,
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

    label.drawWorldText(textRenderer, *font);
}

void UISystem::drawLabelScreenGeometry(const UILabelRecord& record,
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

void UISystem::drawLabelScreenText(const UILabelRecord& record, TextRenderer& textRenderer, AssetManager& assetManager) const {
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

void UISystem::drawProgressBar(const UIProgressBarRecord& record,
                               UIRenderer& uiRenderer) const {
    UIProgressBar progressBar;
    UIProgressBarStyle style;
    style.backgroundColor = record.backgroundColor;
    style.borderColor = record.borderColor;
    style.borderEnabled = record.borderEnabled;
    style.borderThickness = record.borderThickness;
    style.fillColor = record.fillColor;
    style.fillInset = record.fillInset;

    progressBar.draw(uiRenderer, record.position, record.size, record.minValue, record.maxValue, record.value, style);
}

void UISystem::drawWorld(UIRenderer& uiRenderer,
                        TextRenderer& textRenderer,
                        AssetManager& assetManager,
                        const Camera2D& camera) const {
    std::vector<const UISlotStripRecord*> visibleSlotStrips;
    std::vector<const UILabelRecord*> visibleLabels;
    std::vector<const UIProgressBarRecord*> visibleProgressBars;

    for (const auto& pair : slotStrips) {
        const UISlotStripRecord& record = pair.second;
        if (record.renderSpace == UIRenderSpace::World && isWidgetVisible(record.group, record.visible)) {
            visibleSlotStrips.push_back(&record);
        }
    }

    for (const auto& pair : progressBars) {
        const UIProgressBarRecord& record = pair.second;
        if (record.renderSpace == UIRenderSpace::World && isWidgetVisible(record.group, record.visible)) {
            visibleProgressBars.push_back(&record);
        }
    }

    for (const auto& pair : labels) {
        const UILabelRecord& record = pair.second;
        if (record.renderSpace == UIRenderSpace::World && isWidgetVisible(record.group, record.visible)) {
            visibleLabels.push_back(&record);
        }
    }

    std::sort(visibleSlotStrips.begin(), visibleSlotStrips.end(),
        [](const UISlotStripRecord* a, const UISlotStripRecord* b) {
            return a->order < b->order;
        });

    std::sort(visibleProgressBars.begin(), visibleProgressBars.end(),
        [](const UIProgressBarRecord* a, const UIProgressBarRecord* b) {
            return a->order < b->order;
        });

    std::sort(visibleLabels.begin(), visibleLabels.end(),
        [](const UILabelRecord* a, const UILabelRecord* b) {
            return a->order < b->order;
        });

    // Phase 1: geometry
    for (const UISlotStripRecord* record : visibleSlotStrips) {
        drawSlotStrip(*record, uiRenderer);
    }

    for (const UIProgressBarRecord* record : visibleProgressBars) {
        drawProgressBar(*record, uiRenderer);
    }

    for (const UILabelRecord* record : visibleLabels) {
        drawLabelWorldGeometry(*record, uiRenderer, textRenderer, assetManager);
    }

    // Phase 2: text
    if (!visibleLabels.empty()) {
        textRenderer.begin(camera);

        for (const UILabelRecord* record : visibleLabels) {
            drawLabelWorldText(*record, textRenderer, assetManager);
        }

        textRenderer.end();
    }
}

void UISystem::drawScreen(UIRenderer& uiRenderer,
                          TextRenderer& textRenderer,
                          AssetManager& assetManager,
                          int viewportWidth,
                          int viewportHeight) const {
    std::vector<const UISlotStripRecord*> visibleSlotStrips;
    std::vector<const UILabelRecord*> visibleLabels;
    std::vector<const UIProgressBarRecord*> visibleProgressBars;

    for (const auto& pair : slotStrips) {
        const UISlotStripRecord& record = pair.second;
        if (record.renderSpace == UIRenderSpace::Screen && isWidgetVisible(record.group, record.visible)) {
            visibleSlotStrips.push_back(&record);
        }
    }

    for (const auto& pair : progressBars) {
        const UIProgressBarRecord& record = pair.second;
        if (record.renderSpace == UIRenderSpace::Screen && isWidgetVisible(record.group, record.visible)) {
            visibleProgressBars.push_back(&record);
        }
    }

    for (const auto& pair : labels) {
        const UILabelRecord& record = pair.second;
        if (record.renderSpace == UIRenderSpace::Screen && isWidgetVisible(record.group, record.visible)) {
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

    std::sort(visibleProgressBars.begin(), visibleProgressBars.end(),
        [](const UIProgressBarRecord* a, const UIProgressBarRecord* b) {
            return a->order < b->order;
        });

    // Draw Phase 1: render all geometry first
    for (const UISlotStripRecord* record : visibleSlotStrips) {
        drawSlotStrip(*record, uiRenderer);
    }
    for (const UILabelRecord* record : visibleLabels) {
        drawLabelScreenGeometry(*record, uiRenderer, textRenderer, assetManager);
    }
    for (const UIProgressBarRecord* record : visibleProgressBars) {
        drawProgressBar(*record, uiRenderer);
    }

    // Draw Phase 2: render all text using different shader
    if (!visibleLabels.empty()) {
        textRenderer.beginScreen(viewportWidth, viewportHeight);

        for (const UILabelRecord* record : visibleLabels) {
            drawLabelScreenText(*record, textRenderer, assetManager);
        }

        textRenderer.end();
    }
}
