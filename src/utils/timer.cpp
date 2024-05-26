#include "timer.h"

#include <glm/glm.hpp>

#include "log.h"

Timer::Timer() {
  currentTime = std::chrono::high_resolution_clock::now();
  fpsSecond = std::chrono::high_resolution_clock::now();
}

void Timer::update() {
  auto newTime = std::chrono::high_resolution_clock::now();
  frameTime =
      std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();

  frameTime = glm::min(frameTime, MAX_FRAME_TIME);

  currentTime = newTime;

  // FPS

  frameCount++;
  if (std::chrono::duration<float, std::chrono::seconds::period>(newTime - fpsSecond).count() >=
      1.0f) {
    frameCount = 0;
    fpsSecond += std::chrono::seconds(1);
  }
}