#pragma once

#include <unordered_map>
#include <string>
#include "DataList.hpp"

class DataListRegistry {
    public:
        DataList& createList(const std::string& id, int size);

        DataList* getList(const std::string& id);
        const DataList* getList(const std::string& id) const;

        bool hasList(const std::string& id) const;
        void clear();
    private:
        std::unordered_map<std::string, DataList> lists;
};