#pragma once

#include <vulkan/vulkan.h>

class DeviceContext;
class RenderContext;
class Swapchain;

class Object {
public:
    virtual ~Object() = default;
    virtual void Update(const RenderContext &context) = 0;
    virtual void Draw(const RenderContext &context) = 0;
};
