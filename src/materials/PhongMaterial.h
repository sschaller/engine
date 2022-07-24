#pragma once

#include <memory>
#include "Material.h"

class GraphicsPipeline;
class ImageData;

class PhongMaterial : public Material {
public:
    ~PhongMaterial() override;
    void Update(const RenderContext &rContext) override;
    void Bind(VkCommandBuffer &rCommandBuffer) override;

    void SetImage(const std::shared_ptr<ImageData> &imageData);

private:
    std::unique_ptr<GraphicsPipeline> spPipeline_;
    std::shared_ptr<ImageData> imageData_;
};
