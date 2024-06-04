#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

class Collision {
 public:
  static bool AABB(glm::vec4 a, glm::vec2 b);
};