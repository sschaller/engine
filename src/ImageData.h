#pragma once

#include <stdint.h>

enum class ImageFormat : uint32_t { r8g8b8_srgb };

struct ImageData {
    uint32_t width_;
    uint32_t height_;
    ImageFormat format_;
    std::vector<unsigned char> data_;

    ImageData(uint32_t width, uint32_t height, ImageFormat format, std::vector<unsigned char> &&data)
        : width_(width), height_(height), format_(format), data_(std::move(data)) {}
};
