#include <chrono>
#include <memory>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "TextSystem.h"
#include "allocator.h"
#include "color.h"
#include "descriptor.h"
#include "device.h"
#include "font.h"
#include "input.h"
#include "log.h"
#include "renderInfo.h"
#include "renderer.h"
#include "textComponent.h"
#include "timer.h"
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

void updateTextUbo(TextSystem& textSystem, DescriptorInfo* descriptorInfo, int frameIndex) {
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

unsigned int addText(
    std::shared_ptr<Font> font,
    std::shared_ptr<Window> window,
    std::shared_ptr<Device> device,
    GameObject::Map* gameObjects,
    std::string& textReference,
    const glm::vec2& position,
    const glm::vec3& color,
    bool outline,
    const glm::vec3& outlineColor) {
  GameObject text = GameObject::createGameObject();
  text.text = std::make_unique<TextComponent>();
  text.text->font = std::move(font);
  text.text->text = &textReference;
  text.text->color = hexColorToUnitary(color);
  text.text->outlineColor = hexColorToUnitary(outlineColor);
  text.text->outline = outline;
  unsigned int textId = text.getId();
  std::vector<Model::Instance> instances{};
  instances.resize(TextSystem::MAX_TEXT_LENGTH);
  text.text->position = TextSystem::getScreenCoordinates(window->getExtent(), position);
  std::shared_ptr<Model> model = Model::createModelFromTextData(
      *device,
      &text.text->position,
      text.text->color,
      text.text->outlineColor,
      text.text->outline,
      instances);

  text.model = model;
  gameObjects->emplace(textId, std::move(text));
  return textId;
}

int main() {
  MemoryAllocator transiantStorage = makeAllocator(MB(100));

  static constexpr int WIDTH = 800;
  static constexpr int HEIGHT = 600;

  static std::shared_ptr<Window> window;
  static std::shared_ptr<Device> device;
  static std::shared_ptr<Renderer> renderer;
  static std::shared_ptr<Input> input;
  static std::shared_ptr<Timer> timer;
  static DescriptorInfo descriptorInfo{};
  descriptorInfo.uboBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
  static std::shared_ptr<Font> debugFont;

  Camera camera{};
  GameObject::Map gameObjects;

  window = std::make_shared<Window>(WIDTH, HEIGHT, "Vulkan project");
  device = std::make_shared<Device>(*window);
  renderer = std::make_shared<Renderer>(*window, *device);
  input = std::make_shared<Input>(window->getGlfwWindow());
  debugFont = std::make_shared<Font>(device.get(), &transiantStorage, ARIAL, 16);
  debugFont->prepare();

  initGlobalPool(*device, &descriptorInfo);
  initGlobalDescriptors(*device, &descriptorInfo);
  initTextImageDescriptor(*device, &descriptorInfo, debugFont.get());

  std::string debugText = "pirmas blynas!";
  std::string fps = "0";

  std::string pressedTest = "";
  std::string mousePosition = "";

  TextSystem TextSystem{
      *device,
      *window,
      renderer->getSwapChainRenderPass(),
      descriptorInfo.textSetLayout->getDescriptorSetLayout(),
      &descriptorInfo};

  timer = std::make_shared<Timer>();

  addText(
      debugFont,
      window,
      device,
      &gameObjects,
      debugText,
      {0.0f, 15.0f},
      {255, 255, 255},
      false,
      {80, 20, 80});

  addText(
      debugFont,
      window,
      device,
      &gameObjects,
      fps,
      {0.0f, 30.0f},
      {255, 255, 255},
      false,
      {80, 20, 80});

  addText(
      debugFont,
      window,
      device,
      &gameObjects,
      pressedTest,
      {100.0f, 100.0f},
      {255, 255, 255},
      true,
      {255, 0, 0});

  auto mousePositionObjId = addText(
      debugFont,
      window,
      device,
      &gameObjects,
      mousePosition,
      {100, 100},
      {80, 50, 200},
      false,
      {255, 255, 255});

  while (!window->shouldClose()) {
    window->pollEvents();

    timer->update();

    // main loop
    if (auto commandBuffer = renderer->beginFrame()) {
      int frameIndex = renderer->getCurrentFrameIndex();
      FrameInfo frameInfo{
          frameIndex,
          *timer,
          commandBuffer,
          window->getExtent(),
          *device,
          camera,
          descriptorInfo.globalDescriptorSets[frameIndex],
          gameObjects};

      debugText = "frame time: " + std::to_string(timer->getFrameTime());
      fps = "fps: " + std::to_string(timer->getFPS());
      mousePosition = std::to_string((int)input->getMousePosition().x) + ", " +
                      std::to_string((int)input->getMousePosition().y);

      gameObjects.at(mousePositionObjId).text->position = TextSystem::getScreenCoordinates(
          window->getExtent(),
          {input->getMousePosition().x + 5.0f, input->getMousePosition().y + 25.0f});

      if (input->isKeyPressed(87)) {
        pressedTest = (char)87;
      } else {
        pressedTest = "";
      }

      input->update();
      TextSystem.update(frameInfo);
      // Copy Uniform data to GPU
      updateGlobalUbo(camera, &descriptorInfo, frameIndex);
      updateTextUbo(TextSystem, &descriptorInfo, frameIndex);

      // RENDER LOOP
      // render calls should go in between render pass calls
      renderer->beginSwapChainRenderPass(commandBuffer);

      TextSystem.render(frameInfo);

      renderer->endSwapChainRenderPass(commandBuffer);
      renderer->endFrame();
    }
  }
  // Before killing everything wait until GPU finishes it's tasks.
  device->waitIdle();

  return EXIT_SUCCESS;
}