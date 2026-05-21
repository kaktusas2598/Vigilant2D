#include "DataListRegistry.hpp"

DataList& DataListRegistry::createList(const std::string& id, int size) {
    auto& list = lists[id];
    list.resize(size);
    return list;
}

DataList* DataListRegistry::getList(const std::string& id) {
    auto it = lists.find(id);
    return it != lists.end() ? &it->second : nullptr;
}

const DataList* DataListRegistry::getList(const std::string& id) const {
    auto it = lists.find(id);
    return it != lists.end() ? &it->second : nullptr;
}

bool DataListRegistry::hasList(const std::string& id) const {
    return lists.find(id) != lists.end();
}

void DataListRegistry::clear() {
    lists.clear();
}