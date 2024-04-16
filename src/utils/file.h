#pragma once
#include <string>
#include <vector>
class File {
 public:
  static std::vector<char> readFile(const std::string& filePath);
  static std::vector<char> readShaderFile(const std::string& fileName);
  static std::string getModelFilePath(const std::string& fileName);
  static std::string getFontFilePath(const std::string& fileName);
};
