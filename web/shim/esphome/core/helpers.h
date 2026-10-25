#pragma once
// Stand-ins for the few ESPHome helpers yellow*.h uses
#include <cstdarg>
#include <cstdio>
#include <random>
#include <string>
namespace esphome {
inline std::string str_sprintf(const char *fmt, ...) {
  va_list a, b;
  va_start(a, fmt);
  va_copy(b, a);
  int n = vsnprintf(nullptr, 0, fmt, a);
  va_end(a);
  std::string s(n > 0 ? n : 0, '\0');
  if (n > 0) vsnprintf(&s[0], n + 1, fmt, b);
  va_end(b);
  return s;
}
template<typename T> std::string to_string(T v) { return std::to_string(v); }
inline uint32_t random_uint32() {
  static std::mt19937 rng{std::random_device{}()};
  return rng();
}
}  // namespace esphome
#define ESP_LOGD(tag, fmt, ...) printf("[D][%s] " fmt "\n", tag, ##__VA_ARGS__)
