#pragma once

#include <string>
#include <unordered_map>
#include "CustomValue.hpp"

class PropertyBag {
    public:
        void set(const std::string& key, CustomValue value) {
            values[key] = value;
        }

        const CustomValue* get(const std::string& key) const {
            auto it = values.find(key);
            return it != values.end() ? &it->second : nullptr;
        }
        CustomValue* get(const std::string& key) {
            auto it = values.find(key);
            return it != values.end() ? &it->second : nullptr;
        }

        bool has(const std::string& key) const {
            return values.find(key) != values.end();
        }

        const std::unordered_map<std::string, CustomValue>& all() const {
            return values;
        }

    private:
        std::unordered_map<std::string, CustomValue> values;
};