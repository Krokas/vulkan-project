#pragma once
#include <glm/vec3.hpp>

glm::vec3 hexColorToUnitary(glm::vec3 color) { return {color.r / 255.0f, color.g / 255.0f, color.b / 255.0f}; }