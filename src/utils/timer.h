#pragma once

#include <chrono>

class Timer {
 public:
  static constexpr float MAX_FRAME_TIME = 1.0f;

  Timer();
  void update();
  float getFrameTime() { return frameTime; }
  unsigned int getFPS();

 private:
  std::chrono::steady_clock::time_point currentTime;
  float frameTime;
};