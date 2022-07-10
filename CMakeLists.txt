cmake_minimum_required(VERSION 3.10)

project(engine)

file(GLOB_RECURSE SOURCES "src/*.h" "src/*.cpp")

include(conan.cmake)
conan_cmake_run(CONANFILE conanfile.py
                BASIC_SETUP CMAKE_TARGETS
                BUILD missing)

find_package(Vulkan REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(engine ${SOURCES})

target_include_directories(engine PUBLIC ${Vulkan_INCLUDE_DIRS})

target_link_libraries(engine CONAN_PKG::sdl ${Vulkan_LIBRARIES} CONAN_PKG::glm CONAN_PKG::tinyobjloader)

set(SHADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
set(SHADER_TARGET_DIR shaders/)

add_custom_command(TARGET engine POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:engine>/${SHADER_TARGET_DIR}
  COMMENT "Creating ${SHADER_TARGET_DIR}")

file(GLOB SHADERS ${SHADER_SOURCE_DIR}/*.vert ${SHADER_SOURCE_DIR}/*.frag)

foreach(SHADER IN LISTS SHADERS)
    get_filename_component(FILENAME ${SHADER} NAME)
    add_custom_command(TARGET engine POST_BUILD
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${SHADER} -o $<TARGET_FILE_DIR:engine>/${SHADER_TARGET_DIR}/${FILENAME}.spv
        COMMENT "Compiling ${FILENAME}")
endForeach()
