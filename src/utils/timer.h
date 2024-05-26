#pragma once

#include <chrono>

class Timer {
 public:
  static constexpr float MAX_FRAME_TIME = 1.0f;

  Timer();
  void update();
  float getFrameTime() { return frameTime; }
  float getFrameTimeMS() { return frameTime / 1000.0f; }
  int getFPS() { return frameCount; }

 private:
  std::chrono::steady_clock::time_point currentTime;
  float frameTime;
  std::chrono::steady_clock::time_point fpsSecond;
  int frameCount;
};