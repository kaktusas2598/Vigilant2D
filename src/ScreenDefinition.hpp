#pragma once

#include <string>
#include <vector>

struct ScreenDefinition {
    std::string id;
    std::vector<std::string> groups;
    bool overlay = false;
    bool pausesGameplay = false;
    bool initial = false;
    std::string script;
};