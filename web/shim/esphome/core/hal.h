#pragma once
// Stand-in for ESPHome in the browser: time from Emscripten instead of the ESP32
#include <cstdint>
#include <emscripten.h>
namespace esphome {
inline uint32_t millis() { return (uint32_t) emscripten_get_now(); }
}  // namespace esphome
