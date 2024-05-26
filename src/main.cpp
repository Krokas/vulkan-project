#include <chrono>
#include <memory>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "UISystem.h"
#include "allocator.h"
#include "descriptor.h"
#include "device.h"
#include "font.h"
#include "log.h"
#include "renderInfo.h"
#include "renderer.h"
#include "textComponent.h"
#include "utils.h"
#include "window.h"

void initGlobalPool(Device& device, DescriptorInfo* descriptorInfo) {
  descriptorInfo->globalPool =
      DescriptorPool::Builder(device)
          .setMaxSets((uint32_t)(SwapChain::MAX_FRAMES_IN_FLIGHT * 4))
          .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
          .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
          .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, SwapChain::MAX_FRAMES_IN_FLIGHT)
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

  for (int i = 0; i < descriptorInfo->globalDescriptorSets.size(); i++) {
    auto bufferInfo = descriptorInfo->uboBuffers[i]->descriptorInfo();
    DescriptorWriter(*descriptorInfo->globalSetLayout, *descriptorInfo->globalPool)
        .writeBuffer(0, &bufferInfo)
        .build(descriptorInfo->globalDescriptorSets[i]);
  }
}

void initTextImageDescriptor(Device& device, DescriptorInfo* descriptorInfo, Font* font) {
  descriptorInfo->textBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
  descriptorInfo->textDescriptorSets.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
  for (int i = 0; i < descriptorInfo->textBuffers.size(); i++) {
    descriptorInfo->textBuffers[i] = std::make_unique<Buffer>(
        device,
        sizeof(TextUbo),
        1,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

    descriptorInfo->textBuffers[i]->map();
  }

  descriptorInfo->textSetLayout =
      DescriptorSetLayout::Builder(device)
          .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
          .addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
          .build();

  for (int i = 0; i < descriptorInfo->textDescriptorSets.size(); i++) {
    auto bufferInfo = descriptorInfo->textBuffers[i]->descriptorInfo();
    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = font->getTexture()->getImageView();
    imageInfo.sampler = font->getTexture()->getSampler();
    DescriptorWriter(*descriptorInfo->textSetLayout, *descriptorInfo->globalPool)
        .writeBuffer(0, &bufferInfo)
        .writeImage(1, &imageInfo)
        .build(descriptorInfo->textDescriptorSets[i]);
  }
}

void updateTextUbo(UISystem& textSystem, DescriptorInfo* descriptorInfo, int frameIndex) {
  descriptorInfo->textBuffers[frameIndex]->writeToBuffer(textSystem.getUbo());
  descriptorInfo->textBuffers[frameIndex]->flush();
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
  MemoryAllocator transiantStorage = makeAllocator(MB(50));

  static constexpr int WIDTH = 800;
  static constexpr int HEIGHT = 600;
  static constexpr float MAX_FRAME_TIME = 1.0f;

  static std::shared_ptr<Window> window;
  static std::shared_ptr<Device> device;
  static std::shared_ptr<Renderer> renderer;
  static DescriptorInfo descriptorInfo{};
  descriptorInfo.uboBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
  static std::shared_ptr<Font> debugFont;

  Camera camera{};
  GameObject::Map gameObjects;

  window = std::make_shared<Window>(WIDTH, HEIGHT, "Vulkan project");
  device = std::make_shared<Device>(*window);
  renderer = std::make_shared<Renderer>(*window, *device);
  debugFont = std::make_shared<Font>(device.get(), &transiantStorage, SOMETYPE_MONO_REGULAR, 50);
  debugFont->prepare();

  initGlobalPool(*device, &descriptorInfo);
  initGlobalDescriptors(*device, &descriptorInfo);
  initTextImageDescriptor(*device, &descriptorInfo, debugFont.get());

  UISystem uiSystem{
      *device,
      *window,
      renderer->getSwapChainRenderPass(),
      descriptorInfo.textSetLayout->getDescriptorSetLayout(),
      &descriptorInfo};

  auto currentTime = std::chrono::high_resolution_clock::now();

  // TODO: This is way too much lines to render a single triangle/square.

  // THE WAY TO DO IT:
  // 1. Set a <Font> pointer on a text component.
  // 2. Set a text <std::string> to text component. And position <glm::vec2>. TODO: extra options
  // later like outline and text color.
  // 3. update() data for rendering the text using instanced rendering.

  GameObject text = GameObject::createGameObject();
  text.text = std::make_unique<TextComponent>();
  text.text->font = std::move(debugFont);
  text.text->text = "Rokas!";
  unsigned int textId = text.getId();
  std::vector<Model::Instance> instances{};
  instances.resize(UISystem::MAX_TEXT_LENGTH);
  text.text->position = UISystem::getScreenCoordinates(window->getExtent(), {10.0f, 30.0f});
  std::shared_ptr<Model> model =
      Model::createModelFromTextData(*device, text.text->position, instances);

  text.model = model;
  gameObjects.emplace(textId, std::move(text));

  // Text setup end

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
      uiSystem.update(frameInfo);
      // Copy Uniform data to GPU
      updateGlobalUbo(camera, &descriptorInfo, frameIndex);
      updateTextUbo(uiSystem, &descriptorInfo, frameIndex);

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