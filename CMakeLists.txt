cmake_minimum_required(VERSION 3.20)
set(Project_name VulkanProject)
project(${Project_name})

set(CMAKE_CXX_STANDARD 17)

set(SOURCES)

set(Vulkan_Project_SRC
        main.cpp
        ${SOURCES}
        )

# ~/VulkanSDK/1.2.189.0/macOS/bin/glslc
#add_custom_target(
#        CompileShaders ALL
#        COMMAND ./compileShaders.sh
#        COMMENT "Compiling shaders"
#)

add_executable(${Project_name} ${Vulkan_Project_SRC})

target_include_directories(${Project_name}
        PRIVATE core
        PRIVATE application
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