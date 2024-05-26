#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <string>

#include "font.h"

class TextComponent {
 public:
  std::shared_ptr<Font> font;
  glm::vec2 position{};
  glm::vec3 color{};
  std::string text{};
  glm::vec3 outlineColor{};
  bool outline = false;
};
