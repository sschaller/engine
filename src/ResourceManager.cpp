#include "ResourceManager.h"

#include <vulkan/vulkan.h>

#include <array>
#include <fstream>
#include <glm/glm.hpp>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION // define this in only *one* .cc
#include <stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include <tiny_obj_loader.h>

namespace {
VkVertexInputBindingDescription getBindingDescription(uint32_t vertexSize) {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = vertexSize;
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescription;
}

std::vector<VkVertexInputAttributeDescription> getAttributeDescription(bool hasNormals, bool hasTexCoords) {
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

    uint32_t location = 0;
    uint32_t offset = 0;

    {
        VkVertexInputAttributeDescription &rAttribute = attributeDescriptions.emplace_back();
        rAttribute.binding = 0;
        rAttribute.location = location++;
        rAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
        rAttribute.offset = offset;
        offset += 24;  // 3 * 32bit floats
    }

    if (hasNormals) {
        VkVertexInputAttributeDescription &rAttribute = attributeDescriptions.emplace_back();
        rAttribute.binding = 0;
        rAttribute.location = location++;
        rAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
        rAttribute.offset = offset;
        offset += 24;  // 3 * 32bit floats
    }

    if (hasTexCoords) {
        VkVertexInputAttributeDescription &rAttribute = attributeDescriptions.emplace_back();
        rAttribute.binding = 0;
        rAttribute.location = location++;
        rAttribute.format = VK_FORMAT_R32G32_SFLOAT;
        rAttribute.offset = offset;
        offset += 16;  // 2 * 32bit floats
    }
    return attributeDescriptions;
}

uint32_t findMemoryType(VkPhysicalDevice &physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    throw std::runtime_error("failed to find suitable memory type!");
}

void createBuffer(VkDevice &device, VkPhysicalDevice &physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                  VkBuffer &buffer, VkDeviceMemory &bufferMemory) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

}  // namespace

std::vector<char> ResourceManager::ReadBinaryFile(const std::filesystem::path &rFilePath) {
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

std::shared_ptr<VertexData> ResourceManager::LoadVertexDataFromObjFile(const std::filesystem::path &rFilePath) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, rFilePath.string().c_str(), nullptr, true);

    if (!ret || !err.empty()) {
        throw std::runtime_error("tinyobjloader error: " + err);
    }

    if (shapes.empty()) {
        // Get vertex definition of first vertex
        return std::make_shared<VertexData>();
    }

    tinyobj::index_t first = shapes.front().mesh.indices.front();
    bool hasNormals = (first.normal_index >= 0);
    bool hasTexCoords = (first.texcoord_index >= 0);
    uint32_t vertexSize = 3 * sizeof(float);
    if (hasNormals) {
        vertexSize += 3 * sizeof(float);
    }
    if (hasTexCoords) {
        vertexSize += 2 * sizeof(float);
    }

    size_t containerSize = 0;
    for (const tinyobj::shape_t &rShape : shapes) {
        containerSize += rShape.mesh.num_face_vertices.size() * vertexSize;
    }

    auto spVertexData = std::make_shared<VertexData>();
    std::vector<unsigned char> &container = spVertexData->vertexBuffer;
    container.reserve(containerSize);

    // Loop over shapes and combine them into one object
    for (const tinyobj::shape_t &rShape : shapes) {
        // Loop over faces
        uint32_t offset = 0;
        for (size_t numVertices = 0; numVertices < rShape.mesh.num_face_vertices.size(); numVertices++) {
            // Loop over vertices in the face.
            for (size_t v = 0; v < numVertices; v++) {
                tinyobj::index_t idx = rShape.mesh.indices[offset++];

                if (hasNormals != (idx.normal_index >= 0)) {
                    throw std::runtime_error("tinyobjloader: normal data not consistent!");
                }
                if (hasTexCoords != (idx.texcoord_index >= 0)) {
                    throw std::runtime_error("tinyobjloader: tex coords data not consistent!");
                }

                uint32_t vertexOffset = 0;
                std::memcpy(&container[offset * vertexSize],
                            &attrib.vertices[static_cast<size_t>(idx.vertex_index) * 3], 3 * sizeof(float));
                vertexOffset += 3 * sizeof(float);

                if (hasNormals) {
                    std::memcpy(&container[offset * vertexSize + vertexOffset],
                                &attrib.normals[static_cast<size_t>(idx.normal_index) * 3], 3 * sizeof(float));
                    vertexOffset += 3 * sizeof(float);
                }
                if (hasTexCoords) {
                    std::memcpy(&container[offset * vertexSize + vertexOffset],
                                &attrib.texcoords[static_cast<size_t>(idx.texcoord_index) * 2], 2 * sizeof(float));
                }
            }
        }
    }

    spVertexData->inputBindingDescription = getBindingDescription(vertexSize);
    spVertexData->attributeDescriptions = getAttributeDescription(hasNormals, hasTexCoords);
    return spVertexData;
}

std::shared_ptr<ImageData> ResourceManager::LoadImageDataFromFile(const std::filesystem::path &rFilePath) {
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(rFilePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb); // STBI_rgb_alpha

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }

    size_t length = texWidth * texHeight * texChannels;

    std::vector<unsigned char> data;
    data.reserve(length);
    std::memcpy(data.data(), pixels, length);
    stbi_image_free(pixels);

    return std::make_shared<ImageData>(static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), ImageFormat::r8g8b8_srgb, std::move(data));
}
