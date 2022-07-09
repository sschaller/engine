#include <iostream>

#include "DeviceContext.h"
#include "Engine.h"
#include "Scene.h"
#include "WindowManager.h"

#include "objects/MeshObject.h"
#include "materials/PhongMaterial.h"

int main(int argc, char *argv[]) {
    WindowManager manager;

    Window *pWindow = manager.CreateWindow("Test", 800u, 600u);

    DeviceContext context(manager.GetRequiredExtensions(pWindow));
    
    Scene scene;
    Engine engine(scene, context, *pWindow);

    auto spPhongMaterial = std::make_shared<PhongMaterial>();
    auto spMeshObject = std::make_unique<MeshObject>();
    spMeshObject->SetMaterial(spPhongMaterial);
    scene.AddObject(std::move(spMeshObject));

    while (true) {
        manager.PollEvents();

        if (manager.ShouldQuit()) {
            std::cout << "Quit" << std::endl;
            break;
        }

        // Create

        // Update objects, materials, etc

        // Here we can update swap chain if out of date

        engine.Render();

        // Good idea to differentiate between screen size dependent buffers and others
        // frame buffer / attachements vs vertex/index/uniform buffers
        // engine should own everything that needs to be recreated,
        // while everything else should be outside!
    }

    // Loop -> { PollEvents, Render }
    return 0;
}
