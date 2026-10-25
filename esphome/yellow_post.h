#pragma once
// Yellow: signs, post and love letters, away, help, network status (methods of Companion, see yellow.h)
#include "yellow.h"

namespace yellow {

inline void Companion::show_sign(const std::string &text, int seconds) {
  if (!visible_ || away_ || sleeping_) return;
  last_sign_ = text;
  last_sign_time_ = esphome::millis();
  if (sign_font_ == nullptr) sign_font_ = lv_obj_get_style_text_font(sign_text_, LV_PART_MAIN);
  lv_obj_set_style_text_font(sign_text_, sign_font_, 0);  // handwriting again after the countdown
  if (sign_y_ > SIGN_SHOWN + 50) {  // was at the bottom after being dropped: slide in from the top again
    sign_y_ = SIGN_HIDDEN;
    sign_layout();
  }
  if (kind_ == ROBOT) led_flash(GLOW, 150);  // the robot beeps (LED) with every sign
  blunder_at_ = 0;
  lv_anim_delete(this, cb_sign_rotate);
  lv_obj_set_style_transform_rotation(sign_, 0, 0);
  lv_label_set_text(sign_text_, text.c_str());
  anim(cb_sign, sign_y_, SIGN_SHOWN, 380, 0, 1, lv_anim_path_overshoot);
  anim(cb_arm, arm_pos_, 100, 400);
  sign_until_ = esphome::millis() + std::max(1, seconds) * 1000;
}

inline void Companion::sign_blunder(const std::string &text) {
  show_sign(text, 5);
  if (!visible_) return;
  std::string f = text;
  std::vector<size_t> place;  // only ASCII letter pairs so no UTF-8 character gets torn apart
  for (size_t i = 0; i + 1 < f.size(); i++)
    if (isalpha((unsigned char) f[i]) && isalpha((unsigned char) f[i + 1]) && f[i] != f[i + 1]) place.push_back(i);
  // a headstand needs an LVGL layer (width × height × 4 bytes in one block); without the room LVGL waits forever →
  // watchdog (2026-10-25: 53 KB needed, 49 KB free). Then rather the swapped letters.
  lv_obj_update_layout(sign_);
  const size_t layer = (size_t) lv_obj_get_width(sign_) * lv_obj_get_height(sign_) * 4 + 16384;
  const bool headstand_fits = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT) > layer;
  if (!place.empty() && (rnd(2) || !headstand_fits)) {
    size_t i = place[rnd(place.size())];
    std::swap(f[i], f[i + 1]);
    lv_label_set_text(sign_text_, f.c_str());
    blunder_kind_ = 2;
  } else if (headstand_fits) {
    lv_obj_set_style_transform_pivot_x(sign_, lv_obj_get_width(sign_) / 2, 0);
    lv_obj_set_style_transform_pivot_y(sign_, lv_obj_get_height(sign_) / 2, 0);
    lv_obj_set_style_transform_rotation(sign_, 1800, 0);
    blunder_kind_ = 1;
  } else {
    return;  // no blunder possible: the sign simply stays correct
  }
  blunder_at_ = esphome::millis() + 1900;
}

inline void Companion::blunder_tick(uint32_t now_ms) {
  if (!passed(now_ms, blunder_at_)) return;
  blunder_at_ = 0;
  for_a_while(ANNOYED, 1500);
  if (blunder_kind_ == 1)
    anim(cb_sign_rotate, 1800, 0, 500);
  else
    lv_label_set_text(sign_text_, last_sign_.c_str());
  anim(cb_hop, 0, 6, 120, 120, 1, lv_anim_path_ease_out);
  sign_until_ = now_ms + 2500;
  if (kind_ == ROBOT) {
    later_text_ = txt::ERROR_FIXED;
    later_at_ = now_ms + 2700;
  }
}

inline void Companion::message(const std::string &text, bool wait, int seconds) {
  if (text.empty()) return;
  if (!wait) {
    if (!visible_) return;
    plan_signs(text, seconds);
    return;
  }
  if (post_.size() >= 5) post_.pop_front();
  post_.push_back(text);
  next_call_ = esphome::millis() + 500;
  letter_layout();
}

inline void Companion::play_post() {
  if (post_.empty()) return;
  std::string t = post_.front();
  post_.pop_front();
  uint32_t now_ms = esphome::millis();
  const std::string original = t;  // with the prefix to HA so the read receipt can say "love letter"
  bool love = is_love(t);
  if (love) t = t.substr(strlen(LOVE_PREFIX));
  letter_open_until_ = now_ms + 1200;
  flap(true);
  letter_layout();
  if (visible_) {
    for_a_while(HAPPY, love ? 6000 : 2000);
    plan_signs(t, love ? 5 : 4);
    if (love) {  // embarrassed: red, hearts rise, fox wags
      blush_until_ = now_ms + 8000;
      wiggle_until_ = now_ms + 2500;
      hearts_start();
      if (led) led(0xFF4A7A, 0.4f);
      led_off_at_ = now_ms + 4000;
    }
  }
  next_call_ = now_ms + 15000;
  if (read) read(original);
}

inline void Companion::clear_post() {
  post_.clear();
  letter_open_until_ = 0;
  letter_layout();
}

inline void Companion::away(bool on, const std::string &card) {
  bool was_on = away_;
  away_ = on;
  card_ = card;
  if (root_ == nullptr) return;
  if (suitcase_ == nullptr) {
    suitcase_ = shape(root_, 0, 0, 320, 240, 0, 0);
    transparent(suitcase_);
    lv_obj_t *grip = shape(suitcase_, 142, 112, 36, 22, 0, 8);
    transparent(grip);
    border(grip, 5, 0x6B3A18);
    lv_obj_t *k = shape(suitcase_, 104, 126, 112, 78, 0x9A5B2E, 12);
    border(k, 3, 0x6B3A18);
    shape(k, 22, 0, 7, 78, 0x6B3A18, 0);  // straps
    shape(k, 80, 0, 7, 78, 0x6B3A18, 0);
    shape(k, 36, 14, 30, 18, YELLOW, 4);  // stickers
    shape(k, 40, 44, 16, 16, 0x4AA8FF);
    shape(k, 92, 46, 12, 18, 0xFF5A7A, 3);
    shape(suitcase_, 118, 204, 14, 8, 0x3A2418, 3);  // wheels
    shape(suitcase_, 188, 204, 14, 8, 0x3A2418, 3);
    lv_obj_t *z = shape(suitcase_, 30, 22, 260, 74, 0xF4E4C1, 4);
    border(z, 2, 0x8B6B3E);
    note_text_ = lv_label_create(z);
    lv_obj_set_width(note_text_, 244);
    lv_obj_set_style_text_align(note_text_, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(note_text_, lv_color_hex(0x3A2A1A), 0);
    lv_label_set_long_mode(note_text_, LV_LABEL_LONG_WRAP);
    lv_obj_align(note_text_, LV_ALIGN_CENTER, 0, 0);
  }
  lv_label_set_text(note_text_, card.empty() ? txt::AWAY : card.c_str());
  show(suitcase_, on);
  show(body_, !on);
  show(shadow_, !on);
  if (on) {
    sign_until_ = sign_until_ ? esphome::millis() + 10 : 0;  // clear a running sign
  } else if (was_on && visible_) {  // back again, with a souvenir
    uint32_t now_ms = esphome::millis();
    static const char *const souvenir[] = {"sunglasses", "scarf", "fan"};
    prop(souvenir[rnd(3)], 20);
    for_a_while(HAPPY, 4000);
    sparkle_until_ = now_ms + 3000;
    anim(cb_hop, 0, 14, 160, 180, 2, lv_anim_path_ease_out);
    show_sign(txt::BACK, 4);
  }
}

inline void Companion::hearts_start() {
  if (root_ == nullptr || decor_font_ == nullptr) return;
  if (heart_g_[0] == nullptr)
    for (int i = 0; i < 6; i++) {
      heart_g_[i] = lv_label_create(root_);
      lv_obj_set_style_text_font(heart_g_[i], decor_font_, 0);
      lv_label_set_text(heart_g_[i], "\U000F02D1");
      lv_obj_set_style_text_color(heart_g_[i], lv_color_hex(i % 2 ? 0xFF4A6A : 0xFF8FA3), 0);
      show(heart_g_[i], false);
    }
  for (auto *h : heart_g_) {
    lv_obj_move_foreground(h);
    show(h, true);
  }
  lv_obj_move_foreground(sign_);
  hearts_start_ = esphome::millis();
}

inline void Companion::hearts_tick(uint32_t now_ms) {
  if (!hearts_start_) return;
  uint32_t t = now_ms - hearts_start_;
  if (t > 4000) {
    hearts_start_ = 0;
    for (auto *h : heart_g_) show(h, false);
    return;
  }
  for (int i = 0; i < 6; i++) {
    int ti = (int) t - i * 350;
    if (ti < 0) {
      lv_obj_set_y(heart_g_[i], 260);
      continue;
    }
    lv_obj_set_pos(heart_g_[i], 40 + i * 44 + (int) (10 * sinf(ti / 250.0f + i)), 210 - ti * 220 / 3000);
    lv_obj_set_style_text_opa(heart_g_[i], (lv_opa_t) std::max(0, 255 - ti * 255 / 3200), 0);
  }
}

inline void Companion::build_help() {
  help_button_ = shape(root_, 298, 218, 19, 19, 0, 10);  // small circle with "?"
  transparent(help_button_);
  border(help_button_, 2, 0x7A88A8);
  lv_obj_t *fz = lv_label_create(help_button_);
  lv_label_set_text(fz, "?");
  lv_obj_set_style_text_color(fz, lv_color_hex(0x9AA6C4), 0);
  lv_obj_align(fz, LV_ALIGN_CENTER, 0, 0);
  help_panel_ = shape(root_, 0, 0, 320, 240, 0x0E1220, 0);
  lv_obj_set_style_bg_opa(help_panel_, 240, 0);
  lv_obj_t *title = lv_label_create(help_panel_);
  lv_label_set_text(title, txt::HELP_TITLE);
  lv_obj_set_style_text_color(title, lv_color_hex(YELLOW), 0);
  lv_obj_set_pos(title, 14, 8);
  lv_obj_t *text = lv_label_create(help_panel_);
  lv_obj_set_style_text_color(text, lv_color_hex(0xD8DEE9), 0);
  lv_obj_set_style_text_line_space(text, 1, 0);
  lv_obj_set_pos(text, 14, 34);
  lv_label_set_text(text, txt::HELP_TEXT);
  lv_obj_t *close_lbl = lv_label_create(help_panel_);
  lv_label_set_text(close_lbl, txt::TAP_CLOSES);
  lv_obj_set_style_text_color(close_lbl, lv_color_hex(0x5A6680), 0);
  lv_obj_set_pos(close_lbl, 14, 214);
  show(help_panel_, false);

  games_button_ = shape(root_, 3, 218, 19, 19, 0, 10);  // circle with a gamepad, made like the "?"
  transparent(games_button_);
  border(games_button_, 2, 0x7A88A8);
  lv_obj_t *sk = lv_label_create(games_button_);
  if (status_font_) lv_obj_set_style_text_font(sk, status_font_, 0);
  lv_label_set_text(sk, status_font_ ? "\U000F0297" : "S");
  lv_obj_set_style_text_color(sk, lv_color_hex(0x9AA6C4), 0);
  lv_obj_align(sk, LV_ALIGN_CENTER, 0, 0);
  games_panel_ = shape(root_, 0, 0, 320, 240, 0x0E1220, 0);
  lv_obj_set_style_bg_opa(games_panel_, 240, 0);
  lv_obj_t *st = lv_label_create(games_panel_);
  lv_label_set_text(st, txt::WHAT_GAME);
  lv_obj_set_style_text_color(st, lv_color_hex(YELLOW), 0);
  lv_obj_set_pos(st, 14, 8);
  static const char *const NAMES[3] = {txt::GAME_REACTION, txt::GAME_TTT, txt::GAME_SIMON};
  for (int i = 0; i < 3; i++) {  // three big fields, 50 px high each from y = 44
    lv_obj_t *f = shape(games_panel_, 14, 44 + i * 56, 292, 48, 0x1E2740, 10);
    border(f, 2, 0x3A4868);
    lv_obj_t *l = lv_label_create(f);
    lv_label_set_text(l, NAMES[i]);
    lv_obj_set_style_text_color(l, lv_color_hex(0xD8DEE9), 0);
    lv_obj_align(l, LV_ALIGN_CENTER, 0, 0);
  }
  lv_obj_t *sz = lv_label_create(games_panel_);
  lv_label_set_text(sz, txt::TAP_OUTSIDE_CLOSES);
  lv_obj_set_style_text_color(sz, lv_color_hex(0x5A6680), 0);
  lv_obj_set_pos(sz, 14, 214);
  show(games_panel_, false);
}
inline void Companion::games_menu(bool on) {
  if (games_panel_ == nullptr) return;
  show(games_panel_, on);
  if (on) lv_obj_move_foreground(games_panel_);
  games_until_ = on ? esphome::millis() + 20000 : 0;
}
inline void Companion::games_tap(int y) {
  games_menu(false);
  int i = (y - 44) / 56;
  static const char *const GAMES[3] = {"reaction", "ttt", "simon"};
  if (y >= 44 && i < 3 && (y - 44) % 56 < 48) game(GAMES[i]);
}

inline void Companion::help(bool on) {
  if (help_panel_ == nullptr) return;
  show(help_panel_, on);
  if (on) {
    lv_obj_move_foreground(help_panel_);
    help_until_ = esphome::millis() + 20000;
  } else {
    help_until_ = 0;
  }
}

inline void Companion::network(int level, const std::string &ssid) {
  if (root_ == nullptr) return;
  if (network_symbol_ == nullptr) {
    network_symbol_ = lv_label_create(root_);
    if (status_font_) lv_obj_set_style_text_font(network_symbol_, status_font_, 0);
    lv_obj_set_pos(network_symbol_, 276, 220);
    network_text_ = lv_label_create(root_);
    lv_obj_set_width(network_text_, 300);
    lv_obj_set_style_text_align(network_text_, LV_TEXT_ALIGN_RIGHT, 0);
    lv_obj_set_pos(network_text_, 12, 196);
  }
  if (level > 0 && level != network_level_) {
    static const char *const SYMBOL[] = {"", "\U000F05AA", "\U000F0164", "\U000F0A8E"};
    const bool red = level == 1;
    lv_label_set_text(network_symbol_, status_font_ ? SYMBOL[level] : "!");
    lv_obj_set_style_text_color(network_symbol_, lv_color_hex(red ? 0xFF4A4A : 0xFF9A2A), 0);
    lv_obj_set_style_text_color(network_text_, lv_color_hex(red ? 0xFF7A6A : 0xFFB060), 0);
  }
  if (level == 1) lv_label_set_text(network_text_, (txt::WAITING_FOR_NETWORK + (ssid.empty() ? std::string() : ": " + ssid)).c_str());
  else if (level == 2) lv_label_set_text(network_text_, txt::SERVER_UNREACHABLE);
  else if (level == 3) lv_label_set_text(network_text_, (txt::NO_INTERNET + (ssid.empty() ? std::string() : txt::IN_NETWORK + ssid)).c_str());
  network_level_ = level;
  show(network_symbol_, level > 0);
  show(network_text_, level > 0);
}

inline void Companion::plan_signs(const std::string &text, int seconds) {
  signs_.clear();
  std::string row, word;
  auto add_word = [&]() {
    if (word.empty()) return;
    if (!row.empty() && row.size() + 1 + word.size() > 24) {
      signs_.push_back(row);
      row.clear();
    }
    row += (row.empty() ? "" : " ") + word;
    word.clear();
  };
  for (char c : text) {
    if (c == '\n') {  // line break = new sign
      add_word();
      if (!row.empty()) signs_.push_back(row);
      row.clear();
    } else if (c == ' ') {
      add_word();
    } else {
      word += c;
    }
  }
  add_word();
  if (!row.empty()) signs_.push_back(row);
  if (signs_.empty()) return;
  if (signs_.size() > 8) signs_.resize(8);
  sign_duration_ = std::max(2, seconds);
  show_sign(signs_.front(), sign_duration_);
  signs_.pop_front();
}

}  // namespace yellow
