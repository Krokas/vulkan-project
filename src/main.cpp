#include <memory>

#include "device.h"
#include "log.h"
#include "renderer.h"
#include "window.h"

int main() {
  static constexpr int WIDTH = 800;
  static constexpr int HEIGHT = 600;

  static std::shared_ptr<Window> window;
  static std::shared_ptr<Device> device;
  static std::shared_ptr<Renderer> renderer;

  window = std::make_shared<Window>(WIDTH, HEIGHT, "Vulkan project");
  device = std::make_shared<Device>(*window);
  renderer = std::make_shared<Renderer>(*window, *device);

  while (!window->shouldClose()) {
    window->pollEvents();
    // main loop
    if (auto commandBuffer = renderer->beginFrame()) {
      int frameIndex = renderer->getCurrentFrameIndex();
      // RENDER LOOP
      // render calls should go in between render pass calls
      renderer->beginSwapChainRenderPass(commandBuffer);

      renderer->endSwapChainRenderPass(commandBuffer);
      renderer->endFrame();
    }
  }
  // Before killing everything wait until GPU finishes it's tasks.
  device->waitIdle();

  return EXIT_SUCCESS;
}