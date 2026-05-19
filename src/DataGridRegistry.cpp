#include "DataGridRegistry.hpp"

DataGrid& DataGridRegistry::createGrid(const std::string &id, int width, int height) {
    auto& grid = grids[id];
    grid.init(width, height);
    return grid;
}

DataGrid *DataGridRegistry::getGrid(const std::string &id) {
    auto it = grids.find(id);
    return (it != grids.end()) ? &(it->second) : nullptr;
}

const DataGrid *DataGridRegistry::getGrid(const std::string &id) const {
    auto it = grids.find(id);
    return (it != grids.end()) ? &(it->second) : nullptr;
}

bool DataGridRegistry::hasGrid(const std::string &id) const {
    return grids.find(id) != grids.end();
}

void DataGridRegistry::clear() {
    grids.clear();
}
