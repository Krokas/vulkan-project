cmake_minimum_required(VERSION 3.20)
set(Project_name VulkanProject)
project(${Project_name})

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
        src/core/window.h
        src/core/window.cpp
        src/core/device.h
        src/core/device.cpp
        src/core/swapChain.h
        src/core/swapChain.cpp
        src/core/renderer.h
        src/core/renderer.cpp
        src/core/descriptor.h
        src/core/descriptor.cpp
        src/core/pipeline.h
        src/core/pipeline.cpp
        src/utils/file.h
        src/utils/file.cpp
        src/core/buffer.h
        src/core/buffer.cpp
        src/core/gameObject.h
        src/core/gameObject.cpp
        src/core/model.h
        src/core/model.cpp
        src/core/camera.h
        src/core/camera.cpp
        src/utils/hash.h
        src/systems/TextSystem.h
        src/systems/TextSystem.cpp
        src/core/renderInfo.h
        src/core/texture.h
        src/core/texture.cpp
        src/core/components/textComponent.h
        src/core/font.h
        src/core/font.cpp
        src/utils/allocator.h
        src/utils/allocator.cpp
        src/utils/color.h
        src/utils/timer.h
        src/utils/timer.cpp)

set(Vulkan_Project_SRC
        src/main.cpp
        ${SOURCES})

set(COMPILED_SHADERS 
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/text_vert.spv
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/text_frag.spv)

add_compile_options(-Wno-writable-strings -Wno-format-security)
add_executable(${Project_name} ${Vulkan_Project_SRC} ${COMPILED_SHADERS})

target_include_directories(${Project_name}
        PRIVATE src
        PRIVATE src/core
        PRIVATE src/utils
        PRIVATE src/systems
        PUBLIC  vendor/tinyobjloader
        PUBLIC  vendor/stb
        PRIVATE src/core/components
        )

add_subdirectory(vendor/glfw EXCLUDE_FROM_ALL)
target_link_libraries(${Project_name} PRIVATE glfw)

add_subdirectory(vendor/glm EXCLUDE_FROM_ALL)
target_link_libraries(${Project_name} PRIVATE glm)

add_subdirectory(vendor/tinyobjloader EXCLUDE_FROM_ALL)

add_subdirectory(vendor/freetype EXCLUDE_FROM_ALL)
target_link_libraries(${Project_name} PRIVATE freetype)

find_package(Vulkan)
if(Vulkan_FOUND)
    target_include_directories(${Project_name} PRIVATE Vulkan::Vulkan)
    target_link_libraries(${Project_name} PRIVATE Vulkan::Vulkan)
endif()


if(DEFINED ENV{VULKAN_SDK_PATH})
message(VULKAN_SDK_PATH=$ENV{VULKAN_SDK_PATH})
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/text_vert.spv
  COMMAND $ENV{VULKAN_SDK_PATH}/Bin/glslc ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/text.vert
                    -o ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/text_vert.spv
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/text.vert
  VERBATIM)

  add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/text_frag.spv
  COMMAND $ENV{VULKAN_SDK_PATH}/Bin/glslc ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/text.frag
                    -o ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/text_frag.spv
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/text.frag
  VERBATIM)
else()
message(FATAL_ERROR "VULKAN_SDK_PATH Environment variable is missing!")
endif()