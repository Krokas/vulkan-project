#include "renderer.h"

#include <array>
#include <stdexcept>

Renderer::Renderer(Window& window, Device& device) : window{window}, device{device} {
  recreateSwapChain();
  createCommandBuffers();
}

Renderer::~Renderer() { freeCommandBuffers(); }

void Renderer::createCommandBuffers() {
  commandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = device.getCommandPool();
  allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

  if (vkAllocateCommandBuffers(device.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
    R_ASSERT(false, "Failed to allocate command buffers!");
  }
}

void Renderer::freeCommandBuffers() {
  vkFreeCommandBuffers(
      device.device(),
      device.getCommandPool(),
      static_cast<uint32_t>(commandBuffers.size()),
      commandBuffers.data());
  commandBuffers.clear();
}

void Renderer::recreateSwapChain() {
  auto extent = window.getExtent();
  while (extent.width == 0 || extent.height == 0) {
    extent = window.getExtent();
    glfwWaitEvents();
  }
  vkDeviceWaitIdle(device.device());

  if (swapChain == nullptr) {
    swapChain = std::make_unique<SwapChain>(device, extent);
  } else {
    std::shared_ptr<SwapChain> oldSwapChain = std::move(swapChain);
    swapChain = std::make_unique<SwapChain>(device, extent, oldSwapChain);

    if (!oldSwapChain->compareSwapFormats(*swapChain.get())) {
      R_ASSERT(false, "Swap chain image or depth has chainged");
    }
  }
}

VkCommandBuffer Renderer::beginFrame() {
  R_ASSERT(!isFrameStarted, "cannot begin frame when the frame is already in progress");

  auto result = swapChain->acquireNextImage(&currentImageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    recreateSwapChain();
    return nullptr;
  }

  if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    R_ASSERT(false, "failed to aquire swap chain image!");
  }

  isFrameStarted = true;

  auto commandBuffer = getCurrentCommandBuffer();

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
    R_ASSERT(false, "Command buffer failed to begin recording!");
  }

  return commandBuffer;
}

void Renderer::endFrame() {
  R_ASSERT(isFrameStarted, "Can't call endFrame when frame is not in progress");
  auto commandBuffer = getCurrentCommandBuffer();

  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    R_ASSERT(false, "Failed to end recording to command buffer!");
  }

  auto result = swapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      window.wasWindowResized()) {
    window.resetWindowResizedFlag();
    recreateSwapChain();
  } else if (result != VK_SUCCESS) {
    R_ASSERT(false, "failed to present swap chain image");
  }

  isFrameStarted = false;
  currentFrameIndex = (currentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
}
void Renderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer, glm::vec4 clearColor) {
  R_ASSERT(isFrameStarted, "Cant call beginSwapChainRenderPass when frame is not in progress");
  R_ASSERT(
      commandBuffer == getCurrentCommandBuffer(),
      "Cant begin render pass on command buffer from a different frame");

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = swapChain->getRenderPass();
  renderPassInfo.framebuffer = swapChain->getFrameBuffer(currentImageIndex);

  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = swapChain->getSwapChainExtent();

  std::array<VkClearValue, 2> clearValues{};

  clearValues[0].color = {clearColor.r, clearColor.g, clearColor.b, clearColor.a};
  clearValues[1].depthStencil = {1.0f, 0};
  renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassInfo.pClearValues = clearValues.data();

  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(swapChain->getSwapChainExtent().width);
  viewport.height = static_cast<float>(swapChain->getSwapChainExtent().height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  VkRect2D scissor{{0, 0}, swapChain->getSwapChainExtent()};
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}
void Renderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
  R_ASSERT(isFrameStarted, "Cant call beginSwapChainRenderPass when frame is not in progress");
  R_ASSERT(
      commandBuffer == getCurrentCommandBuffer(),
      "Cant end render pass on command buffer from a different frame");

  vkCmdEndRenderPass(commandBuffer);
}
