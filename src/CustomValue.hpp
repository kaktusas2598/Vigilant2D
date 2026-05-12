#pragma once

#include <string>
#include <variant>

// Defines custom user script values
// TODO: Add support for glm vectors
using CustomValue = std::variant<
    std::monostate,
    bool,
    int,
    float,
    std::string
    >;