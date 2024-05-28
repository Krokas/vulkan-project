#include "timer.h"

#include <glm/glm.hpp>

#include "log.h"

Timer::Timer() { currentTime = std::chrono::high_resolution_clock::now(); }

void Timer::update() {
  auto newTime = std::chrono::high_resolution_clock::now();
  frameTime =
      std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();

  frameTime = glm::min(frameTime, MAX_FRAME_TIME);

  currentTime = newTime;
}

unsigned int Timer::getFPS() { return round(1 / frameTime); }