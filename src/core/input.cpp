#include "input.h"

#include "log.h"

Input::Input(GLFWwindow* window) : window{window} {
  glfwGetWindowUserPointer(window);
  glfwSetScrollCallback(window, scrollCallback);

  InputData defaultData{};
  keyMap.insert({87, defaultData});
}

void Input::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
  Input* input;
  input = (Input*)glfwGetWindowUserPointer(window);
  if (yoffset != 0) {
    // input->setScrolled(yoffset);
  }
}

void Input::update() {
  std::map<int, InputData>::iterator it;
  for (it = keyMap.begin(); it != keyMap.end(); it++) {
    if (glfwGetKey(window, it->first) == GLFW_PRESS) {
      it->second.isPressed = true;
    } else {
      it->second.isPressed = false;
    }
  }

  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);
  mousePosition = {(float)xpos, (float)ypos};
}