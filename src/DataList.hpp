#pragma once

#include <vector>
#include "PropertyBag.hpp"

// Generic List Structure, could be used for inventories etc.
class DataList {
    public:
        void resize(int count);
        void clear();

        bool isInBounds(int index) const;

        PropertyBag* tryGet(int index);
        const PropertyBag* tryGet(int index) const;
        int getSize() const { return static_cast<int>(items.size()); }
    private:
        std::vector<PropertyBag> items;
};