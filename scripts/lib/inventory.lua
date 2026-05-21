local items = require("scripts.items")

local M = {}

local function get_item_def(item_id)
    return items[item_id]
end

function M.get_slot(list_id, index)
    if not list.is_in_bounds(list_id, index) then
        return nil
    end

    local item_id = list.get_data(list_id, index, "item_id")
    if item_id == nil then
        return nil
    end

    return {
        item_id = item_id,
        count = list.get_data(list_id, index, "count") or 0
    }
end

function M.clear_slot(list_id, index)
    if not list.is_in_bounds(list_id, index) then
        return false
    end

    list.set_data(list_id, index, "item_id", "")
    list.set_data(list_id, index, "count", 0)
    return true
end

function M.find_empty_slot(list_id)
    local index = 0
    while list.is_in_bounds(list_id, index) do
        local item_id = list.get_data(list_id, index, "item_id")
        local count = list.get_data(list_id, index, "count") or 0

        if item_id == nil or item_id == "" or count <= 0 then
            return index
        end

        index = index + 1
    end

    return nil
end

function M.find_stackable_slot(list_id, item_id)
    local item_def = get_item_def(item_id)
    if item_def == nil then
        return nil
    end

    local index = 0
    while list.is_in_bounds(list_id, index) do
        local slot_item_id = list.get_data(list_id, index, "item_id")
        local slot_count = list.get_data(list_id, index, "count") or 0

        if slot_item_id == item_id and slot_count < item_def.stack_limit then
            return index
        end

        index = index + 1
    end

    return nil
end

function M.add_item(list_id, item_id, count)
    local item_def = get_item_def(item_id)
    if item_def == nil then
        print("Unknown item id: " .. tostring(item_id))
        return 0
    end

    local remaining = count or 1
    if remaining <= 0 then
        return 0
    end

    while remaining > 0 do
        local slot_index = M.find_stackable_slot(list_id, item_id)

        if slot_index ~= nil then
            local slot_count = list.get_data(list_id, slot_index, "count") or 0
            local free_space = item_def.stack_limit - slot_count
            local to_add = math.min(free_space, remaining)

            list.set_data(list_id, slot_index, "count", slot_count + to_add)
            remaining = remaining - to_add
        else
            local empty_index = M.find_empty_slot(list_id)
            if empty_index == nil then
                break
            end

            local to_add = math.min(item_def.stack_limit, remaining)
            list.set_data(list_id, empty_index, "item_id", item_id)
            list.set_data(list_id, empty_index, "count", to_add)
            remaining = remaining - to_add
        end
    end

    return count - remaining
end

function M.remove_item(list_id, item_id, count)
    local remaining = count or 1
    if remaining <= 0 then
        return 0
    end

    local index = 0
    while list.is_in_bounds(list_id, index) and remaining > 0 do
        local slot_item_id = list.get_data(list_id, index, "item_id")
        local slot_count = list.get_data(list_id, index, "count") or 0

        if slot_item_id == item_id and slot_count > 0 then
            local to_remove = math.min(slot_count, remaining)
            local new_count = slot_count - to_remove

            if new_count <= 0 then
                M.clear_slot(list_id, index)
            else
                list.set_data(list_id, index, "count", new_count)
            end

            remaining = remaining - to_remove
        end

        index = index + 1
    end

    return count - remaining
end

function M.count_item(list_id, item_id)
    local total = 0
    local index = 0

    while list.is_in_bounds(list_id, index) do
        local slot_item_id = list.get_data(list_id, index, "item_id")
        local slot_count = list.get_data(list_id, index, "count") or 0

        if slot_item_id == item_id then
            total = total + slot_count
        end

        index = index + 1
    end

    return total
end

return M