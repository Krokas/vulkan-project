#pragma once
#include "device.h"
#include "pipeline.h"
#include "renderInfo.h"

class UISystem {
 public:
  UISystem(
      Device& device,
      Window& window,
      VkRenderPass renderPass,
      VkDescriptorSetLayout globalSetLayout,
      DescriptorInfo* descriptorInfo);
  ~UISystem();
  void update(FrameInfo& frameInfo);
  void render(FrameInfo& frameInfo);

 private:
  void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
  void createPipeline(VkRenderPass renderPass);

  Device& device;
  Window& window;

  std::unique_ptr<Pipeline> pipeline;
  VkPipelineLayout pipelineLayout;
  DescriptorInfo* descriptorInfo = nullptr;
};