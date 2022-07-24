#pragma once

#include <memory>
#include "Object.h"
#include "../materials/Material.h"

class VertexData;

class MeshObject final : public Object {
public:
    ~MeshObject() override;
    void SetMaterial(const std::shared_ptr<Material> &rMaterial);
    std::shared_ptr<Material> GetMaterial() const;

    void SetVertexData(const std::shared_ptr<VertexData> &rVertexData);

    void Update(const RenderContext &context) override;
    void Draw(const RenderContext &context) override;

private:
    std::shared_ptr<Material> material_;
    std::shared_ptr<VertexData> vertexData_;
};
