# C++ Vulkan Renderer

## Current Progress
- Create Window with SDL
- Setup vulkan instance, device
- Setup swapchain
- Working on engine / creation of render passes next


## Goals
- Deferred shading
- Some sort of order-independent transparency
- Shadows
- Well structured components (vulkan, engine, windowing, gui / inputs)

## Build & Run
- Tested on Windows and MacOS.
- Requires build tools (msvc or clang), Conan, VulkanSDK installed and in Path
- Either:
    - Open CMakeLists.txt in Visual Studio
    - Open CMakeLists.txt in Visual Studio Code with CMake extension
    - `mkdir build && cd build && cmake .. && cmake --build .` and run engine application

## Credits
- Conan
- SDL
- VulkanSDK

## Resources
- https://vulkan-tutorial.com
- https://github.com/SaschaWillems/Vulkan
- https://en.wikipedia.org/wiki/Deferred_shading
- https://docs.unity3d.com/Manual/SL-Reference.html