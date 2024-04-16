#pragma once
#include <string>

#include "device.h"

class Texture {
 public:
  Texture(Device* device);
  ~Texture();
  void addTexturePath(const std::string& fileName);
  void prepare();
  VkImageView getImageView() { return textureImageView; }
  VkSampler getSampler() { return textureSampler; }

  void createFromData(unsigned char* bitmap, int width, int height, int channels);

 private:
  void createTextureImage();
  void createTextureImageView();
  void createTextureSampler();

  // helpers
  void transitionImageLayout(
      VkImage image,
      VkFormat format,
      VkImageLayout oldLayout,
      VkImageLayout newLayout,
      uint32_t mipLevels);
  void generateMipmaps(
      VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

  bool isDataUsed = false;
  int texWidth, texHeight, texChannels;
  unsigned char* pixels;

  std::string texturePath;
  Device* device;

  uint32_t mipLevels;
  VkImage textureImage;
  VkDeviceMemory textureImageMemory;
  VkImageView textureImageView;
  VkSampler textureSampler;
};