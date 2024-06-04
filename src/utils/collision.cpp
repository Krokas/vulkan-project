#include "collision.h"

// returns if point is inside axis aligned bounding box
bool Collision::AABB(glm::vec4 a, glm::vec2 b) {
  bool isCollision = false;

  // x       y
  //
  // w       z

  isCollision = a.x <= b.x && a.y >= b.x && a.z <= b.y && a.w >= b.y;

  return isCollision;
}