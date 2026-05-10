#pragma once

#include <vector>

// Custom game data, NOTE: not sure if I like having to implement custom c++ layer here
// TODO: Do some research on how game engines handle custom game layer data, maybe not suitable 
// for Lua scripting, but there might be some better answer here, maybe something data-defined
struct FarmTileState {
    bool tilled = false;
    bool watered = false;
    bool occupied = false;
};

class FarmWorldState {
public:
    void init(int width, int height);
    void clear();

    bool isInBounds(int x, int y) const;

    FarmTileState* tryGet(int x, int y);
    const FarmTileState* tryGet(int x, int y) const;

    bool isTilled(int x, int y) const;
    bool isWatered(int x, int y) const;
    bool isOccupied(int x, int y) const;

    bool setTilled(int x, int y, bool value);
    bool setWatered(int x, int y, bool value);
    bool setOccupied(int x, int y, bool value);

    int getWidth() const { return width; }
    int getHeight() const { return height; }

private:
    int indexOf(int x, int y) const;

    int width = 0;
    int height = 0;
    std::vector<FarmTileState> tiles;
};