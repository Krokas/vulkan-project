#pragma once

#include <vulkan/vulkan.h>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <unordered_map>

#include "camera.h"
#include "descriptor.h"
#include "device.h"
#include "gameObject.h"
#include "swapChain.h"
#include "timer.h"

#define MAX_LIGHTS 10

struct PointLight {
  glm::vec4 position{};
  glm::vec4 color{};
};

struct FrameInfo {
  int frameIndex;
  Timer& timer;
  VkCommandBuffer commandBuffer;
  VkExtent2D windowExtent;
  Device& device;
  Camera& camera;
  VkDescriptorSet globalDescriptorSet;
  GameObject::Map& gameObjects;
};

struct GlobalUbo {
  glm::mat4 projection{1.0f};
  glm::mat4 view{1.0f};
  glm::mat4 inverseView{1.0f};
  glm::vec4 ambientColor{1.f, 1.f, 1.f, 0.02f};
  PointLight pointLights[MAX_LIGHTS];
  int numLights;
  glm::vec3 directionalLight{0.0f, 50.f, 0.0f};
  glm::vec4 lightColor{1.0f, 1.0f, 1.0f, 1.0f};
};

struct DescriptorInfo {
  std::unique_ptr<DescriptorPool> globalPool{};
  std::vector<std::unique_ptr<Buffer>> uboBuffers{};
  std::vector<VkDescriptorSet> globalDescriptorSets{};
  std::unique_ptr<DescriptorSetLayout> globalSetLayout;

  // text
  std::unique_ptr<DescriptorSetLayout> textSetLayout;
  std::vector<std::unique_ptr<Buffer>> textBuffers{};
  std::vector<VkDescriptorSet> textDescriptorSets{};
};