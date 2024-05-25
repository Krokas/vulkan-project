#include "UISystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <map>
#include <stdexcept>

#include "file.h"
#include "font.h"

UISystem::UISystem(
    Device& device,
    Window& window,
    VkRenderPass renderPass,
    VkDescriptorSetLayout globalSetLayout,
    DescriptorInfo* descriptorInfo)
    : device{device}, window{window}, descriptorInfo{descriptorInfo} {
  createPipelineLayout(globalSetLayout);
  createPipeline(renderPass);
}

UISystem::~UISystem() { vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr); }

void UISystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
  /*VkPushConstantRange pushConstantRange{};
  pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
  pushConstantRange.offset = 0;
  pushConstantRange.size = sizeof(PointLightPushConstants);*/

  std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

  VkPipelineLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  layoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
  layoutInfo.pSetLayouts = descriptorSetLayouts.data();
  layoutInfo.pushConstantRangeCount = 0;
  layoutInfo.pPushConstantRanges = nullptr;

  if (vkCreatePipelineLayout(device.device(), &layoutInfo, nullptr, &pipelineLayout) !=
      VK_SUCCESS) {
    throw std::runtime_error("Failed to create pipeline layout!");
  }
}

void UISystem::createPipeline(VkRenderPass renderPass) {
  assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");
  PipelineConfigInfo pipelineConfig{};
  Pipeline::defaultPipelineConfigInfo(pipelineConfig);
  Pipeline::enableAlphaBlending(pipelineConfig);
  /*pipelineConfig.attributeDescriptions.clear();
  pipelineConfig.bindingDescriptions.clear();*/
  pipelineConfig.renderPass = renderPass;
  pipelineConfig.pipelineLayout = pipelineLayout;
  pipelineConfig.bindingDescriptions = Model::Vertex::getBindingDescriptions(true);
  pipelineConfig.attributeDescriptions = Model::Vertex::getAttributeDescriptions(true);

  pipeline = std::make_unique<Pipeline>(device, "text_vert.spv", "text_frag.spv", pipelineConfig);
}

void UISystem::update(FrameInfo& frameInfo) {
  for (auto& kv : frameInfo.gameObjects) {
    auto& obj = kv.second;
    if (obj.text == nullptr) continue;
    if (obj.model == nullptr) continue;
    // obj.model->~Model();

    unsigned int key = kv.first;

    if (obj.text->text.length() > 0) {
      std::vector<Model::Instance> instanceData{};
      instanceData.resize(MAX_TEXT_LENGTH);
      glm::vec2 startPos = obj.text->position;
      for (int j = 0; j < MAX_TEXT_LENGTH; j++) {
        instanceData[j].isVisible = false;
      }
      std::string text = obj.text->text;
      for (int i = 0; i < obj.text->text.length(); i++) {
        glm::ivec2 glyphSize = obj.text->font->getGlyphSize(text[i]);
        glm::vec2 glyphOffset = obj.text->font->getGlyphOffset(text[i]);
        glm::vec2 glyphAdvance = obj.text->font->getGlyphAdvance(text[i]);
        int textureWidth = obj.text->font->getTextureWidth();

        instanceData[i].isVisible = true;
        instanceData[i].size = {0.1f, 0.3f, 0.0f};

        // R_WARN(
        //     "character %c size: x: %i, y: %i; offset: x: %f, y: %f; advance: x: %f, y: %f",
        //     text[i],
        //     glyphSize.x,
        //     glyphSize.y,
        //     glyphOffset.x,
        //     glyphOffset.y,
        //     glyphAdvance.x,
        //     glyphAdvance.y);
      }
      // R_TRACE("The end! Starting position: {%f, %f}", startPos.x, startPos.y);
      obj.model =
          Model::createModelFromTextData(frameInfo.device, obj.text->position, instanceData);
    }

    // auto textPair = frameInfo.texts.find(key);
    // auto vertices =
    //     obj.text->font->getVertices(textPair->second, obj.text->position,
    //     frameInfo.windowExtent);

    // std::shared_ptr<Model> model = Model::createTextFromData(frameInfo.device, vertices);
    // obj.model = model;
    // font.
  }
}

void UISystem::render(FrameInfo& frameInfo) {
  pipeline->bind(frameInfo.commandBuffer);

  std::vector<VkDescriptorSet> descriptorSets{
      descriptorInfo->textDescriptorSets[frameInfo.frameIndex]};

  vkCmdBindDescriptorSets(
      frameInfo.commandBuffer,
      VK_PIPELINE_BIND_POINT_GRAPHICS,
      pipelineLayout,
      0,
      static_cast<uint32_t>(descriptorSets.size()),
      descriptorSets.data(),
      0,
      nullptr);

  for (auto& kv : frameInfo.gameObjects) {
    auto& obj = kv.second;
    if (obj.text == nullptr) continue;
    if (obj.model == nullptr) continue;

    // ubo.color = obj.text->color;
    // ubo.position = obj.text->position;
    obj.transform.translation = glm::vec3(obj.text->position.x, 1.0f, obj.text->position.y);

    obj.model->bind(frameInfo.commandBuffer);
    // obj.model->draw(frameInfo.commandBuffer);
    obj.model->drawInstanced(frameInfo.commandBuffer, MAX_TEXT_LENGTH);
  }
}

glm::vec2 UISystem::getScreenCoordinates(
    const VkExtent2D& swapChainExtent, const glm::vec2& topLeftOffset) {
  glm::vec2 trueCoords{};

  trueCoords.x = ((topLeftOffset.x * 2.0f) / swapChainExtent.width) - 1.0f;
  trueCoords.y = ((topLeftOffset.y * 2.0f) / swapChainExtent.height) - 1.0f;

  return trueCoords;
}
