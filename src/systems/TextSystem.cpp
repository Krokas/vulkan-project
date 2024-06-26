#include "TextSystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <map>
#include <stdexcept>

#include "file.h"
#include "font.h"

TextSystem::TextSystem(
    Device& device, Window& window, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout, DescriptorInfo* descriptorInfo)
    : device{device}, window{window}, descriptorInfo{descriptorInfo} {
  createPipelineLayout(globalSetLayout);
  createPipeline(renderPass);
}

TextSystem::~TextSystem() { vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr); }

void TextSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
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

  if (vkCreatePipelineLayout(device.device(), &layoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create pipeline layout!");
  }
}

void TextSystem::createPipeline(VkRenderPass renderPass) {
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

void TextSystem::update(FrameInfo& frameInfo) {
  for (auto& kv : frameInfo.gameObjects) {
    auto& obj = kv.second;
    if (obj.text == nullptr) continue;
    if (obj.model == nullptr) continue;
    // obj.model->~Model();

    unsigned int key = kv.first;

    if (obj.text->text->length() > 0) {
      std::vector<Model::Instance> instanceData{};
      instanceData.resize(MAX_TEXT_LENGTH);
      glm::vec2 startPos = obj.text->position;
      std::string text = *obj.text->text;
      int textureWidth = obj.text->font->getTextureWidth();

      glm::vec2 textureToSceen = {
          (float)textureWidth / (float)frameInfo.windowExtent.width,
          (float)textureWidth / (float)frameInfo.windowExtent.height};

      glm::vec2 screenToTexture = {
          (float)frameInfo.windowExtent.width / (float)textureWidth,
          (float)frameInfo.windowExtent.height / (float)textureWidth};

      glm::vec2 nextPosition{0.0f};

      for (int i = 0; i < obj.text->text->length(); i++) {
        glm::ivec2 glyphSize = obj.text->font->getGlyphSize(text[i]);
        glm::vec2 glyphOffset = obj.text->font->getGlyphOffset(text[i]);
        glm::vec2 glyphAdvance = obj.text->font->getGlyphAdvance(text[i]);
        glm::ivec2 glyphTexturePos = obj.text->font->getTextureCoords(text[i]);

        float offset = (float)glyphOffset.x + 10.0f;
        if (obj.text->outline) {
          offset += 4.0f;
        }

        instanceData[i].position = {
            ((float)nextPosition.x + offset) * (float)screenToTexture.x / (float)frameInfo.windowExtent.width,
            ((float)nextPosition.y - (float)glyphOffset.y) * (float)screenToTexture.y / (float)frameInfo.windowExtent.height,
            0.0f};

        nextPosition += glm::vec2((float)glyphAdvance.x + offset, (float)glyphAdvance.y);

        instanceData[i].textureSize = {(float)glyphSize.x / (float)textureWidth, (float)glyphSize.y / (float)textureWidth};

        instanceData[i].texturePos = {(float)glyphTexturePos.x / (float)textureWidth, (float)glyphTexturePos.y / (float)textureWidth};

        instanceData[i].size = {
            (float)glyphSize.x * (float)screenToTexture.x / (float)frameInfo.windowExtent.width,
            (float)glyphSize.y * (float)screenToTexture.y / (float)frameInfo.windowExtent.height,
            0.0f};
      }

      if (obj.text->text->length() > 0) {
        obj.text->isVisible = true;
      } else {
        obj.text->isVisible = false;
      }

      obj.model = Model::createModelFromTextData(
          frameInfo.device,
          &obj.text->position,
          obj.text->color,
          obj.text->outlineColor,
          obj.text->outline,
          instanceData,
          obj.text->isVisible);
    }

    if (obj.text->text->length() == 0 && obj.text->isVisible) {
      obj.text->isVisible = false;
      std::vector<Model::Instance> instanceData{};
      instanceData.resize(1);
      obj.model = Model::createModelFromTextData(
          frameInfo.device,
          &obj.text->position,
          obj.text->color,
          obj.text->outlineColor,
          obj.text->outline,
          instanceData,
          obj.text->isVisible);
    }
  }
}

void TextSystem::render(FrameInfo& frameInfo) {
  pipeline->bind(frameInfo.commandBuffer);

  std::vector<VkDescriptorSet> descriptorSets{descriptorInfo->textDescriptorSets[frameInfo.frameIndex]};

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

glm::vec2 TextSystem::getScreenCoordinates(const VkExtent2D& swapChainExtent, const glm::vec2& topLeftOffset) {
  glm::vec2 trueCoords{};

  trueCoords.x = ((topLeftOffset.x * 2.0f) / swapChainExtent.width) - 1.0f;
  trueCoords.y = ((topLeftOffset.y * 2.0f) / swapChainExtent.height) - 1.0f;

  return trueCoords;
}

glm::vec2 TextSystem::getScreenScreenSize(const VkExtent2D& swapChainExtent, const glm::vec2& pixelSize) {
  glm::vec2 screenSize{};

  screenSize.x = (pixelSize.x * 2.0f) / swapChainExtent.width;
  screenSize.y = (pixelSize.y * 2.0f) / swapChainExtent.height;

  return screenSize;
}

glm::ivec2 TextSystem::getPixelPosition(const VkExtent2D& swapchainExtent, const glm::vec2& windowPosition) {
  glm::ivec2 pixelPosition{};

  pixelPosition.x = ((windowPosition.x + 1.0f) / 2.0f) * swapchainExtent.width;
  pixelPosition.y = ((windowPosition.y + 1.0f) / 2.0f) * swapchainExtent.height;

  return pixelPosition;
}