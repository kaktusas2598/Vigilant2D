local inventory = require("scripts.lib.inventory")

local M = {}

local function set_slot_image(slotId, itemId)
    if itemId == "coin" then
        ui.set_image_texture(slotId, "crate")
        ui.set_image_visible(slotId, true)
    elseif itemId == "vegetable" then
        ui.set_image_texture_grid(slotId, "cozy_ui_items", 5, 0, 10, 12)
        ui.set_image_visible(slotId, true)
    else
        ui.set_image_visible(slotId, false)
    end
end

function M.build(self)
    ui.create_image("menu.inventory.fill", "menu.inventory")
    ui.set_image_render_space("menu.inventory.fill", "screen")
    ui.set_image_screen_anchor("menu.inventory.fill", 0.62, 0.5)
    ui.set_image_screen_pivot("menu.inventory.fill", 0.5, 0.5)
    ui.set_image_position("menu.inventory.fill", 0, 0)
    ui.set_image_texture_rect("menu.inventory.fill", "cozy_ui_inventory_chopped", 90, 112, 200, 78)
    ui.set_image_size("menu.inventory.fill", 600, 234)

    ui.create_image("menu.inventory.fill_left", "menu.inventory")
    ui.set_image_render_space("menu.inventory.fill_left", "screen")
    ui.set_image_screen_anchor("menu.inventory.fill_left", 0.2, 0.5)
    ui.set_image_screen_pivot("menu.inventory.fill_left", 0.5, 0.5)
    ui.set_image_position("menu.inventory.fill_left", 0, 0)
    ui.set_image_texture_rect("menu.inventory.fill_left", "cozy_ui_inventory_chopped", 304, 16, 78, 78)
    ui.set_image_size("menu.inventory.fill_left", 234, 234)


    ui.create_image("menu.inventory.frame", "menu.inventory")
    ui.set_image_render_space("menu.inventory.frame", "screen")
    ui.set_image_screen_anchor("menu.inventory.frame", 0.5, 0.5)
    ui.set_image_screen_pivot("menu.inventory.frame", 0.5, 0.5)
    ui.set_image_position("menu.inventory.frame", 0, 0)
    ui.set_image_texture_rect("menu.inventory.frame", "cozy_ui_inventory_chopped", 0, 12, 300, 100)
    ui.set_image_size("menu.inventory.frame", 900, 300)


    ui.create_label("menu.inventory.title", "menu.inventory")
    ui.set_label_text("menu.inventory.title", "Inventory")
    ui.set_label_scale("menu.inventory.title", 1.4)
    ui.set_label_screen_anchor("menu.inventory.title", 0.5, 0.5)
    ui.set_label_screen_pivot("menu.inventory.title", 0.5, 0.5)
    ui.set_label_position("menu.inventory.title", 0, 275)

    local startX = -120
    local startY = 40
    local spacing = 56
    local slotSize = 32
    local slotIndex = 0

    for row = 0, 3 do
        for col = 0, 9 do
            local imageId = "menu.inventory.slot_icon_" .. tostring(slotIndex)
            local countId = "menu.inventory.slot_count_" .. tostring(slotIndex)

            ui.create_image(imageId, "menu.inventory")
            ui.set_image_render_space(imageId, "screen")
            ui.set_image_screen_anchor(imageId, 0.48, 0.55)
            ui.set_image_screen_pivot(imageId, 0.5, 0.5)
            ui.set_image_position(imageId, startX + col * spacing, startY - row * spacing)
            ui.set_image_size(imageId, slotSize, slotSize)
            ui.set_image_visible(imageId, false)

            ui.create_label(countId, "menu.inventory")
            ui.set_label_screen_anchor(countId, 0.5, 0.5)
            ui.set_label_screen_pivot(countId, 1.0, 1.0)
            ui.set_label_position(countId, startX + col * spacing + 18, startY - row * spacing - 18)
            ui.set_label_text(countId, "")
            ui.set_label_scale(countId, 0.8)

            slotIndex = slotIndex + 1
        end
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
        local countId = "menu.inven,tory.slot_count_" .. tostring(i)

        if slot ~= nil then
            set_slot_image(imageId, slot.item_id)
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