#include "DataList.hpp"

#include <cstddef>

void DataList::resize(int count) {
    items.clear();
    items.resize(static_cast<size_t>(count));
}

void DataList::clear() {
    items.clear();
}

bool DataList::isInBounds(int index) const {
    return index >= 0 && index < static_cast<int>(items.size());
}

PropertyBag* DataList::tryGet(int index) {
    if (!isInBounds(index)) {
        return nullptr;
    }

    return &items[static_cast<size_t>(index)];
}

const PropertyBag* DataList::tryGet(int index) const {
    if (!isInBounds(index)) {
        return nullptr;
    }

    return &items[static_cast<size_t>(index)];
}