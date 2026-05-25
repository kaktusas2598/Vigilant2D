local inventory = require("scripts.lib.inventory")
local items = require("scripts.items")
local M = {}

local function get_hotbar_slot(slotIndex)
    return inventory.get_slot("inventory", slotIndex)
end

local function get_item_display_name(itemId)
    local itemDef = items[itemId]
    if itemDef ~= nil and itemDef.name ~= nil then
        return itemDef.name
    end

    return itemId or "Empty"
end

function M.refresh_ui()
    for i = 0, 7 do
        local slot = get_hotbar_slot(i)

        if slot ~= nil and slot.item_id ~= nil and slot.item_id ~= "" then
            ui.set_slot_strip_slot_from_entity_definition("hud.hotbar", i, slot.item_id)
        else
            ui.clear_slot_strip_slot("hud.hotbar", i)
        end
    end
end

function M.apply_selected_slot(self)
    local slot = get_hotbar_slot(self.selected_slot - 1)

    if slot ~= nil and slot.item_id ~= nil and slot.item_id ~= "" then
        self.selected_tool = slot.item_id
        ui.set_label_text("hud.hotbar_label", get_item_display_name(slot.item_id))
    else
        self.selected_tool = nil
        ui.set_label_text("hud.hotbar_label", "Empty")
    end

    ui.set_slot_strip_selected("hud.hotbar", self.selected_slot - 1)
end

function M.update_selection_input(self)
    -- Mouse wheel to change selected slot in hotbar
    local scrollY = engine.get_mouse_scroll_y()
    if scrollY > 0 then
        self.selected_slot = (self.selected_slot - 2) % 8 + 1
        M.apply_selected_slot(self)
    elseif scrollY < 0 then
        self.selected_slot = (self.selected_slot % 8) + 1
        M.apply_selected_slot(self)
    end

    -- Number keys 1..8 map directly to hotbar slots 1..8.
    for i = 1, 8 do
        if engine.is_key_pressed(48 + i) then
            self.selected_slot = i
            M.apply_selected_slot(self)
            break
        end
    end
end

return M
