cmake_minimum_required(VERSION 3.20)
set(Project_name VulkanProject)
project(${Project_name})

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
        src/core/window.h
        src/core/window.cpp
        src/core/device.h
        src/core/device.cpp)

set(Vulkan_Project_SRC
        src/main.cpp
        ${SOURCES}
        )

# ~/VulkanSDK/1.2.189.0/macOS/bin/glslc
#add_custom_target(
#        CompileShaders ALL
#        COMMAND ./compileShaders.sh
#        COMMENT "Compiling shaders"
#)

add_compile_options(-Wno-writable-strings -Wno-format-security)
add_executable(${Project_name} ${Vulkan_Project_SRC})

target_include_directories(${Project_name}
        PRIVATE src
        PRIVATE src/core
        )

add_subdirectory(vendor/glfw EXCLUDE_FROM_ALL)
target_link_libraries(${Project_name} PRIVATE glfw)

add_subdirectory(vendor/glm EXCLUDE_FROM_ALL)
target_link_libraries(${Project_name} PRIVATE glm)

find_package(Vulkan)
if(Vulkan_FOUND)
    target_include_directories(${Project_name} PRIVATE Vulkan::Vulkan)
    target_link_libraries(${Project_name} PRIVATE Vulkan::Vulkan)
endif()