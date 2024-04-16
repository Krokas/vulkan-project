#include <chrono>
#include <memory>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "UISystem.h"
#include "descriptor.h"
#include "device.h"
#include "log.h"
#include "renderInfo.h"
#include "renderer.h"
#include "textComponent.h"
#include "window.h"

void initGlobalPool(Device& device, DescriptorInfo* descriptorInfo) {
  descriptorInfo->globalPool =
      DescriptorPool::Builder(device)
          .setMaxSets((uint32_t)(SwapChain::MAX_FRAMES_IN_FLIGHT * 4))
          .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
          //.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, SwapChain::MAX_FRAMES_IN_FLIGHT)
          .build();
}

void initGlobalDescriptors(Device& device, DescriptorInfo* descriptorInfo) {
  descriptorInfo->globalDescriptorSets.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
  descriptorInfo->uboBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

  for (int i = 0; i < descriptorInfo->uboBuffers.size(); i++) {
    descriptorInfo->uboBuffers[i] = std::make_unique<Buffer>(
        device,
        sizeof(GlobalUbo),
        1,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

    descriptorInfo->uboBuffers[i]->map();
  }

  descriptorInfo->globalSetLayout =
      DescriptorSetLayout::Builder(device)
          .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
          .build();

  std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
  for (int i = 0; i < globalDescriptorSets.size(); i++) {
    auto bufferInfo = descriptorInfo->uboBuffers[i]->descriptorInfo();
    DescriptorWriter(*descriptorInfo->globalSetLayout, *descriptorInfo->globalPool)
        .writeBuffer(0, &bufferInfo)
        .build(globalDescriptorSets[i]);
  }
}

void updateGlobalUbo(Camera& camera, DescriptorInfo* descriptorInfo, int frameIndex) {
  GlobalUbo ubo{};
  ubo.projection = camera.getProjection();
  ubo.view = camera.getView();
  ubo.inverseView = camera.getInverseView();
  ubo.directionalLight = {4.0f, 3.0f, 0.0f};

  descriptorInfo->uboBuffers[frameIndex]->writeToBuffer(&ubo);
  descriptorInfo->uboBuffers[frameIndex]->flush();
}

int main() {
  static constexpr int WIDTH = 800;
  static constexpr int HEIGHT = 600;
  static constexpr float MAX_FRAME_TIME = 1.0f;

  static std::shared_ptr<Window> window;
  static std::shared_ptr<Device> device;
  static std::shared_ptr<Renderer> renderer;
  static DescriptorInfo descriptorInfo{};
  descriptorInfo.uboBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

  Camera camera{};
  GameObject::Map gameObjects;

  window = std::make_shared<Window>(WIDTH, HEIGHT, "Vulkan project");
  device = std::make_shared<Device>(*window);
  renderer = std::make_shared<Renderer>(*window, *device);
  initGlobalPool(*device, &descriptorInfo);
  initGlobalDescriptors(*device, &descriptorInfo);

  UISystem uiSystem{
      *device,
      *window,
      renderer->getSwapChainRenderPass(),
      descriptorInfo.globalSetLayout->getDescriptorSetLayout()};

  auto currentTime = std::chrono::high_resolution_clock::now();

  GameObject text = GameObject::createGameObject();
  text.text = std::make_unique<TextComponent>();
  unsigned int textId = text.getId();

  std::vector<Model::Vertex> vertices{};

  vertices.resize(3);
  std::shared_ptr<Model> model = Model::createTextFromData(*device, vertices);
  text.model = model;
  gameObjects.emplace(textId, std::move(text));

  while (!window->shouldClose()) {
    window->pollEvents();

    auto newTime = std::chrono::high_resolution_clock::now();
    auto frameTime =
        std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();

    frameTime = glm::min(frameTime, MAX_FRAME_TIME);

    currentTime = newTime;
    // main loop
    if (auto commandBuffer = renderer->beginFrame()) {
      int frameIndex = renderer->getCurrentFrameIndex();
      FrameInfo frameInfo{
          frameIndex,
          frameTime,
          commandBuffer,
          window->getExtent(),
          *device,
          camera,
          descriptorInfo.globalDescriptorSets[frameIndex],
          gameObjects};
      // Copy Uniform data to GPU
      updateGlobalUbo(camera, &descriptorInfo, frameIndex);

      // RENDER LOOP
      // render calls should go in between render pass calls
      renderer->beginSwapChainRenderPass(commandBuffer);

      uiSystem.render(frameInfo);

      renderer->endSwapChainRenderPass(commandBuffer);
      renderer->endFrame();
    }
  }
  // Before killing everything wait until GPU finishes it's tasks.
  device->waitIdle();

  return EXIT_SUCCESS;
}