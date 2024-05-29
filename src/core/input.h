#pragma once
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

 private:
  static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
  GLFWwindow* window;
  std::map<int, InputData> keyMap;
};