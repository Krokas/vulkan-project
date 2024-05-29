#pragma once
#include <glm/glm.hpp>
#include <map>
#include <memory>

#include "window.h"

struct InputData {
  bool isPressed;
};

class Input {
 public:
  Input(GLFWwindow* window);
  void update();
  bool isKeyPressed(int key) { return keyMap[key].isPressed; }
  glm::vec2 getMousePosition() { return mousePosition; }

 private:
  static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
  GLFWwindow* window;
  std::map<int, InputData> keyMap;
  glm::vec2 mousePosition{};
};