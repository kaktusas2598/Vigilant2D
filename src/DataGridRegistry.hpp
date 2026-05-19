#pragma once

#include <unordered_map>
#include <string>
#include "DataGrid.hpp"

class DataGridRegistry {
    public:
        DataGrid& createGrid(const std::string& id, int width, int height);

        DataGrid* getGrid(const std::string& id);
        const DataGrid* getGrid(const std::string& id) const;

        bool hasGrid(const std::string& id) const;
        void clear();
    private:
        std::unordered_map<std::string, DataGrid> grids;
};