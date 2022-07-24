#pragma once

#include <memory>
#include <vector>
#include <vulkan/vulkan.h>
#include "objects/Object.h"

class Scene
{
public:
    void AddObject(std::unique_ptr<Object> &&rrObject);
    void RemoveObject(const Object &rObject);
    
    void Update(const RenderContext &rContext);
    void Draw(const RenderContext &rContext);
private:
    std::vector<std::unique_ptr<Object>> objects_;
};
