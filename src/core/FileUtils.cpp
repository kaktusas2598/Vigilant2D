#include "FileUtils.hpp"

#include <fstream>
#include <sstream>

std::string FileUtils::loadFile(const std::string& filePath) {
    std::ifstream file;
    file.open(filePath);
    std::stringstream ss;
    ss << file.rdbuf();
    file.close();
    return ss.str();
}
