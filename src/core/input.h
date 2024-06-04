#pragma once
#include <glm/glm.hpp>
#include <map>
#include <memory>

#include "window.h"

struct InputData {
  bool isPressed;
};

enum Actions { FORWARD, BACKWARD, LEFT, RIGHT, SELECT };

class Input {
 public:
  Input(GLFWwindow* window);
  void update();
  bool isKeyPressed(Actions key);
  glm::vec2 getMousePosition() { return mousePosition; }
  void addKeyBinding(Actions action, int key);

 private:
  static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
  GLFWwindow* window;
  std::map<Actions, std::map<int, InputData>> keyMap;
  glm::vec2 mousePosition{};
};