#pragma once

#include <string>

// For common filesystem and file/resource operations
// TODO: implement methods for filesystem library which can be used in imgui for editor
class FileUtils {
    public:
        static std::string loadFile(const std::string& filePath);
};
