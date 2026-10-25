#pragma once
// Yellow: special moves and big effects (ink, mouse dive, glitch, matrix, fist bump …) (methods of Companion, see yellow.h)
#include "yellow.h"

namespace yellow {

inline void Companion::ink() {
  if (!visible_) return;
  uint32_t now_ms = esphome::millis();
  if (ink_[0] == nullptr) {
    for (int i = 0; i < 6; i++) {
      ink_[i] = shape(root_, 0, 0, 10, 10, 0x22182E);
      border(ink_[i], 3, 0x3E2E55);
    }
    for (auto *&s : splash_) s = shape(root_, 0, 0, 9, 9, 0x2E2240);
  }
  for_a_while(SURPRISED, 600);
  anim(cb_hop, 0, 20, 150, 200, 1, lv_anim_path_ease_out);
  wiggle_until_ = now_ms + 600;
  for (auto *k : ink_) {
    lv_obj_move_foreground(k);
    lv_obj_set_style_opa(k, LV_OPA_COVER, 0);
    show(k, true);
  }
  for (auto *s : splash_) {
    lv_obj_move_foreground(s);
    lv_obj_set_style_opa(s, LV_OPA_COVER, 0);
    show(s, true);
  }
  lv_obj_move_foreground(sign_);  // the sign stays in front of the cloud
  ink_start_ = now_ms;
  ink_tick(now_ms);
}

inline void Companion::ink_tick(uint32_t now_ms) {
  if (!ink_start_) return;
  static const int CIRCLE[6][3] = {{160, 110, 120}, {120, 90, 90}, {200, 95, 95}, {140, 150, 100}, {185, 150, 95}, {160, 60, 80}};
  uint32_t t = now_ms - ink_start_;
  for (int i = 0; i < 8; i++) {  // splashes fly in all directions and fall
    float w = i * 0.785f + 0.3f, r = std::min(1.0f, t / 700.0f) * (70 + (i % 3) * 18);
    lv_obj_set_pos(splash_[i], 156 + (int) (r * cosf(w)), 108 + (int) (r * sinf(w)) + (int) (t * t / 30000));
    lv_obj_set_style_opa(splash_[i], (lv_opa_t) std::max(0, 255 - (int) t * 255 / 1200), 0);
  }
  if (t >= 2600) {
    ink_start_ = 0;
    for (auto *k : ink_) show(k, false);
    for (auto *s : splash_) show(s, false);
    if (ink_gone_) {  // e.g. hidden in between: bring Okti back in any case
      ink_gone_ = false;
      show(body_, true);
      show(shadow_, true);
    }
    return;
  }
  float wax = std::min(1.0f, t / 450.0f);
  int opened = t > 700 ? (int) (t - 700) / 60 : 0;  // the cloud rises a bit while fading
  lv_opa_t opa = t < 900 ? LV_OPA_COVER : (lv_opa_t) std::max(0, 255 - (int) (t - 900) * 255 / 1700);
  for (int i = 0; i < 6; i++) {
    int d = std::max(10, (int) (CIRCLE[i][2] * wax));
    lv_obj_set_size(ink_[i], d, d);
    lv_obj_set_pos(ink_[i], CIRCLE[i][0] - d / 2, CIRCLE[i][1] - d / 2 - opened);
    lv_obj_set_style_opa(ink_[i], opa, 0);
  }
  bool gone = t >= 450 && t < 1900;  // Okti has disappeared in the cloud
  if (gone != ink_gone_) {
    ink_gone_ = gone;
    show(body_, !gone);
    show(shadow_, !gone);
    if (!gone) {  // back again: embarrassed
      for_a_while(HAPPY, 2500);
      blush_until_ = now_ms + 3500;
      anim(cb_hop, 0, 8, 140, 140, 1, lv_anim_path_ease_out);
    }
  }
}

inline void Companion::fist_bump() {
  if (!visible_ || sign_until_ || fist_until_) return;
  uint32_t now_ms = esphome::millis();
  if (fist_ == nullptr) {
    fist_arm_ = shape(root_, 0, 0, 10, 10, 0, 8);  // forearm from below to the fist
    fist_ = shape(root_, 0, 0, 10, 10, 0);
    for (int i = 0; i < 3; i++) knuckle_[i] = shape(fist_, 0, 0, 2, 2, 0);  // finger gaps
    for (auto *&f : sparks_) f = shape(root_, 0, 0, 7, 7, 0xFFF3A0);
    for (auto *f : sparks_) show(f, false);
  }
  uint32_t color = kind_ == OKTI ? YELLOW : kind_ == FOX ? FOX_ORANGE : METAL;
  uint32_t border_f = kind_ == OKTI ? YELLOW_EDGE : kind_ == FOX ? FOX_BROWN : 0x5A6274;
  lv_obj_set_style_bg_color(fist_, lv_color_hex(color), 0);
  border(fist_, 3, border_f);
  lv_obj_set_style_bg_color(fist_arm_, lv_color_hex(color), 0);
  border(fist_arm_, 3, border_f);
  lv_obj_move_foreground(fist_arm_);
  show(fist_arm_, true);
  for (auto *k : knuckle_) lv_obj_set_style_bg_color(k, lv_color_hex(border_f), 0);
  lv_obj_move_foreground(fist_);
  show(fist_, true);
  fist_start_ = now_ms;
  fist_until_ = now_ms + 8000;
  for_a_while(HAPPY, 2000);
  anim(cb_look_x, look_x_, 0, 200);
  anim(cb_look_y, look_y_, 4, 200);
  next_look_ = now_ms + 8000;
  show_sign(txt::FIST_Q, 8);
}

inline void Companion::fist_hit(uint32_t now_ms) {
  fist_until_ = 0;
  counts_[FIST]++;
  show(fist_, false);
  show(fist_arm_, false);
  sparks_start_ = now_ms;
  for (auto *f : sparks_) {
    lv_obj_move_foreground(f);
    show(f, true);
  }
  for_a_while(HAPPY, 2500);
  anim(cb_hop, 0, 14, 140, 180, 1, lv_anim_path_ease_out);
  wiggle_until_ = now_ms + 1200;
  sparkle_until_ = now_ms + 2000;
  show_sign(txt::BOOM, 2);
  if (led) led(0xFFD23F, 0.6f);
  led_off_at_ = now_ms + 600;
}

inline void Companion::fist_tick(uint32_t now_ms) {
  if (fist_until_) {
    if (passed(now_ms, fist_until_)) {  // nobody bumped
      fist_until_ = 0;
      show(fist_, false);
      show(fist_arm_, false);
      for_a_while(SAD, 2000);
      later_text_ = txt::FINE;
      later_at_ = now_ms + 300;
    } else {  // the fist comes forward: grows with an overshoot to 52 px, then bobs slightly
      uint32_t t = now_ms - fist_start_;
      float k = t < 350 ? t / 350.0f * 1.15f : t < 500 ? 1.15f - (t - 350) / 150.0f * 0.15f : 1.0f;
      int d = std::max(10, (int) (66 * k)) + (t > 500 ? (int) (2 * sinf(t / 180.0f)) : 0);
      lv_obj_set_size(fist_, d, d * 4 / 5);
      lv_obj_set_style_radius(fist_, d / 3, 0);
      lv_obj_set_pos(fist_, 160 - d / 2, FIST_Y - d * 2 / 5);
      int aw = d * 11 / 20;  // forearm: narrower, reaches below the edge
      lv_obj_set_size(fist_arm_, aw, 240 - FIST_Y + 10);
      lv_obj_set_pos(fist_arm_, 160 - aw / 2, FIST_Y + d / 5);
      for (int i = 0; i < 3; i++) {
        lv_obj_set_size(knuckle_[i], 2, d / 4);
        lv_obj_set_pos(knuckle_[i], d * (i + 1) / 4 - 2, 2);
      }
    }
  }
  if (sparks_start_) {  // eight sparks fly apart
    uint32_t t = now_ms - sparks_start_;
    if (t > 600) {
      sparks_start_ = 0;
      for (auto *f : sparks_) show(f, false);
      return;
    }
    for (int i = 0; i < 8; i++) {
      float w = i * 0.785f, r = 10 + t * 0.11f;
      lv_obj_set_pos(sparks_[i], 157 + (int) (r * cosf(w)), FIST_Y - 3 + (int) (r * sinf(w)));
      lv_obj_set_style_opa(sparks_[i], (lv_opa_t) (255 - t * 255 / 600), 0);
    }
  }
}

inline void Companion::special() {
  int w = rnd(3);
  if (kind_ == OKTI) w == 0 ? ink() : w == 1 ? knot() : juggle();
  else if (kind_ == FOX) w == 0 ? mouse_dive() : w == 1 ? tail_fan() : chase_tail();
  else w == 0 ? glitch() : w == 1 ? open_visor() : propeller();
}

inline void Companion::juggle() {
  if (!visible_ || kind_ != OKTI || juggle_start_) return;
  if (ball_[0] == nullptr) {
    static const uint32_t COLOR[3] = {0xFF4A4A, 0x4AA8FF, 0x5AE07A};
    for (int i = 0; i < 3; i++) ball_[i] = shape(root_, 0, 0, 16, 16, COLOR[i]);
  }
  for (auto *b : ball_) {
    lv_obj_move_foreground(b);
    show(b, true);
  }
  juggle_start_ = esphome::millis();
  juggle_drop_ = rnd(3) == 0;
  for_a_while(HAPPY, 3500);
  wiggle_until_ = juggle_start_ + 3500;
}

inline void Companion::chase_tail() {
  if (!visible_ || kind_ != FOX || chase_start_) return;
  chase_start_ = esphome::millis();
  for_a_while(SURPRISED, 2800);
  wiggle_until_ = chase_start_ + 2800;
}

inline void Companion::propeller() {
  if (!visible_ || kind_ != ROBOT || prop_start_) return;
  if (prop_[0] == nullptr)
    for (auto *&p : prop_) p = segment_line(body_, 5, 0xC8D0DC);
  for (auto *p : prop_) show(p, true);
  prop_start_ = esphome::millis();
  for_a_while(HAPPY, 3800);
  show_sign(txt::LIFT_OFF, 2);
}

inline void Companion::special3_tick(uint32_t now_ms) {
  if (juggle_start_) {
    uint32_t t = now_ms - juggle_start_;
    if (t > 3600) {
      juggle_start_ = 0;
      for (auto *b : ball_) show(b, false);
      if (juggle_drop_) {
        for_a_while(ANNOYED, 1500);
        show_sign(txt::OOPS, 2);
      } else {
        show_sign(txt::TA_DA, 2);
      }
    } else {
      for (int i = 0; i < 3; i++) {  // balls on offset arcs between the outer arms
        float p = fmodf(t / 900.0f + i / 3.0f, 1.0f);
        int x = 90 + (int) (140 * p), y = 70 - (int) (90 * 4 * p * (1 - p));
        if (juggle_drop_ && i == 2 && t > 2200) {  // this one drops
          x = 200;
          y = 60 + (int) ((t - 2200) * (t - 2200) / 6000);
        }
        lv_obj_set_pos(ball_[i], x, y);
      }
    }
  }
  if (chase_start_) {
    uint32_t t = now_ms - chase_start_;
    if (t > 2800) {
      chase_start_ = 0;
      visit_x_ = 0;
      body_layout();
      for_a_while(DIZZY, 2000);
      anim(cb_wobble, 0, 1000, 1500);
      show_sign(rnd(2) ? txt::ALMOST : txt::FAST_ONE, 2);
    } else {
      visit_x_ = (int) (40 * sinf(t / 180.0f));
      body_layout();
    }
  }
  if (prop_start_ && prop_[0]) {
    uint32_t t = now_ms - prop_start_;
    if (t > 3800) {
      prop_start_ = 0;
      mouse_y_ = 0;
      body_layout();
      for (auto *p : prop_) show(p, false);
      anim(cb_hop, 0, 6, 100, 120, 1, lv_anim_path_ease_out);
    } else {
      float a = t / 40.0f;  // rotor blades turn (two lines above the antenna)
      for (int k = 0; k < 2; k++) {
        float w = a + k * 1.5708f, r = 26 * fabsf(cosf(w));
        prop_pts_[k][0].x = lroundf(90 - r) + ARM_DX;
        prop_pts_[k][0].y = -26 + ARM_DY;
        prop_pts_[k][1].x = lroundf(90 + r) + ARM_DX;
        prop_pts_[k][1].y = -26 + ARM_DY;
        lv_line_set_points(prop_[k], prop_pts_[k], 2);
      }
      int high = t < 800 ? 0 : t < 1600 ? (int) ((t - 800) * 40 / 800) : t < 2800 ? 40 : (int) (40 - (t - 2800) * 40 / 1000);
      mouse_y_ = -high;
      body_layout();
    }
  }
}

inline void Companion::knot() {
  if (!visible_ || kind_ != OKTI) return;
  uint32_t now_ms = esphome::millis();
  knot_until_ = now_ms + 3200;
  for_a_while(DIZZY, 3200);
  later_text_ = rnd(2) ? txt::ONE_MOMENT : txt::WHICH_ARM;
  later_at_ = now_ms + 600;
  knot_loose_ = now_ms + 3200;
}

inline void Companion::tail_fan() {
  if (!visible_ || kind_ != FOX) return;
  uint32_t now_ms = esphome::millis();
  tail_fan_until_ = now_ms + 3500;
  for_a_while(HAPPY, 3500);
  if (tail_) for (auto *l : {tail_edge_, tail_belly_, tail_, tail_tip_}) if (l) lv_obj_move_foreground(l);
  for (auto *l : tip_) if (l) lv_obj_move_foreground(l);
  later_text_ = txt::PHEW;
  later_at_ = now_ms + 3600;
}

inline void Companion::open_visor() {
  if (!visible_ || kind_ != ROBOT || visor_ == nullptr || flap_start_) return;
  flap_start_ = esphome::millis();
  for_a_while(SURPRISED, 600);
}

inline void Companion::special2_tick(uint32_t now_ms) {
  if (knot_loose_ && passed(now_ms, knot_loose_)) {  // shaken loose
    knot_loose_ = 0;
    anim(cb_wobble, 0, 1000, 900);
    wiggle_until_ = now_ms + 900;
    for_a_while(ANNOYED, 1500);
  }
  if (tail_fan_until_ && passed(now_ms, tail_fan_until_)) {
    tail_fan_until_ = 0;
    build_figure();  // tail back behind the body
  }
  if (flap_start_ && visor_) {
    uint32_t t = now_ms - flap_start_;
    int y = t < 500 ? 13 - (int) (t * 90 / 500) : t < 3500 ? -77 : t < 4000 ? -77 + (int) ((t - 3500) * 90 / 500) : 13;
    lv_obj_set_y(visor_, y);
    for (int k = 0; k < 2; k++)  // gears "turn": the hub moves in a circle
      if (gear_[k]) {
        lv_obj_t *hub = lv_obj_get_child(gear_[k], 0);
        float a = (k ? -1 : 1) * t / 200.0f;
        int r = k ? 6 : 9, m = k ? 13 : 18;
        if (hub) lv_obj_set_pos(hub, m - 4 + (int) (r * cosf(a)), m - 4 + (int) (r * sinf(a)));
      }
    if (t >= 600 && t < 650) show_sign(rnd(2) ? txt::MAINTENANCE : txt::QUICK_LOOK, 3);
    if (t >= 4000) {
      flap_start_ = 0;
      lv_obj_set_y(visor_, 13);
      for_a_while(HAPPY, 1500);
    }
  }
}

inline void Companion::mouse_dive() {
  if (!visible_ || kind_ != FOX || mouse_start_) return;
  if (mound_ == nullptr) {
    mound_ = shape(root_, 70, 196, 180, 100, 0x6B4A2E, 50);
    shape(mound_, 40, 6, 100, 10, 0x8A6440, 5);  // lighter top
    show(mound_, false);
  }
  lv_obj_move_foreground(mound_);
  lv_obj_move_foreground(sign_);
  show(mound_, true);
  for_a_while(SURPRISED, 700);
  next_look_ = esphome::millis() + 3000;
  anim(cb_look_y, look_y_, 8, 200);
  mouse_start_ = esphome::millis();
}

inline void Companion::mouse_tick(uint32_t now_ms) {
  if (!mouse_start_) return;
  uint32_t t = now_ms - mouse_start_;
  int y;
  if (t < 350) y = -(int) (60 * sinf(t / 350.0f * 1.5708f));                          // take-off
  else if (t < 700) y = -60 + (int) (190 * ((t - 350) / 350.0f) * ((t - 350) / 350.0f));  // head first
  else if (t < 2300) y = 130;                                                          // stuck inside
  else if (t < 2700) y = 130 - (int) (140 * (t - 2300) / 400.0f);                      // comes out
  else if (t < 2900) y = -10 + (int) (10 * (t - 2700) / 200.0f);
  else y = 0;
  mouse_y_ = y;
  if (t >= 700 && t < 2300) lv_obj_set_x(mound_, 70 + (int) (3 * sinf(t / 60.0f)));  // it digs
  body_layout();
  show(shadow_, t < 350 || t >= 2700);
  if (t >= 2300 && !mouse_out_) {
    mouse_out_ = true;
    for_a_while(rnd(3) ? ANNOYED : HAPPY, 2500);
    static const char *const catch_[] = {txt::MOUSE_NO, txt::NOTHING, txt::DIRT_YUM, txt::ALMOST};
    later_text_ = catch_[rnd(4)];
    later_at_ = now_ms + 600;
  }
  if (t >= 3600) {
    mouse_start_ = 0;
    mouse_out_ = false;
    mouse_y_ = 0;
    show(mound_, false);
    show(shadow_, true);
    body_layout();
  }
}

inline void Companion::glitch() {
  if (!visible_ || kind_ != ROBOT || visor_ == nullptr || glitch_start_) return;
  glitch_start_ = esphome::millis();
  led_flash(0x7FE8FF, 1600);
}

inline void Companion::glitch_tick(uint32_t now_ms) {
  if (!glitch_start_ || visor_ == nullptr) return;
  uint32_t t = now_ms - glitch_start_;
  if (t >= 1800) {
    glitch_start_ = 0;
    lv_obj_set_style_bg_color(visor_, lv_color_hex(VISOR), 0);
    color_st_ = -1;  // set the mood color again right away
    for (auto *s : scan_) show(s, false);
    show(face_, true);
    face_layout();
    for_a_while(SURPRISED, 1200);
    show_sign(rnd(2) ? txt::REBOOT_DONE : txt::THAT_WAS_NOTHING, 3);
    return;
  }
  static const uint32_t COLORS[4] = {VISOR, 0x3A1E40, 0x1E4038, 0x2A3550};
  lv_obj_set_style_bg_color(visor_, lv_color_hex(COLORS[rnd(4)]), 0);
  for (auto *s : scan_) {
    lv_obj_set_y(s, (int) rnd(80));
    show(s, rnd(3) != 0);
  }
  bool black = t > 1100 && t < 1450;  // picture gone briefly
  show(face_, !black);
  if (!black) lv_obj_set_pos(face_, look_x_ + (int) rnd(17) - 8, look_y_ + (int) rnd(5) - 2);
}

inline void Companion::matrix(int seconds) {
  if (root_ == nullptr) return;
  if (matrix_[0] == nullptr) {
    for (int i = 0; i < 14; i++) {
      matrix_[i] = lv_label_create(root_);
      lv_obj_set_style_text_color(matrix_[i], lv_color_hex(i % 3 ? 0x2E9E5A : 0x7AF09A), 0);
      lv_label_set_text(matrix_[i], "1\n0\n1");
      matrix_y_[i] = -(float) rnd(240);
      lv_obj_move_to_index(matrix_[i], 1);  // behind decoration and character
      show(matrix_[i], false);
    }
  }
  matrix_until_ = seconds > 0 ? esphome::millis() + seconds * 1000 : 0;
  for (auto *m : matrix_) show(m, matrix_until_ != 0);
}

inline void Companion::matrix_tick(uint32_t now_ms) {
  if (!matrix_until_) return;
  if (passed(now_ms, matrix_until_)) {
    matrix(0);
    return;
  }
  if (now_ms - last_matrix_ < 80) return;
  last_matrix_ = now_ms;
  for (int i = 0; i < 14; i++) {
    matrix_y_[i] += 4 + (i % 4);
    if (matrix_y_[i] > 240) {
      matrix_y_[i] = -60;
      char s[8] = {(char) ('0' + rnd(2)), '\n', (char) ('0' + rnd(2)), '\n', (char) ('0' + rnd(2)), 0};
      lv_label_set_text(matrix_[i], s);
    }
    lv_obj_set_pos(matrix_[i], 6 + i * 22, (int) matrix_y_[i]);
  }
}

inline void Companion::sleep(bool on) {
  if (on == sleeping_) return;
  sleeping_ = on;
  if (root_ == nullptr) return;
  if (on) {
    sleep_scene();
    return;
  }
  show(bed_, false);
  wilt_until_ = 0;
  if (!mouse_start_) mouse_y_ = 0;
  base_ = NORMAL;
  bored_level_ = 0;
  last_touch_ = esphome::millis();
  build_figure();  // restores order, face and visor
  if (visible_) {
    for_a_while(YAWN, 2200);
    later_text_ = kind_ == ROBOT ? txt::SYSTEM_UP : kind_ == FOX ? txt::AWAKE : txt::MORNING_ALREADY;
    later_at_ = esphome::millis() + 2400;
  }
}

inline void Companion::sleep_scene() {
  if (!sleeping_ || body_ == nullptr) return;
  uint32_t now_ms = esphome::millis();
  base_ = ASLEEP;
  expression_until_ = 0;
  sign_until_ = sign_until_ ? now_ms + 10 : 0;
  apply_expression(ASLEEP);
  wilt_until_ = now_ms + 600000;  // arms hang; sleep_tick extends it
  if (kind_ == OKTI) {
    if (bed_ == nullptr) {
      bed_ = shape(root_, 0, 0, 320, 240, 0, 0);
      transparent(bed_);
      shape(bed_, 60, 196, 200, 14, 0x8B5A2B, 4);   // bed frame
      shape(bed_, 66, 206, 10, 22, 0x6B4226, 2);    // legs
      shape(bed_, 244, 206, 10, 22, 0x6B4226, 2);
      lv_obj_t *blanket = shape(bed_, 70, 146, 180, 58, 0x5A7AE8, 18);
      border(blanket, 3, 0x3A5AC8);
      for (int k = 0; k < 4; k++) shape(blanket, 18 + k * 40, 8, 18, 18, 0x7FA0FF, 9);  // dots
      lv_obj_t *envelope = shape(bed_, 76, 146, 168, 14, 0xF4F4F4, 7);  // folded-over sheet
      (void) envelope;
    }
    show(bed_, true);
    lv_obj_move_foreground(bed_);
    lv_obj_move_foreground(sign_);
  } else if (kind_ == ROBOT && visor_) {
    show(face_, false);
    show(zzz_, false);
    lv_obj_set_style_bg_color(visor_, lv_color_hex(0x07090E), 0);
    if (standby_ == nullptr) standby_ = shape(visor_, 62, 34, 16, 16, 0xFF8A1A);
    show(standby_, true);
    if (antenna_) lv_obj_set_style_bg_color(antenna_, lv_color_hex(0x3A3F4A), 0);
  } else if (kind_ == FOX) {
    if (!mouse_start_) mouse_y_ = 22;  // sits lower, curled up
    if (tail_) {  // tail to the front
      for (auto *l : {tail_edge_, tail_belly_, tail_, tail_tip_}) if (l) lv_obj_move_foreground(l);
      for (auto *l : tip_) if (l) lv_obj_move_foreground(l);
    }
    body_layout();
  }
}

inline void Companion::sleep_tick(uint32_t now_ms) {
  wilt_until_ = now_ms + 600000;
  if (standby_) lv_obj_set_style_bg_opa(standby_, (lv_opa_t) (110 + 145 * (0.5f + 0.5f * sinf(now_ms / 900.0f))), 0);
  if (kind_ == ROBOT && heart_) lv_obj_set_style_bg_opa(heart_, 40, 0);
}

inline void Companion::light_scare() {
  if (sleeping_ || away_) return;
  scare_at_ = esphome::millis() + (kind_ == FOX ? 2300 : 600);  // the fox walks in first
}

inline void Companion::scare_tick(uint32_t now_ms) {
  if (!passed(now_ms, scare_at_)) return;
  scare_at_ = 0;
  if (!visible_) return;
  led_flash(0xFFFFFF, 300);
  if (kind_ == FOX) {  // brave: only a short blink
    anim(cb_blink, 0, 80, 90, 250, 1);
    show_later(txt::WAS_AWAKE, 900);
    return;
  }
  for_a_while(SURPRISED, 1300);
  anim(cb_hop, 0, 18, 120, 220, 1, lv_anim_path_ease_out);
  anim(cb_blink, 0, 90, 80, 600, 1);  // squeeze the eyes shut
  wiggle_until_ = now_ms + 900;
  show_later(kind_ == ROBOT ? txt::BRIGHTNESS_800 : (rnd(2) ? txt::LIGHT_EYES : txt::WHO_TURNED_ON), 1300);
}

}  // namespace yellow
