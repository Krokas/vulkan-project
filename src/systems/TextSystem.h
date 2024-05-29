#pragma once

#include <memory>
#include <vector>

#include "camera.h"
#include "device.h"
#include "font.h"
#include "gameObject.h"
#include "pipeline.h"
#include "renderInfo.h"

struct TextUbo {
  glm::vec3 color{1.0f};
  // alignas(16) glm::vec2 position{0.0f};
};

class TextSystem {
 public:
  // TODO: consider making bigger.
  static const unsigned int MAX_TEXT_LENGTH = 500;

  using Map = std::unordered_map<std::string, Font>;

  TextSystem(
      Device& device,
      Window& window,
      VkRenderPass renderPass,
      VkDescriptorSetLayout globalSetLayout,
      DescriptorInfo* descriptorInfo);
  ~TextSystem();

  TextSystem(const TextSystem&) = delete;
  TextSystem& operator=(const TextSystem&) = delete;

  TextUbo* getUbo() { return &ubo; }
  void update(FrameInfo& frameInfo);
  void render(FrameInfo& frameInfo);
  static glm::vec2 getScreenCoordinates(
      const VkExtent2D& swapChainExtent, const glm::vec2& topLeftOffset);
  void updateText(unsigned int objectId, const std::string& text);

 private:
  void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
  void createPipeline(VkRenderPass renderPass);

  Device& device;
  Window& window;
  TextUbo ubo{};
  DescriptorInfo* descriptorInfo = nullptr;

  std::unique_ptr<Pipeline> pipeline;
  VkPipelineLayout pipelineLayout;

  Map fonts{};
};
