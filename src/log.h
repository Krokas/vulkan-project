#pragma once

#include <stdio.h>

#ifdef _WIN32
#define DEBUG_BREAK() __debugbreak()
#define EXPORT_FN __declspec(dllexport)
#elif __linux__
#define DEBUG_BREAK() __builtin_debugtrap()
#elif __APPLE__
#define DEBUG_BREAK() __builtin_trap()
#endif

enum TextColor {
  TEXT_COLOR_BLACK,
  TEXT_COLOR_RED,
  TEXT_COLOR_GREEN,
  TEXT_COLOR_YELLOW,
  TEXT_COLOR_BLUE,
  TEXT_COLOR_MAGENTA,
  TEXT_COLOR_CYAN,
  TEXT_COLOR_WHITE,
  TEXT_COLOR_BRIGHT_BLACK,
  TEXT_COLOR_BRIGHT_RED,
  TEXT_COLOR_BRIGHT_GREEN,
  TEXT_COLOR_BRIGHT_YELLOW,
  TEXT_COLOR_BRIGHT_BLUE,
  TEXT_COLOR_BRIGHT_MAGENTA,
  TEXT_COLOR_BRIGHT_CYAN,
  TEXT_COLOR_BRIGHT_WHITE,
  TEXT_COLOR_COUNT
};

template <typename... Args>
void _log(char *prefix, char *msg, TextColor textColor, Args... args) {
  static char *textColorTable[TEXT_COLOR_COUNT] = {
      "\x1b[30m",
      "\x1b[31m",
      "\x1b[32m",
      "\x1b[33m",
      "\x1b[34m",
      "\x1b[35m",
      "\x1b[36m",
      "\x1b[37m",
      "\x1b[90m",
      "\x1b[91m",
      "\x1b[92m",
      "\x1b[93m",
      "\x1b[94m",
      "\x1b[95m",
      "\x1b[96m",
      "\x1b[97m",
  };

  char formatBuffer[8192] = {};
  sprintf(formatBuffer, "%s %s %s \033[0m", textColorTable[textColor], prefix, msg);

  char textBuffer[8912] = {};
  sprintf(textBuffer, formatBuffer, args...);

  puts(textBuffer);
}

#define R_TRACE(msg, ...) _log("TRACE: ", msg, TEXT_COLOR_GREEN, ##__VA_ARGS__);
#define R_WARN(msg, ...) _log("WARNING: ", msg, TEXT_COLOR_YELLOW, ##__VA_ARGS__);
#define R_ERROR(msg, ...) _log("ERROR: ", msg, TEXT_COLOR_RED, ##__VA_ARGS__);

#define R_ASSERT(x, msg, ...)     \
  {                               \
    if (!(x)) {                   \
      R_ERROR(msg, ##__VA_ARGS__) \
      DEBUG_BREAK();              \
      R_ERROR(msg)                \
    }                             \
  }
