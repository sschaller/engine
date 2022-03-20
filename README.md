# C++ Vulkan Renderer

## Current Progress
- Create Window with SDL
- Setup vulkan instance, device
- Setup swapchain
- Working on engine / creation of render passes next


## Goals
- Deferred shading
- Some sort of order-independent transparency
- Well structured components (vulkan, engine, windowing, gui / inputs)

## Build & Run
- Currently only works on Windows. Open CMakeLists.txt in Visual Studio Code / Visual Studio
- Requires build tools (Visual Studio), Conan, VulkanSDK installed and in Path
- Or mkdir build && cd build && cmake .. && cmake --build .
- Run engine.exe

## Credits
- Conan
- SDL
- VulkanSDK

## Resources
- https://vulkan-tutorial.com
- https://github.com/SaschaWillems/Vulkan
- https://en.wikipedia.org/wiki/Deferred_shading
- https://docs.unity3d.com/Manual/SL-Reference.html