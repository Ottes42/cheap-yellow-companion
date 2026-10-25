// Yellow in the browser: does what yellow-base.yaml does on the device (page, sign, fonts, touch, tick) and includes
// the firmware headers unchanged. JavaScript (yellow-card.js) calls the extern "C" functions below.
// YELLOW_LANG_H is set by build.sh to lang/<language>.h.
#include <emscripten.h>
#include "lvgl.h"
#include "fonts.h"
#include YELLOW_LANG_H
#include "yellow.h"

namespace {
constexpr int W = 320, H = 240;
uint32_t frame_buf[W * H];  // XRGB8888, BGRA in memory
bool redrawn = false;
int pointer_x = 0, pointer_y = 0;
bool pressed = false;
bool press_read = true;  // a quick click between two LVGL reads: keep it pressed until it was read once
bool release_pending = false;
int tap_y = 0;
bool tap_help = false, tap_network = false, tap_games = false;
uint32_t last_tick = 0, greet_at = 0;
std::string info_text;  // shown on a long press, set by the card from Home Assistant

void flush(lv_display_t *d, const lv_area_t *, uint8_t *) {
  redrawn = true;
  lv_display_flush_ready(d);
}

void read_pointer(lv_indev_t *, lv_indev_data_t *data) {
  data->point.x = pointer_x;
  data->point.y = pointer_y;
  data->state = pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
  press_read = true;
  if (release_pending) release_pending = false, pressed = false;
}

// on_press / on_short_click / on_long_press from yellow-base.yaml (without clock view and Wi-Fi dialog)
void surface_cb(lv_event_t *e) {
  auto &c = yellow::companion;
  switch (lv_event_get_code(e)) {
    case LV_EVENT_PRESSED: {
      lv_point_t p;
      lv_indev_get_point(lv_indev_active(), &p);
      tap_help = p.x > 284 && p.y > 200;
      tap_network = p.x > 262 && p.x <= 284 && p.y > 200;
      tap_games = p.x < 30 && p.y > 200;
      tap_y = p.y;
      break;
    }
    case LV_EVENT_SHORT_CLICKED:
      if (c.petting() || !c.is_visible()) break;
      if (c.games_open()) c.games_tap(tap_y);
      else if (tap_network && c.network_level() == 1) break;
      else if (c.help_open() || tap_help) c.help(!c.help_open());
      else if (tap_games) c.games_menu(true);
      else c.poke();
      break;
    case LV_EVENT_LONG_PRESSED:
      if (c.is_visible() && !c.petting() && !c.help_open())
        c.message(info_text.empty() ? std::string(yellow::txt::NOTHING_TODAY) : info_text, false, 3);
      break;
    default:
      break;
  }
}
}  // namespace

extern "C" {

EMSCRIPTEN_KEEPALIVE void start(int kind) {
  lv_init();
  lv_tick_set_cb([] { return esphome::millis(); });
  lv_display_t *d = lv_display_create(W, H);
  lv_display_set_color_format(d, LV_COLOR_FORMAT_XRGB8888);
  lv_display_set_buffers(d, frame_buf, nullptr, sizeof(frame_buf), LV_DISPLAY_RENDER_MODE_DIRECT);
  lv_display_set_flush_cb(d, flush);
  lv_indev_t *touch = lv_indev_create();
  lv_indev_set_type(touch, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(touch, read_pointer);
  lv_indev_set_long_press_time(touch, 1200);
  lv_indev_set_long_press_repeat_time(touch, 100);

  lv_obj_t *page = lv_screen_active();
  lv_obj_set_style_bg_color(page, lv_color_hex(0x000000), 0);
  lv_obj_remove_flag(page, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *surface = lv_obj_create(page);
  lv_obj_set_style_bg_opa(surface, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(surface, 0, 0);
  lv_obj_set_style_pad_all(surface, 0, 0);
  lv_obj_set_size(surface, lv_pct(100), lv_pct(100));
  lv_obj_remove_flag(surface, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_event_cb(surface, surface_cb, LV_EVENT_ALL, nullptr);

  // Yellow's cardboard sign, yellow.h slides it in from the top
  lv_obj_t *sign = lv_obj_create(surface);
  lv_obj_set_style_bg_color(sign, lv_color_hex(0xF4E4C1), 0);
  lv_obj_set_style_border_color(sign, lv_color_hex(0x8B6B3E), 0);
  lv_obj_set_style_border_width(sign, 3, 0);
  lv_obj_set_style_radius(sign, 6, 0);
  lv_obj_set_style_pad_all(sign, 4, 0);
  lv_obj_set_size(sign, 200, 80);
  lv_obj_set_pos(sign, 60, -100);
  lv_obj_add_flag(sign, LV_OBJ_FLAG_HIDDEN);
  lv_obj_remove_flag(sign, (lv_obj_flag_t) (LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE));
  lv_obj_t *sign_text = lv_label_create(sign);
  lv_obj_set_style_align(sign_text, LV_ALIGN_CENTER, 0);
  lv_obj_set_style_text_align(sign_text, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_text_color(sign_text, lv_color_hex(0x3A2A18), 0);
  lv_obj_set_style_text_font(sign_text, &font_sign, 0);
  lv_obj_set_width(sign_text, lv_pct(100));
  lv_label_set_long_mode(sign_text, LV_LABEL_LONG_WRAP);
  lv_label_set_text(sign_text, "");

  auto &c = yellow::companion;
  c.kind((yellow::Kind) kind);
  c.mood(0);  // start values like the sliders on the device
  c.tension(20);
  c.energy(70);
  c.decor_font(&font_decor);
  c.status_font(&font_status);
  c.digit_font(&font_clock);
  c.build(surface, sign, sign_text);
  c.visible(true);
  greet_at = esphome::millis() + 1500;
}

// Called per animation frame: LVGL timers, and the companion's tick every 50 ms. Returns 1 if the frame changed.
EMSCRIPTEN_KEEPALIVE int step() {
  uint32_t now = esphome::millis();
  if (now - last_tick >= 50) {
    last_tick = now;
    yellow::companion.tick();
  }
  if (greet_at && (int32_t) (now - greet_at) >= 0) {
    greet_at = 0;
    yellow::companion.gesture("wave");
    yellow::companion.show_sign(yellow::txt::HELLO, 5);
  }
  lv_timer_handler();
  bool r = redrawn;
  redrawn = false;
  return r;
}

EMSCRIPTEN_KEEPALIVE uint8_t *frame() { return (uint8_t *) frame_buf; }
EMSCRIPTEN_KEEPALIVE void pointer(int x, int y, int down) {
  pointer_x = x, pointer_y = y;
  if (down) pressed = true, press_read = false, release_pending = false;
  else if (press_read) pressed = false;
  else release_pending = true;
}
EMSCRIPTEN_KEEPALIVE void kind(int k) { yellow::companion.kind((yellow::Kind) k); }
EMSCRIPTEN_KEEPALIVE void mood(int v) { yellow::companion.mood(v); }
EMSCRIPTEN_KEEPALIVE void tension(int v) { yellow::companion.tension(v); }
EMSCRIPTEN_KEEPALIVE void energy(int v) { yellow::companion.energy(v); }
EMSCRIPTEN_KEEPALIVE void decor(int m) { yellow::companion.decor(m); }
EMSCRIPTEN_KEEPALIVE void occasion(const char *o) { yellow::companion.occasion(o); }
EMSCRIPTEN_KEEPALIVE void weather(const char *condition) { yellow::companion.weather(condition); }
EMSCRIPTEN_KEEPALIVE void set_time(int month, int day, int hour, int minute) {
  yellow::companion.clock(hour, minute);
  yellow::companion.season(month, day);
}
EMSCRIPTEN_KEEPALIVE int gesture(const char *g) { return yellow::companion.gesture(g); }
EMSCRIPTEN_KEEPALIVE void message(const char *t, int seconds) { yellow::companion.message(t, false, seconds); }
// Long-press sign: outside temperature from Home Assistant, with the language's decimal separator
EMSCRIPTEN_KEEPALIVE void info_temp(double t, const char *unit) {
  info_text = esphome::str_sprintf("%.1f %s", t, unit);
  if (yellow::txt::DECIMAL_COMMA) for (auto &ch : info_text) if (ch == '.') ch = ',';
}
EMSCRIPTEN_KEEPALIVE void visible(int v) { yellow::companion.visible(v); }
}
