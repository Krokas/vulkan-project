#include "input.h"

#include "log.h"

Input::Input(GLFWwindow* window) : window{window} {
  glfwGetWindowUserPointer(window);
  glfwSetScrollCallback(window, scrollCallback);

  addKeyBinding(FORWARD, GLFW_KEY_W);
  addKeyBinding(BACKWARD, GLFW_KEY_S);
  addKeyBinding(LEFT, GLFW_KEY_A);
  addKeyBinding(RIGHT, GLFW_KEY_D);

  addKeyBinding(FORWARD, GLFW_KEY_UP);
  addKeyBinding(BACKWARD, GLFW_KEY_DOWN);
  addKeyBinding(LEFT, GLFW_KEY_LEFT);
  addKeyBinding(RIGHT, GLFW_KEY_RIGHT);

  addKeyBinding(SELECT, GLFW_MOUSE_BUTTON_LEFT);
}

void Input::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
  Input* input;
  input = (Input*)glfwGetWindowUserPointer(window);
  if (yoffset != 0) {
    // input->setScrolled(yoffset);
  }
}

void Input::update() {
  std::map<Actions, std::map<int, InputData>>::iterator it;
  for (it = keyMap.begin(); it != keyMap.end(); it++) {
    std::map<int, InputData>::iterator keyIterator;
    for (keyIterator = it->second.begin(); keyIterator != it->second.end(); keyIterator++) {
      if (glfwGetKey(window, keyIterator->first) == GLFW_PRESS || glfwGetMouseButton(window, keyIterator->first) == GLFW_PRESS) {
        keyIterator->second.isPressed = true;
      } else {
        keyIterator->second.isPressed = false;
      }
    }
  }

  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);
  mousePosition = {(float)xpos, (float)ypos};
}

bool Input::isKeyPressed(Actions key) {
  std::map<int, InputData>::iterator it;
  for (it = keyMap[key].begin(); it != keyMap[key].end(); it++) {
    if (it->second.isPressed) {
      return true;
    }
  }
  return false;
}

void Input::addKeyBinding(Actions action, int key) {
  InputData defaultData{};
  std::map<int, InputData> keyBinding;
  keyBinding.insert({key, defaultData});
  if (keyMap.find(action) == keyMap.end()) {
    keyMap.insert({action, keyBinding});
  } else {
    keyMap[action].insert({key, defaultData});
  }
}