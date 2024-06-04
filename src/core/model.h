#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include <vector>

#include "buffer.h"
#include "device.h"

class Model {
 public:
  struct Vertex {
    glm::vec3 position{};
    glm::vec3 color{};
    glm::vec3 normal{};
    glm::vec2 uv{};

    static std::vector<VkVertexInputBindingDescription> getBindingDescriptions(bool isInstanced = false);
    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions(bool isInstanced = false);

    bool operator==(const Vertex& other) const {
      return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
    }
  };

  struct Instance {
    glm::vec3 position{};
    glm::vec3 size{};
    glm::vec2 textureSize{};
    glm::vec2 texturePos{};
  };

  struct Builder {
    std::vector<Vertex> vertices{};
    std::vector<uint32_t> indices{};
    std::vector<Instance> instances{};
    bool isInstanced = false;

    void loadModel(const std::string& filename);
  };

  Model(Device& device, const Model::Builder& builder);
  ~Model();

  Model(const Model&) = delete;
  Model& operator=(const Model&) = delete;

  static std::unique_ptr<Model> createModelFromFile(Device& device, const std::string& filename);
  static std::unique_ptr<Model> createFromData(Device& device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
  static std::unique_ptr<Model> createModelFromTextData(
      Device& device,
      glm::vec2* position,
      glm::vec3& color,
      glm::vec3& outlineColor,
      bool outline,
      const std::vector<Instance>& instances,
      bool isVisible = true);
  static std::unique_ptr<Model> createQuad(Device& device, const glm::vec2& position, const glm::vec2& size, const glm::vec3 color);

  void bind(VkCommandBuffer commandBuffer);
  void draw(VkCommandBuffer commandBuffer);
  void drawInstanced(VkCommandBuffer commandBuffer);
  void drawInstanced(VkCommandBuffer commandBuffer, int instanceCount);

 private:
  void createVertexBuffers(const std::vector<Vertex>& vertices);
  void createIndexBuffers(const std::vector<uint32_t>& indices);
  void createInstaceBuffers(const std::vector<Instance>& instances);
  static std::vector<unsigned int> getQuadIndices(unsigned int startingIndex);
  static std::vector<unsigned int> getTextQuadIndices(unsigned int startingIndex);

  Device& device;

  std::unique_ptr<Buffer> vertexBuffer;
  uint32_t vertexCount;

  bool hasIndexBuffer = false;
  std::unique_ptr<Buffer> indexBuffer;
  uint32_t indexCount;

  bool isInstanced = false;
  std::unique_ptr<Buffer> instanceBuffer;
  uint32_t instanceCount;
};
