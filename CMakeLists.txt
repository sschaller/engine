cmake_minimum_required(VERSION 3.10)

project(engine)

file(GLOB SOURCES "src/*.h" "src/*.cpp")

include(conan.cmake)
conan_cmake_run(CONANFILE conanfile.py
                BASIC_SETUP CMAKE_TARGETS
                BUILD missing)

find_package(Vulkan REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(engine ${SOURCES})

target_include_directories(engine PUBLIC ${Vulkan_INCLUDE_DIRS})

target_link_libraries(engine CONAN_PKG::sdl ${Vulkan_LIBRARIES})
