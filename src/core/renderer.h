#pragma once

#include <glm/vec4.hpp>
#include <memory>
#include <vector>

#include "device.h"
#include "log.h"
#include "swapChain.h"
#include "window.h"

class Renderer {
 public:
  Renderer(Window& window, Device& device);
  ~Renderer();

  Renderer(const Renderer&) = delete;
  Renderer& operator=(const Renderer&) = delete;

  VkRenderPass getSwapChainRenderPass() const { return swapChain->getRenderPass(); }
  float getAspectRatio() const { return swapChain->extentAspectRatio(); }

  bool isFrameInProgress() const { return isFrameStarted; }
  VkCommandBuffer getCurrentCommandBuffer() const {
    R_ASSERT(isFrameStarted, "cannot getframe command buffer when frame is not in progress");
    return commandBuffers[currentFrameIndex];
  }

  int getCurrentFrameIndex() const {
    R_ASSERT(isFrameStarted, "cannot call getCurrentFrameIndex when frame is not in progress");
    return currentFrameIndex;
  }

  VkCommandBuffer beginFrame();
  void endFrame();
  void beginSwapChainRenderPass(
      VkCommandBuffer commandBuffer, glm::vec4 clearColor = {0.01f, 0.01f, 0.01f, 1.0f});
  void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

 private:
  void createCommandBuffers();
  void freeCommandBuffers();
  void recreateSwapChain();

  Window& window;
  Device& device;
  std::unique_ptr<SwapChain> swapChain;
  std::vector<VkCommandBuffer> commandBuffers;

  uint32_t currentImageIndex;
  int currentFrameIndex = 0;
  bool isFrameStarted = false;
};
