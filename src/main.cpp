#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <memory>

#include "log.h"
#include "window.h"

int main() {
  static constexpr int WIDTH = 800;
  static constexpr int HEIGHT = 600;

  static std::shared_ptr<Window> window;

  window = std::make_shared<Window>(WIDTH, HEIGHT, "Vulkan project");

  while (!window->shouldClose()) {
    window->pollEvents();
    // main loop
  }
}