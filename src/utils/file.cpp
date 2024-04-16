#include "file.h"

#include <filesystem>
#include <fstream>

std::vector<char> File::readFile(const std::string& filePath) {
  std::ifstream file{filePath, std::ios::ate | std::ios::binary};
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file: " + filePath);
  }

  size_t fileSize = static_cast<size_t>(file.tellg());
  std::vector<char> buffer(fileSize);

  file.seekg(0);

  file.read(buffer.data(), fileSize);

  file.close();

  return buffer;
}

std::vector<char> File::readShaderFile(const std::string& fileName) {
  std::string filePath = "";
  filePath.append("resources\\shaders\\");
  filePath.append(fileName);

  return readFile(std::filesystem::absolute(filePath).string());
}

std::string File::getModelFilePath(const std::string& fileName) {
  std::string filePath = "";
  filePath.append("resources\\models\\");
  filePath.append(fileName);

  return std::filesystem::absolute(filePath).string();
}

std::string File::getFontFilePath(const std::string& fileName) {
  std::string filePath = "";
  filePath.append("resources\\fonts\\");
  filePath.append(fileName);

  return std::filesystem::absolute(filePath).string();
}
