#pragma once

#include <filesystem>
#include <fstream>
#include <vector>

class ResourceManager {
public:
    static std::vector<char> ReadBinaryFile(const std::filesystem::path &rFilePath)
    {
        std::ifstream file(rFilePath, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            throw std::runtime_error("failed to open file: " + rFilePath.string() + "!");
        }

        std::size_t fileSize = static_cast<std::size_t>(file.tellg());
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        return buffer;
    }
};
