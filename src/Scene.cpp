#include "Scene.h"

void Scene::AddObject(std::unique_ptr<Object> &&rrObject) { objects_.push_back(std::move(rrObject)); }

void Scene::RemoveObject(const Object &rObject) {
    auto it = std::find_if(objects_.begin(), objects_.end(),
                           [&rObject](const std::unique_ptr<Object> &rObj) { return rObj.get() == &rObject; });

    if (it != objects_.end()) {
        objects_.erase(it);
    }
}

void Scene::Update(DeviceContext &rDeviceContext, Swapchain &rSwapchain, VkRenderPass &rRenderPass,
                   VkFormat &rImageFormat, VkCommandBuffer &rCommandBuffer, bool outOfDate) {
    for (auto &rspObject : objects_) {
        rspObject->Update(rDeviceContext, rSwapchain, rRenderPass, rImageFormat, rCommandBuffer, outOfDate);
    }
}

void Scene::Draw(VkCommandBuffer &rCommandBuffer) {
    for (auto &rspObject : objects_) {
        rspObject->Draw(rCommandBuffer);
    }
}
