#include "UISystem.hpp"

#include <algorithm>

#include <GLFW/glfw3.h>
#include "core/Input.hpp"

#include "AssetManager.hpp"
#include "UIButton.hpp"
#include "UILabel.hpp"
#include "UIProgressBar.hpp"
#include "UISlotStrip.hpp"
#include "TextRenderer.hpp"
#include "UIRenderer.hpp"

static glm::vec2 measureTextPanelSize(TextRenderer& textRenderer,
                                      const Font& font,
                                      const std::string& text,
                                      float scale,
                                      const glm::vec2& padding) {
    return textRenderer.measureText(font, text, scale) + padding * 2.0f;
}

static glm::vec2 resolveScreenWidgetPosition(const glm::vec2& basePosition,
                                             const UIScreenLayout& layout,
                                             const glm::vec2& widgetSize,
                                             int viewportWidth,
                                             int viewportHeight) {
    if (!layout.enabled)
        return basePosition;

    const glm::vec2 anchorPoint{
        viewportWidth * layout.anchor.x,
        viewportHeight * layout.anchor.y
    };

    return anchorPoint + basePosition - widgetSize * layout.pivot;
}

static bool pointInsideRect(const glm::vec2& point, const glm::vec2& pos, const glm::vec2& size) {
    return point.x >= pos.x &&
           point.y >= pos.y &&
           point.x <= pos.x + size.x &&
           point.y <= pos.y + size.y;
}

static UIButton makeButtonWidget(const UIButtonRecord& record,
                                 const glm::vec2& resolvedPosition,
                                 const UIButtonVisualState& visualState) {
    UIButton button;
    button.setText(record.text);
    button.setPosition(resolvedPosition);
    button.setSize(record.size);
    button.setTextScale(record.textScale);
    button.setBorderEnabled(record.borderEnabled);

    button.setTextColor(visualState.textColor);
    button.setBackgroundColor(visualState.backgroundColor);
    button.setBorderColor(visualState.borderColor);

    button.setIconEnabled(record.iconEnabled);
    button.setIcon(record.icon);
    button.setIconSize(record.iconSize);
    button.setIconTint(visualState.iconTint);

    return button;
}

static const UIButtonVisualState& getButtonVisualState(const UIButtonRecord& record) {
    if (record.pressedNow)
        return record.pressed;
    if (record.hoveredNow)
        return record.hovered;
    return record.normal;
}

static UILabel makeLabelWidget(const UILabelRecord& record, const glm::vec2& resolvedPosition) {
    UILabel label;
    label.setText(record.text);
    label.setPosition(resolvedPosition);
    label.setScale(record.scale);
    label.setTextColor(record.textColor);
    label.setPadding(record.padding);
    label.setBackgroundEnabled(record.backgroundEnabled);
    label.setBackgroundColor(record.backgroundColor);
    label.setBorderEnabled(record.borderEnabled);
    label.setBorderColor(record.borderColor);
    return label;
}

static glm::vec2 resolveScreenLabelPosition(const UILabelRecord& record,
                                            TextRenderer& textRenderer,
                                            const Font& font,
                                            int viewportWidth,
                                            int viewportHeight) {
    if (!record.screenLayout.enabled) {
        return record.position;
    }

    const glm::vec2 textSize = textRenderer.measureText(font, record.text, record.scale);
    const glm::vec2 boxSize = textSize + record.padding * 2.0f;
    const glm::vec2 anchorPoint{
        viewportWidth * record.screenLayout.anchor.x,
        viewportHeight * record.screenLayout.anchor.y
    };

    return anchorPoint + record.position - boxSize * record.screenLayout.pivot;
}

UIContainerRecord& UISystem::createContainer(const std::string& id) {
    auto& container = containers[id];
    container.id = id;
    return container;
}

UILabelRecord& UISystem::createLabel(const std::string& id) {
    auto& label = labels[id];
    label.id = id;
    return label;
}

UIButtonRecord& UISystem::createButton(const std::string& id) {
    auto& button = buttons[id];
    button.id = id;

    button.hovered.backgroundColor = {0.18f, 0.18f, 0.20f, 0.98f};
    button.hovered.borderColor = {0.95f, 0.88f, 0.60f, 1.0f};
    button.pressed.backgroundColor = {0.24f, 0.22f, 0.16f, 1.0f};
    button.pressed.borderColor = {1.0f, 0.90f, 0.55f, 1.0f};

    return button;
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

UIImageRecord& UISystem::createImage(const std::string& id) {
    auto& image = images[id];
    image.id = id;
    return image;
}

UIContainerRecord* UISystem::getContainer(const std::string& id) {
    auto it = containers.find(id);
    return it != containers.end() ? &it->second : nullptr;
}

const UIContainerRecord* UISystem::getContainer(const std::string& id) const {
    auto it = containers.find(id);
    return it != containers.end() ? &it->second : nullptr;
}



UIImageRecord* UISystem::getImage(const std::string& id) {
    auto it = images.find(id);
    return it != images.end() ? &it->second : nullptr;
}

const UIImageRecord* UISystem::getImage(const std::string& id) const {
    auto it = images.find(id);
    return it != images.end() ? &it->second : nullptr;
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

UIButtonRecord* UISystem::getButton(const std::string& id) {
    auto it = buttons.find(id);
    return it != buttons.end() ? &it->second : nullptr;
}

const UIButtonRecord* UISystem::getButton(const std::string& id) const {
    auto it = buttons.find(id);
    return it != buttons.end() ? &it->second : nullptr;
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

void UISystem::updateScreenInteraction(const Input& input,
                                      TextRenderer& textRenderer,
                                      AssetManager& assetManager,
                                      int viewportWidth,
                                      int viewportHeight) {
    for (auto& pair : buttons) {
        pair.second.hoveredNow = false;
        pair.second.pressedNow = false;
        pair.second.clicked = false;
    }

    UIButtonRecord* hoveredButton = nullptr;
    int hoveredOrder = 0;
    const glm::vec2 mousePos{
        static_cast<float>(input.getMouseX()),
        viewportHeight - static_cast<float>(input.getMouseY())
    };

    for (auto& pair : buttons) {
        UIButtonRecord& record = pair.second;
        if (!isWidgetVisible(record.group, record.visible))
            continue;

        const glm::vec2 resolvedPosition = resolveParentedPosition(
            record.parentId,
            record.position,
            UIRenderSpace::Screen,
            record.screenLayout,
            record.size,
            viewportWidth,
            viewportHeight
        );
        if (pointInsideRect(mousePos, resolvedPosition, record.size)) {
            if (hoveredButton == nullptr || record.order >= hoveredOrder) {
                hoveredButton = &record;
                hoveredOrder = record.order;
            }
        }
    }

    if (hoveredButton != nullptr)
        hoveredButton->hoveredNow = true;
    
    if (hoveredButton != nullptr && input.isMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT)) {
        activeScreenButtonId = hoveredButton->id;
    }

    UIButtonRecord* activeButton = getButton(activeScreenButtonId);
    if (activeButton != nullptr && isWidgetVisible(activeButton->group, activeButton->visible)) {
        activeButton->pressedNow = input.isMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT);
    }

    if (input.isMouseButtonReleased(GLFW_MOUSE_BUTTON_LEFT)) {
        if (activeButton != nullptr && hoveredButton != nullptr && activeButton->id == hoveredButton->id) {
            activeButton->clicked = true;
        }
        activeScreenButtonId.clear();
    }
}

void UISystem::clear() {
    labels.clear();
    buttons.clear();
    slotStrips.clear();
    progressBars.clear();
    images.clear();
    containers.clear();
    groupVisibility.clear();
    activeScreenButtonId.clear();
}

glm::vec2 UISystem::resolveContainerPosition(const UIContainerRecord& record,
                                             int viewportWidth,
                                             int viewportHeight) const {
    if (record.renderSpace == UIRenderSpace::Screen) {
        return resolveScreenWidgetPosition(
            record.position,
            record.screenLayout,
            record.size,
            viewportWidth,
            viewportHeight
        );
    }

    return record.position;
}

glm::vec2 UISystem::resolveParentedPosition(const std::string& parentId,
                                            const glm::vec2& localPosition,
                                            UIRenderSpace renderSpace,
                                            const UIScreenLayout& screenLayout,
                                            const glm::vec2& widgetSize,
                                            int viewportWidth,
                                            int viewportHeight) const {
    if (!parentId.empty()) {
        const UIContainerRecord* parent = getContainer(parentId);
        if (parent != nullptr && isWidgetVisible(parent->group, parent->visible)) {
            return resolveContainerPosition(*parent, viewportWidth, viewportHeight) + localPosition;
        }
    }

    if (renderSpace == UIRenderSpace::Screen) {
        return resolveScreenWidgetPosition(
            localPosition,
            screenLayout,
            widgetSize,
            viewportWidth,
            viewportHeight
        );
    }

    return localPosition;
}

void UISystem::drawLabelWorldGeometry(const UILabelRecord& record,
                                      UIRenderer& uiRenderer,
                                      TextRenderer& textRenderer,
                                      AssetManager& assetManager) const {
    Font* font = assetManager.getFont(record.fontId);
    if (font == nullptr)
        return;

    UILabel label = makeLabelWidget(record, record.position);
    label.drawWorldGeometry(uiRenderer, textRenderer, *font);
}

void UISystem::drawLabelWorldText(const UILabelRecord& record,
                                  TextRenderer& textRenderer,
                                  AssetManager& assetManager) const {
    Font* font = assetManager.getFont(record.fontId);
    if (font == nullptr)
        return;

    UILabel label = makeLabelWidget(record, record.position);
    label.drawWorldText(textRenderer, *font);
}

void UISystem::drawLabelScreenGeometry(const UILabelRecord& record,
                         UIRenderer& uiRenderer,
                         TextRenderer& textRenderer,
                         AssetManager& assetManager, 
                         int viewportWidth, int viewportHeight) const {
    Font* font = assetManager.getFont(record.fontId);
    if (font == nullptr)
        return;

    glm::vec2 resolvedPosition;
    if (!record.parentId.empty()) {
        const glm::vec2 textSize = textRenderer.measureText(*font, record.text, record.scale);
        const glm::vec2 boxSize = textSize + record.padding * 2.0f;
        resolvedPosition = resolveParentedPosition(
            record.parentId,
            record.position,
            UIRenderSpace::Screen,
            record.screenLayout,
            boxSize,
            viewportWidth,
            viewportHeight
        );
    } else {
        resolvedPosition = resolveScreenLabelPosition(record, textRenderer, *font, viewportWidth, viewportHeight);
    }

    UILabel label = makeLabelWidget(record, resolvedPosition);
    label.drawScreenGeometry(uiRenderer, textRenderer, *font);
}

void UISystem::drawLabelScreenText(const UILabelRecord& record,
    TextRenderer& textRenderer, AssetManager& assetManager, int viewportWidth, int viewportHeight) const {
    Font* font = assetManager.getFont(record.fontId);
    if (font == nullptr)
        return;

    glm::vec2 resolvedPosition;
    if (!record.parentId.empty()) {
        const glm::vec2 textSize = textRenderer.measureText(*font, record.text, record.scale);
        const glm::vec2 boxSize = textSize + record.padding * 2.0f;
        resolvedPosition = resolveParentedPosition(
            record.parentId,
            record.position,
            UIRenderSpace::Screen,
            record.screenLayout,
            boxSize,
            viewportWidth,
            viewportHeight
        );
    } else {
        resolvedPosition = resolveScreenLabelPosition(record, textRenderer, *font, viewportWidth, viewportHeight);
    }

    UILabel label = makeLabelWidget(record, resolvedPosition);
    label.drawScreenText(textRenderer, *font);
}

void UISystem::drawButtonScreenGeometry(const UIButtonRecord& record,
                                      UIRenderer& uiRenderer,
                                      int viewportWidth, int viewportHeight) const {
    const glm::vec2 resolvedPosition = resolveParentedPosition(
        record.parentId,
        record.position,
        UIRenderSpace::Screen,
        record.screenLayout,
        record.size,
        viewportWidth,
        viewportHeight
    );
    const UIButtonVisualState& visualState = getButtonVisualState(record);
    UIButton button = makeButtonWidget(record, resolvedPosition, visualState);
    button.drawScreenGeometry(uiRenderer);
}

void UISystem::drawButtonScreenText(const UIButtonRecord& record,
                                  TextRenderer& textRenderer,
                                  AssetManager& assetManager,
                                  int viewportWidth, int viewportHeight) const {
    const glm::vec2 resolvedPosition = resolveParentedPosition(
        record.parentId,
        record.position,
        UIRenderSpace::Screen,
        record.screenLayout,
        record.size,
        viewportWidth,
        viewportHeight
    );
    const UIButtonVisualState& visualState = getButtonVisualState(record);
    UIButton button = makeButtonWidget(record, resolvedPosition, visualState);
    button.drawScreenText(textRenderer, *assetManager.getFont(record.fontId));
}

void UISystem::drawButtonScreenIcon(const UIButtonRecord& record,
                                  UIRenderer& uiRenderer,
                                  int viewportWidth, int viewportHeight) const {
    const glm::vec2 resolvedPosition = resolveParentedPosition(
        record.parentId,
        record.position,
        UIRenderSpace::Screen,
        record.screenLayout,
        record.size,
        viewportWidth,
        viewportHeight
    );
    const UIButtonVisualState& visualState = getButtonVisualState(record);
    UIButton button = makeButtonWidget(record, resolvedPosition, visualState);
    button.drawScreenIcon(uiRenderer);
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
                               UIRenderer& uiRenderer,
                               int viewportWidth, int viewportHeight) const {
    UIProgressBar progressBar;
    UIProgressBarStyle style;
    style.backgroundColor = record.backgroundColor;
    style.borderColor = record.borderColor;
    style.borderEnabled = record.borderEnabled;
    style.borderThickness = record.borderThickness;
    style.fillColor = record.fillColor;
    style.fillInset = record.fillInset;

    glm::vec2 resolvedPosition = resolveParentedPosition(
        record.parentId,
        record.position,
        record.renderSpace,
        UIScreenLayout{},
        record.size,
        viewportWidth,
        viewportHeight
    );

    progressBar.draw(uiRenderer, resolvedPosition, record.size, record.minValue, record.maxValue, record.value, style);
}

void UISystem::drawImage(const UIImageRecord& record,
                            UIRenderer& uiRenderer,
                            int viewportWidth, int viewportHeight) const {
    glm::vec2 resolvedPosition = record.position;
    if (record.renderSpace == UIRenderSpace::Screen) {
        resolvedPosition = resolveParentedPosition(
            record.parentId,
            record.position,
            record.renderSpace,
            record.screenLayout,
            record.size,
            viewportWidth,
            viewportHeight
        );
    }

    uiRenderer.drawQuad({
        {resolvedPosition, record.size},
        record.region,
        record.tint
    });
}

void UISystem::drawWorld(UIRenderer& uiRenderer,
                        TextRenderer& textRenderer,
                        AssetManager& assetManager,
                        const Camera2D& camera) const {
    std::vector<const UISlotStripRecord*> visibleSlotStrips;
    std::vector<const UILabelRecord*> visibleLabels;
    std::vector<const UIProgressBarRecord*> visibleProgressBars;
    std::vector<const UIImageRecord*> visibleImages;

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

    for (const auto& pair : images) {
        const UIImageRecord& record = pair.second;
        if (record.renderSpace == UIRenderSpace::World && isWidgetVisible(record.group, record.visible)) {
            visibleImages.push_back(&record);
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

    std::sort(visibleImages.begin(), visibleImages.end(),
        [](const UIImageRecord* a, const UIImageRecord* b) {
            return a->order < b->order;
        });

    // Phase 1: geometry
    for (const UISlotStripRecord* record : visibleSlotStrips) {
        drawSlotStrip(*record, uiRenderer);
    }

    for (const UIProgressBarRecord* record : visibleProgressBars) {
        drawProgressBar(*record, uiRenderer, 0, 0);
    }

    for (const UIImageRecord* record : visibleImages) {
        drawImage(*record, uiRenderer, 0, 0);
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
    std::vector<const UIButtonRecord*> visibleButtons;
    std::vector<const UIImageRecord*> visibleImages;

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

    for (const auto& pair : buttons) {
        const UIButtonRecord& record = pair.second;
        if (isWidgetVisible(record.group, record.visible)) {
            visibleButtons.push_back(&record);
        }
    }

    for (const auto& pair : images) {
        const UIImageRecord& record = pair.second;
        if (record.renderSpace == UIRenderSpace::Screen && isWidgetVisible(record.group, record.visible)) {
            visibleImages.push_back(&record);
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

    std::sort(visibleButtons.begin(), visibleButtons.end(),
        [](const UIButtonRecord* a, const UIButtonRecord* b) {
            return a->order < b->order;
        });

    std::sort(visibleImages.begin(), visibleImages.end(),
        [](const UIImageRecord* a, const UIImageRecord* b) {
            return a->order < b->order;
        });

    // Draw Phase 1: render all geometry first
    for (const UISlotStripRecord* record : visibleSlotStrips) {
        drawSlotStrip(*record, uiRenderer);
    }
    for (const UIImageRecord* record : visibleImages) {
        drawImage(*record, uiRenderer, viewportWidth, viewportHeight);
    }
    for (const UILabelRecord* record : visibleLabels) {
        drawLabelScreenGeometry(*record, uiRenderer, textRenderer, assetManager, viewportWidth, viewportHeight);
    }
    for (const UIButtonRecord* record : visibleButtons) {
        drawButtonScreenGeometry(*record, uiRenderer, viewportWidth, viewportHeight);
    }
    for (const UIProgressBarRecord* record : visibleProgressBars) {
        drawProgressBar(*record, uiRenderer, viewportWidth, viewportHeight);
    }

    // Draw Phase 2: render all text using different shader
    if (!visibleLabels.empty()) {
        textRenderer.beginScreen(viewportWidth, viewportHeight);

        for (const UILabelRecord* record : visibleLabels) {
            drawLabelScreenText(*record, textRenderer, assetManager, viewportWidth, viewportHeight);
        }

        for (const UIButtonRecord* record : visibleButtons) {
            drawButtonScreenText(*record, textRenderer, assetManager, viewportWidth, viewportHeight);
        }

        textRenderer.end();
    }
}
