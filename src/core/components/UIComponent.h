#pragma once
#include <glm/glm.hpp>

struct UIComponent {
  glm::vec2 position{};
  glm::vec2 size{};
  glm::vec3 color{};
  bool isVisible = true;
};