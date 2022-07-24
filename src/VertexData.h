#pragma once

#include <vector>
#include <vulkan/vulkan.h>

struct VertexData {
    VkVertexInputBindingDescription inputBindingDescription;
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

    std::vector<unsigned char> vertexBuffer;
};
