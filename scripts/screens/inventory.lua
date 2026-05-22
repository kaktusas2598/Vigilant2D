local inventory = require("scripts.lib.inventory")
local uiLayout = require("scripts.lib.ui_layout")

local M = {}

function M.build(self)
    ui.create_container("menu.inventory.root", "menu.inventory")
    ui.set_container_render_space("menu.inventory.root", "screen")
    ui.set_container_screen_anchor("menu.inventory.root", 0.5, 0.5)
    ui.set_container_screen_pivot("menu.inventory.root", 0.5, 0.5)
    ui.set_container_position("menu.inventory.root", 0, 0)
    ui.set_container_size("menu.inventory.root", 900, 300)

    ui.create_image("menu.inventory.fill", "menu.inventory")
    ui.set_image_parent("menu.inventory.fill", "menu.inventory.root")
    ui.set_image_position("menu.inventory.fill", 270, 32)
    ui.set_image_texture_rect("menu.inventory.fill", "cozy_ui_inventory_chopped", 90, 112, 200, 78)
    ui.set_image_size("menu.inventory.fill", 600, 234)
    ui.set_image_order("menu.inventory.fill", 0)

    ui.create_image("menu.inventory.fill_left", "menu.inventory")
    ui.set_image_parent("menu.inventory.fill_left", "menu.inventory.root")
    ui.set_image_position("menu.inventory.fill_left", 25, 34)
    ui.set_image_texture_rect("menu.inventory.fill_left", "cozy_ui_inventory_chopped", 304, 16, 78, 78)
    ui.set_image_size("menu.inventory.fill_left", 234, 234)
    ui.set_image_order("menu.inventory.fill_left", 0)

    ui.create_image("menu.inventory.frame", "menu.inventory")
    ui.set_image_parent("menu.inventory.frame", "menu.inventory.root")
    ui.set_image_position("menu.inventory.frame", 0, 0)
    ui.set_image_texture_rect("menu.inventory.frame", "cozy_ui_inventory_chopped", 0, 12, 300, 100)
    ui.set_image_size("menu.inventory.frame", 900, 300)
    ui.set_image_order("menu.inventory.frame", 1)


    ui.create_label("menu.inventory.title", "menu.inventory")
    ui.set_label_parent("menu.inventory.title", "menu.inventory.root")
    ui.set_label_text("menu.inventory.title", "Inventory")
    ui.set_label_scale("menu.inventory.title", 1.4)
    -- TODO: centre align in relation to container
    ui.set_label_position("menu.inventory.title", 420, 310)

    ui.create_label("menu.inventory.tip_title", "menu.inventory")
    ui.set_label_parent("menu.inventory.tip_title", "menu.inventory.root")
    ui.set_label_text("menu.inventory.tip_title", "Press I to close")
    ui.set_label_scale("menu.inventory.tip_title", 1.0)
    -- TODO: right align in relation to container
    ui.set_label_position("menu.inventory.tip_title", 730, 310)

    local startX = 280
    local startY = 230
    local spacing = 56
    local slotSize = 32
    local slotIndex = 0

    -- Create inventory slot UI: icons and count text
    for i = 0, 25 do
        local imageId = "menu.inventory.slot_icon_" .. tostring(slotIndex)
        local countId = "menu.inventory.slot_count_" .. tostring(slotIndex)
        local x, y = uiLayout.grid_local_position(i, 8, startX, startY, spacing, spacing);

        ui.create_image(imageId, "menu.inventory")
        ui.set_image_parent(imageId, "menu.inventory.root")
        ui.set_image_position(imageId, x, y)
        ui.set_image_size(imageId, slotSize, slotSize)
        ui.set_image_visible(imageId, false)
        ui.set_image_order(imageId, 2)

        ui.create_label(countId, "menu.inventory")
        ui.set_label_parent(countId, "menu.inventory.root")
        ui.set_label_position(countId, x + 18, y - 18)
        ui.set_label_text(countId, "")
        ui.set_label_scale(countId, 0.8)

        slotIndex = slotIndex + 1
    end
end

function M.on_enter(self)
end

function M.on_exit(self)
end

function M.on_update(self, dt)
    for i = 0, 15 do
        local slot = inventory.get_slot("inventory", i)
        local imageId = "menu.inventory.slot_icon_" .. tostring(i)
        local countId = "menu.inventory.slot_count_" .. tostring(i)

        if slot ~= nil then
            if ui.set_image_from_entity_definition(imageId, slot.item_id) then
                ui.set_image_visible(imageId, true)
            else
                ui.set_image_visible(imageId, false)
            end
            if slot.count ~= nil and slot.count > 1 then
                ui.set_label_text(countId, tostring(slot.count))
            else
                ui.set_label_text(countId, "")
            end
        else
            ui.set_image_visible(imageId, false)
            ui.set_label_text(countId, "")
        end
    end

    if engine.is_key_pressed(73) then -- I
        screenflow.clear_overlay()
    end
end

return M