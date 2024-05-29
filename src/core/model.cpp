#include "model.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include "file.h"
#include "hash.h"

// std
#include <cassert>
#include <cstring>
#include <unordered_map>

namespace std {
template <>
struct hash<Model::Vertex> {
  size_t operator()(Model::Vertex const& vertex) const {
    size_t seed = 0;
    hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
    return seed;
  }
};
}  // namespace std

Model::Model(Device& device, const Model::Builder& builder)
    : device{device}, isInstanced{builder.isInstanced} {
  createVertexBuffers(builder.vertices);
  createIndexBuffers(builder.indices);
  if (isInstanced) {
    createInstaceBuffers(builder.instances);
  }
}

Model::~Model() {}

void Model::createVertexBuffers(const std::vector<Vertex>& vertices) {
  vertexCount = static_cast<uint32_t>(vertices.size());
  assert(vertexCount >= 3 && "Vertex count must be at least 3");
  VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
  uint32_t vertexSize = sizeof(vertices[0]);

  Buffer stagingBuffer{
      device,
      vertexSize,
      vertexCount,
      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
  };

  stagingBuffer.map();
  stagingBuffer.writeToBuffer((void*)vertices.data());

  vertexBuffer = std::make_unique<Buffer>(
      device,
      vertexSize,
      vertexCount,
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  device.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
}

void Model::createIndexBuffers(const std::vector<uint32_t>& indices) {
  indexCount = static_cast<uint32_t>(indices.size());
  hasIndexBuffer = indexCount > 0;

  if (!hasIndexBuffer) {
    return;
  }

  VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
  uint32_t indexSize = sizeof(indices[0]);

  Buffer stagingBuffer{
      device,
      indexSize,
      indexCount,
      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};

  stagingBuffer.map();
  stagingBuffer.writeToBuffer((void*)indices.data());

  indexBuffer = std::make_unique<Buffer>(
      device,
      indexSize,
      indexCount,
      VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  device.copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);
}

void Model::createInstaceBuffers(const std::vector<Instance>& instances) {
  instanceCount = static_cast<uint32_t>(instances.size());
  assert(instanceCount >= 1 && "Instance count must be at least 1");
  VkDeviceSize bufferSize = sizeof(instances[0]) * instanceCount;
  uint32_t instanceSize = sizeof(instances[0]);

  Buffer stagingBuffer{
      device,
      instanceSize,
      instanceCount,
      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
  };

  stagingBuffer.map();
  stagingBuffer.writeToBuffer((void*)instances.data());

  instanceBuffer = std::make_unique<Buffer>(
      device,
      instanceSize,
      instanceCount,
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  device.copyBuffer(stagingBuffer.getBuffer(), instanceBuffer->getBuffer(), bufferSize);
}

std::unique_ptr<Model> Model::createModelFromFile(Device& device, const std::string& filename) {
  Builder builder{};
  builder.loadModel(filename);
  return std::make_unique<Model>(device, builder);
}

std::unique_ptr<Model> Model::createFromData(
    Device& device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
  Builder builder{};
  builder.vertices = vertices;
  builder.indices = indices;
  return std::make_unique<Model>(device, builder);
}

std::unique_ptr<Model> Model::createModelFromTextData(
    Device& device,
    const glm::vec2& position,
    glm::vec3& color,
    glm::vec3& outlineColor,
    bool outline,
    const std::vector<Instance>& instances,
    bool isVisible) {
  std::vector<Vertex> vertices{};

  vertices.resize(6);
  for (int i = 0; i < vertices.size(); i++) {
    vertices[i].position = {position.x, 1.0f, position.y};
    vertices[i].color = color;
    vertices[i].normal = outlineColor;
    vertices[i].uv = {outline ? 1.0f : 0.0f, isVisible ? 1.0f : 0.0f};
  }

  Builder builder{};
  builder.vertices = vertices;
  builder.instances = instances;
  builder.isInstanced = true;
  return std::make_unique<Model>(device, builder);
}

void Model::draw(VkCommandBuffer commandBuffer) {
  if (hasIndexBuffer) {
    vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
  } else {
    vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
  }
}

void Model::drawInstanced(VkCommandBuffer commandBuffer) {
  if (vertexCount >= 3) {
    vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
  }
}

void Model::drawInstanced(VkCommandBuffer commandBuffer, int instanceCount) {
  vkCmdDraw(commandBuffer, vertexCount, instanceCount, 0, 0);
}

void Model::bind(VkCommandBuffer commandBuffer) {
  VkBuffer buffers[] = {vertexBuffer->getBuffer()};
  VkBuffer instanceBuffers[] = {instanceBuffer->getBuffer()};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

  if (hasIndexBuffer) {
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
  }

  if (isInstanced) {
    vkCmdBindVertexBuffers(commandBuffer, 1, 1, instanceBuffers, offsets);
  }
}

std::vector<VkVertexInputBindingDescription> Model::Vertex::getBindingDescriptions(
    bool isInstanced) {
  unsigned int bindingDescriptionCount = 1;
  if (isInstanced) {
    bindingDescriptionCount = 2;
  }
  std::vector<VkVertexInputBindingDescription> bindingDescriptions(bindingDescriptionCount);
  bindingDescriptions[0].binding = 0;
  bindingDescriptions[0].stride = sizeof(Vertex);
  bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  if (isInstanced) {
    bindingDescriptions[1].binding = 1;
    bindingDescriptions[1].stride = sizeof(Instance);
    bindingDescriptions[1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
  }

  return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> Model::Vertex::getAttributeDescriptions(
    bool isInstanced) {
  std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

  attributeDescriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)});
  attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)});
  attributeDescriptions.push_back({2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)});
  attributeDescriptions.push_back({3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv)});

  if (isInstanced) {
    attributeDescriptions.push_back(
        {4, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Instance, position)});
    attributeDescriptions.push_back({5, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Instance, size)});
    attributeDescriptions.push_back(
        {6, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(Instance, textureSize)});
    attributeDescriptions.push_back(
        {7, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(Instance, texturePos)});
  }

  return attributeDescriptions;
}

void Model::Builder::loadModel(const std::string& filename) {
  std::string filepath = File::getModelFilePath(filename);

  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, error;

  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &error, filepath.c_str())) {
    throw std::runtime_error(warn + error);
  }

  vertices.clear();
  indices.clear();

  std::unordered_map<Vertex, uint32_t> uniqueVertices{};
  for (const auto& shape : shapes) {
    for (const auto& index : shape.mesh.indices) {
      Vertex vertex{};

      if (index.vertex_index >= 0) {
        vertex.position = {
            attrib.vertices[3 * index.vertex_index + 0],
            attrib.vertices[3 * index.vertex_index + 1],
            attrib.vertices[3 * index.vertex_index + 2]};

        vertex.color = {
            attrib.colors[3 * index.vertex_index + 0],
            attrib.colors[3 * index.vertex_index + 1],
            attrib.colors[3 * index.vertex_index + 2]};
      }

      if (index.normal_index >= 0) {
        vertex.normal = {
            attrib.normals[3 * index.normal_index + 0],
            attrib.normals[3 * index.normal_index + 1],
            attrib.normals[3 * index.normal_index + 2]};
      }

      if (index.texcoord_index >= 0) {
        vertex.uv = {
            attrib.texcoords[2 * index.texcoord_index + 0],
            attrib.texcoords[2 * index.texcoord_index + 1]};
      }

      if (uniqueVertices.count(vertex) == 0) {
        uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
        vertices.push_back(vertex);
      }
      indices.push_back(uniqueVertices[vertex]);
    }
  }
}

std::vector<unsigned int> Model::getQuadIndices(unsigned int startingIndex) {
  std::vector<unsigned int> indices;
  for (int i = 0; i < 6; i++) {
    switch (i) {
      case 1:
        indices.push_back(startingIndex + 1);
        break;
      case 2:
      case 3:
        indices.push_back(startingIndex + 2);
        break;
      case 4:
        indices.push_back(startingIndex + 3);
        break;
      default:
        indices.push_back(startingIndex);
    }
  }
  return indices;
}

std::vector<unsigned int> Model::getTextQuadIndices(unsigned int startingIndex) {
  std::vector<unsigned int> indices;
  for (int i = 0; i < 6; i++) {
    switch (i) {
      case 1:
      case 3:
        indices.push_back(startingIndex + 1);
        break;
      case 2:
      case 5:
        indices.push_back(startingIndex + 2);
        break;
      case 4:
        indices.push_back(startingIndex + 3);
        break;
      case 0:
        indices.push_back(startingIndex);
    }
  }
  return indices;
}