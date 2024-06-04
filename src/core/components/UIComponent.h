#pragma once
#include <glm/glm.hpp>

struct UIComponent {
  glm::vec3 color{};
  bool isVisible = true;
};