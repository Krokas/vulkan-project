#include "window.h"

#include "log.h"

Window::Window(int width, int height, std::string name)
    : width{width}, height{height}, windowName{name} {
  initWindow();
}

Window::~Window() {
  glfwDestroyWindow(window);
  glfwTerminate();
}

void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
  if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
    R_ASSERT(false, "Failed tor create window surface");
    return;
  }
}

void Window::frameBufferResizeCallback(GLFWwindow* window_, int width, int height) {
  auto window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window_));
  window->frameBufferResized = true;
  window->width = width;
  window->height = height;
}

void Window::initWindow() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

  window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
  glfwSetWindowUserPointer(window, this);
  glfwSetFramebufferSizeCallback(window, frameBufferResizeCallback);
}
