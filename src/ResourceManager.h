#pragma once


#include <filesystem>
#include <vector>
#include "VertexData.h"
#include "ImageData.h"

class ResourceManager {
public:
    static std::vector<char> ReadBinaryFile(const std::filesystem::path &rFilePath);
    static std::shared_ptr<VertexData> LoadVertexDataFromObjFile(const std::filesystem::path &rFilePath);
    static std::shared_ptr<ImageData> LoadImageDataFromFile(const std::filesystem::path &rFilePath);

    struct STextureHandle {
        uint32_t test;
    };

    std::shared_ptr<STextureHandle> GetTexture(const std::string &rTextureId);
    std::shared_ptr<STextureHandle> CreateTexture(const std::string &rTextureId);

    void CreateVertexDataBuffer(const VertexData &rVertexData);
};
