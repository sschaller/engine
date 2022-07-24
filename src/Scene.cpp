#include "Scene.h"

void Scene::AddObject(std::unique_ptr<Object> &&rrObject) { objects_.push_back(std::move(rrObject)); }

void Scene::RemoveObject(const Object &rObject) {
    auto it = std::find_if(objects_.begin(), objects_.end(),
                           [&rObject](const std::unique_ptr<Object> &rObj) { return rObj.get() == &rObject; });

    if (it != objects_.end()) {
        objects_.erase(it);
    }
}

void Scene::Update(const RenderContext &rContext) {
    for (auto &rspObject : objects_) {
        rspObject->Update(rContext);
    }
}

void Scene::Draw(const RenderContext &rContext) {
    for (auto &rspObject : objects_) {
        rspObject->Draw(rContext);
    }
}
