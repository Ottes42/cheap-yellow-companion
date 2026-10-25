#pragma once
// Yellow: build and draw the character: body, arms/tentacles/tail, eyes, color, expression (methods of Companion, see yellow.h)
#include "yellow.h"

namespace yellow {

inline void Companion::build_figure() {
  if (body_ != nullptr) lv_obj_delete(body_);
  antenna_ = head_ = cap_ = glasses_ = scarf_ = umbrella_ = fan_ = can_ = hat_glyph_ = joint_ = nullptr;
  basket_ = bin_ = pillow_ = nullptr;
  jewelry_ = jewelry_shine_ = nullptr;
  visor_ = nullptr;
  glitch_start_ = 0;
  prop_[0] = prop_[1] = nullptr;
  prop_start_ = chase_start_ = 0;
  gear_[0] = gear_[1] = nullptr;
  flap_start_ = 0;
  sweat_ = tear_ = nullptr;
  blush_on_ = false;
  arm0_front_ = false;
  brow_[0] = brow_[1] = nullptr;
  arm_[0] = arm_[1] = nullptr;
  for (int i = 0; i < ARMS; i++) tentacle_[i] = underside_[i] = nullptr;
  for (auto &arm : sucker_)
    for (auto *&s : arm) s = nullptr;
  plan_ = kind_ == OKTI ? &PLAN_OKTI : kind_ == FOX ? &PLAN_FOX : &PLAN_ROBOT;
  robot_arm_[0] = robot_arm_[1] = robot_hand_[0] = robot_hand_[1] = heart_ = nullptr;
  tail_ = tail_tip_ = tail_edge_ = tail_belly_ = nullptr;
  for (auto *&l : tip_) l = nullptr;
  paw_[0] = paw_[1] = nullptr;
  const Blueprint &p = *plan_;
  body_ = shape(root_, p.kx, p.ky, 180, 170, 0, 0);
  transparent(body_);
  // LVGL draws children only inside the parent's area (+ ext_draw_size): arms, pumpkin and antenna
  // stick out, so extend the drawing area all around
  lv_obj_add_event_cb(body_, cb_border, LV_EVENT_REFR_EXT_DRAW_SIZE, nullptr);
  lv_obj_refresh_ext_draw_size(body_);
  lv_obj_t *carrier;  // carries the face
  if (kind_ == OKTI) {
    // arms behind the head: first the differently coloured underside (wider, slightly lower), then the arm
    for (int i = 0; i < ARMS; i++) underside_[i] = line(body_, 17, RED);
    for (int i = 0; i < ARMS; i++) tentacle_[i] = line(body_, 13, YELLOW);
    for (int i = 0; i < ARMS; i++) {
      for (lv_obj_t *l : {underside_[i], tentacle_[i]}) {
        lv_obj_set_pos(l, -ARM_DX, -ARM_DY);
        lv_obj_set_size(l, 360, 300);
      }
    }
    for (auto &arm : sucker_)  // suckers in front of the arms, behind the head
      for (auto *&s : arm) s = shape(body_, 0, 0, 5, 5, 0xFFE2B0);
    head_ = shape(body_, 15, 0, 150, 126, YELLOW, 62);
    lv_obj_t *shine = shape(head_, 30, 14, 36, 18, 0xFFFFFF, 9);  // highlight top left
    lv_obj_set_style_bg_opa(shine, 80, 0);
    carrier = head_;
  } else if (kind_ == FOX) {
    // Fox: tail (behind), ears as triangles of fanned lines, body with belly, head with a white snout
    // bushy: dark edge and a bulging middle under the actual tail, big white tip
    tail_edge_ = segment_line(body_, 48, 0xC85E12);
    tail_belly_ = segment_line(body_, 42, FOX_ORANGE);
    tail_ = segment_line(body_, 28, FOX_ORANGE);
    tail_tip_ = segment_line(body_, 32, FOX_CREAM);
    // round tip as before (pointed and "slightly tapering" tried, the owner liked round best, 2026-10-25)
    // ears a bit rounder (thicker lines, round ends); a kink in the right ear was tried and dropped (owner, 2026-10-25)
    for (int i = 0; i < 2; i++) {
      auto x = [i](int v) { return i ? 180 - v : v; };
      triangle_lines(body_, EAR_TIP[i][0][0], EAR_TIP[i][0][1], x(14), 36, x(70), 20, FOX_ORANGE, 20,
                     ear_pts_[i][0]);
      triangle_lines(body_, EAR_TIP[i][1][0], EAR_TIP[i][1][1], x(26), 28, x(56), 20, FOX_CREAM, 9,
                     ear_pts_[i][1]);
    }
    lv_obj_t *torso = shape(body_, 35, 104, 110, 62, FOX_ORANGE, 30);
    shape(torso, 32, 10, 46, 44, FOX_CREAM);
    paw_[0] = shape(body_, 44, 156, 28, 14, FOX_BROWN);
    paw_[1] = shape(body_, 108, 156, 28, 14, FOX_BROWN);
    head_ = shape(body_, 15, 12, 150, 110, FOX_ORANGE, 46);
    shape(head_, 26, 56, 98, 58, FOX_CREAM);  // white snout/cheeks
    shape(head_, 67, 60, 16, 11, FOX_BROWN, 5);  // nose
    carrier = head_;
  } else {
    // Robot: antenna, ear screws, head with a dark visor, body with a pulsing heart light, arms with hands
    shape(body_, 88, -10, 4, 22, METAL, 2);
    antenna_ = shape(body_, 83, -22, 14, 14, 0xFF5A7A);
    shape(body_, -2, 40, 12, 32, METAL, 5);
    shape(body_, 170, 40, 12, 32, METAL, 5);
    shape(body_, 48, 164, 30, 10, METAL, 4);
    shape(body_, 102, 164, 30, 10, METAL, 4);
    for (int i = 0; i < 2; i++) robot_arm_[i] = segment_line(body_, 9, METAL);
    lv_obj_t *torso = shape(body_, 40, 118, 100, 50, YELLOW, 16);
    border(torso, 3, YELLOW_EDGE);
    heart_ = shape(torso, 40, 14, 14, 14, 0xFF5A7A);
    lv_obj_t *head = shape(body_, 5, 4, 170, 116, YELLOW, 28);
    border(head, 3, YELLOW_EDGE);
    for (int k = 0; k < 2; k++) {  // gears behind the visor (visible when it flips up)
      gear_[k] = shape(head, k ? 84 : 36, k ? 34 : 24, k ? 34 : 44, k ? 34 : 44, 0x5A606E);
      border(gear_[k], 5, 0x8A93A6);
      lv_obj_set_style_border_side(gear_[k], LV_BORDER_SIDE_FULL, 0);
      shape(gear_[k], k ? 10 : 15, k ? 10 : 15, 8, 8, 0x2A2F3A);
    }
    carrier = shape(head, 12, 13, 140, 82, VISOR, 22);
    visor_ = carrier;
    for (auto *&s : scan_) {
      s = shape(visor_, 0, 0, 140, 3, 0x7FE8FF, 0);
      lv_obj_set_style_bg_opa(s, 120, 0);
      show(s, false);
    }
  }

  face_ = shape(carrier, 0, 0, 180, 150, 0, 0);
  transparent(face_);
  int b = p.arc;
  for (int i = 0; i < 2; i++) {
    cheek_[i] = shape(face_, p.cheek_x[i], p.cheek_y, p.cheek_w, p.cheek_h, CHEEK);
    eye_[i] = shape(face_, 0, 0, 24, 40, p.eye_color);
    shine_[i] = shape(eye_[i], 3, 4, 6, 6, 0xFFFFFF);
    happy_[i] = arc(face_, b, 5, p.eye_color);
    lv_arc_set_bg_angles(happy_[i], 200, 340);
    lv_obj_set_pos(happy_[i], p.eye_x[i] - b / 2, p.eye_y - b / 3);
    spiral_[i] = arc(face_, b, 4, p.eye_color);
    lv_arc_set_bg_angles(spiral_[i], 0, 290);
    lv_obj_set_pos(spiral_[i], p.eye_x[i] - b / 2, p.eye_y - b / 2);
  }
  {  // grumpy brows (all characters; robot in glow colour on the dark visor)
    for (int i = 0; i < 2; i++) {
      int x = p.eye_x[i], s = i ? -1 : 1;
      brow_pts_[i][0].x = x - 12 * s;
      brow_pts_[i][0].y = p.eye_y - 24;
      brow_pts_[i][1].x = x + 9 * s;
      brow_pts_[i][1].y = p.eye_y - 15;
      brow_[i] = line(face_, 5, kind_ == ROBOT ? GLOW : EYE);
      lv_line_set_points(brow_[i], brow_pts_[i], 2);
    }
  }
  mouth_arc_ = arc(face_, p.mouth_arc, 4, p.mouth_color);
  mouth_o_ = shape(face_, 0, 0, 14, 16, p.mouth_color);
  mouth_line_ = shape(face_, p.mouth_x - 10, p.mouth_y, 20, 4, p.mouth_color, 2);

  // props: sunglasses (on the face), scarf (below the face), umbrella (above the head)
  glasses_ = shape(face_, 0, 0, 180, 150, 0, 0);  // full size: LVGL draws children only inside the parent area
  lv_obj_set_style_bg_opa(glasses_, LV_OPA_TRANSP, 0);
  lv_obj_add_flag(glasses_, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
  for (int i = 0; i < 2; i++) {
    lv_obj_t *g = shape(glasses_, p.eye_x[i] - 17, p.eye_y - 13, 34, 24, 0x101018, 9);
    border(g, 2, 0x3A3A48);
    shape(g, 5, 4, 8, 4, 0x6A6A88, 2);  // shine
  }
  shape(glasses_, (p.eye_x[0] + p.eye_x[1]) / 2 - 7, p.eye_y - 6, 14, 4, 0x101018, 2);
  scarf_ = shape(face_, p.mouth_x - 52, p.mouth_y + 14, 104, 16, 0xC8463C, 8);
  shape(scarf_, 10, 0, 6, 16, 0xF4E4C1, 0);
  shape(scarf_, 40, 0, 6, 16, 0xF4E4C1, 0);
  shape(scarf_, 70, 0, 6, 16, 0xF4E4C1, 0);
  lv_obj_add_flag(scarf_, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
  shape(scarf_, 72, 12, 16, 30, 0xC8463C, 6);  // hanging end
  umbrella_ = shape(body_, 74, -26, 180, 170, 0, 0);  // to the right over the shoulder
  lv_obj_set_style_bg_opa(umbrella_, LV_OPA_TRANSP, 0);
  lv_obj_add_flag(umbrella_, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
  // roof: circle in a box half as high = filled half circle, plus arc edges like umbrella segments
  lv_obj_t *roof = shape(umbrella_, 52, 2, 104, 52, 0, 0);
  lv_obj_set_style_bg_opa(roof, LV_OPA_TRANSP, 0);
  shape(roof, 0, 0, 104, 104, 0x4A7AE8);
  for (int k = 0; k < 3; k++) shape(roof, 8 + k * 32, 44, 24, 16, BACKGROUND);  // wavy edge
  umbrella_stick_[0].x = 104;
  umbrella_stick_[0].y = 10;
  umbrella_stick_[1].x = 112;
  umbrella_stick_[1].y = 128;
  lv_obj_t *stem = line(umbrella_, 4, 0x3A2418);
  lv_obj_set_size(stem, 180, 170);
  lv_obj_set_pos(stem, 0, 0);
  lv_line_set_points(stem, umbrella_stick_, 2);
  lv_obj_move_background(umbrella_);  // the stick disappears behind the head
  // fan left (half circle with stripes), watering can right (can, spout, handle)
  fan_ = shape(body_, -40, 70, 80, 40, 0, 0);
  lv_obj_set_style_bg_opa(fan_, LV_OPA_TRANSP, 0);
  lv_obj_t *fb = shape(fan_, 0, 0, 80, 40, 0, 0);
  lv_obj_set_style_bg_opa(fb, LV_OPA_TRANSP, 0);
  shape(fb, 0, 0, 80, 80, 0xFF8FA3);
  for (int k = 0; k < 4; k++) shape(fb, 10 + k * 18, 0, 4, 40, 0xC8463C, 0);
  shape(fan_, 36, 34, 8, 6, 0x6B4226, 2);
  can_ = shape(body_, 150, 96, 90, 60, 0, 0);
  lv_obj_set_style_bg_opa(can_, LV_OPA_TRANSP, 0);
  shape(can_, 20, 20, 44, 36, 0x3D9A5A, 8);
  lv_obj_t *handle = arc(can_, 28, 5, 0x2E7A44);
  lv_arc_set_bg_angles(handle, 180, 360);
  lv_obj_set_pos(handle, 28, 8);
  can_spout_[0].x = 22;
  can_spout_[0].y = 40;
  can_spout_[1].x = 2;
  can_spout_[1].y = 22;
  lv_obj_t *spout = line(can_, 6, 0x2E7A44);
  lv_obj_set_size(spout, 90, 60);
  lv_line_set_points(spout, can_spout_, 2);
  for (int k = 0; k < 3; k++) shape(can_, 0 + k * 5, 26 + k * 7, 4, 6, 0x6FB8FF, 2);  // drops
  // a "joint" for 16:20 (comic style): white roll at the mouth corner, ember, little smoke clouds rise
  joint_ = shape(face_, p.mouth_x + 8, p.mouth_y - 2, 30, 6, 0xF4F4F4, 3);
  lv_obj_add_flag(joint_, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
  shape(joint_, 26, 0, 6, 6, 0xFF6A1A, 3);
  for (int k = 0; k < 3; k++) {  // smoke right at the face: as a child of the 6 px high joint it was not drawn
    smoke_[k] = shape(face_, 0, 0, 10, 10, 0xB8BEC8);
    show(smoke_[k], false);
  }
  // laundry basket in front of the belly (with colourful laundry), trash can to the right, pillow left at the head
  basket_ = shape(body_, 46, 118, 88, 40, 0, 0);
  lv_obj_set_style_bg_opa(basket_, LV_OPA_TRANSP, 0);
  shape(basket_, 6, 0, 22, 18, 0x4AA8FF);
  shape(basket_, 30, 2, 24, 16, 0xFF8FA3);
  shape(basket_, 56, 0, 24, 18, 0x5AE07A);
  lv_obj_t *kb = shape(basket_, 0, 12, 88, 28, 0xC8A06A, 8);
  for (int k = 0; k < 4; k++) shape(kb, 10 + k * 20, 4, 4, 20, 0x9A7444, 2);
  bin_ = shape(body_, 176, 92, 44, 66, 0, 0);
  lv_obj_set_style_bg_opa(bin_, LV_OPA_TRANSP, 0);
  shape(bin_, 0, 0, 44, 10, 0x3A3F4A, 3);
  shape(bin_, 3, 9, 38, 50, 0x5A606E, 4);
  shape(bin_, 6, 56, 10, 10, 0x1A1A22);
  shape(bin_, 28, 56, 10, 10, 0x1A1A22);
  pillow_ = shape(body_, -44, 52, 64, 36, 0xBFD8FF, 14);
  shape(pillow_, 10, 8, 44, 4, 0x9FC0F0, 2);
  props_layout();

  if (kind_ == ROBOT) {  // hands on the arms (in front of head and body so they hold the sign)
    for (int i = 0; i < 2; i++) robot_hand_[i] = shape(body_, 0, 0, 16, 16, 0x6A7386);
  } else if (kind_ == FOX) {  // front paws for the sign
    for (int i = 0; i < 2; i++) {
      arm_[i] = shape(body_, 0, 0, 26, 18, FOX_BROWN);
      shape(arm_[i], 9, 4, 8, 6, 0x6A4430, 3);  // pads
    }
  }

  // pumpkin (Halloween)
  stem_ = shape(body_, p.hat_x + 29, p.hat_y - 9, 8, 12, 0x3D7A2A, 3);
  pumpkin_ = shape(body_, p.hat_x, p.hat_y, 66, 30, 0xF28C28);
  border(pumpkin_, 2, 0xA84E10);
  shape(pumpkin_, 24, 2, 14, 22, 0xE07A1C);
  hat_glyph_ = lv_label_create(body_);
  if (decor_font_) lv_obj_set_style_text_font(hat_glyph_, decor_font_, 0);
  lv_obj_set_pos(hat_glyph_, p.hat_x + 18, p.hat_y + (kind_ == OKTI ? 10 : -4));  // Okti: the head reaches to the top
  // hat/crown/star 1.8×: the 30 px symbol was tiny on the head; grows upwards from the bottom edge
  lv_obj_set_style_transform_pivot_x(hat_glyph_, 15, 0);
  lv_obj_set_style_transform_pivot_y(hat_glyph_, 30, 0);
  lv_obj_set_style_transform_scale(hat_glyph_, 460, 0);
  lv_label_set_text(hat_glyph_, "");
  // Santa hat: red, white brim, bobble
  cap_ = shape(body_, p.hat_x + 4, p.hat_y - 6, 58, 30, RED, 14);
  shape(cap_, 40, -6, 14, 14, 0xF4F4F4);
  lv_obj_add_flag(cap_, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
  lv_obj_t *brim = shape(body_, p.hat_x - 2, p.hat_y + 20, 70, 11, 0xF4F4F4, 5);
  lv_obj_set_parent(brim, cap_);
  lv_obj_set_pos(brim, -6, 26);
  hat();

  // closed envelope with a seal while post is waiting (an arm holds it)
  letter_ = shape(body_, LETTER_X - 23, LETTER_Y - 16, 46, 32, 0xF4E4C1, 4);
  border(letter_, 2, 0x8B6B3E);
  lv_obj_add_flag(letter_, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
  letter_flap_obj_ = line(letter_, 2, 0x8B6B3E);
  lv_obj_set_size(letter_flap_obj_, 46, 50);
  lv_obj_set_pos(letter_flap_obj_, 0, -14);
  flap(false);
  letter_seal_ = shape(letter_, 17, 10, 9, 9, RED);
  letter_layout();

  // jewelry: chain as a flat arc where head and body meet; position per character (center y, radius)
  {
    int my = kind_ == OKTI ? 70 : kind_ == FOX ? 80 : 90, r = kind_ == OKTI ? 60 : kind_ == FOX ? 50 : 40;
    jewelry_ = shape(body_, 0, 0, 180, 170, 0, 0);
    transparent(jewelry_);
    if (kind_ == FOX) {  // fox: old-gold chain on the light chest
      lv_obj_t *chain = arc(jewelry_, 2 * r, 5, 0xB8860B);
      lv_arc_set_bg_angles(chain, 52, 128);
      lv_obj_set_pos(chain, 90 - r, my - r);
    } else {  // Okti/robot: gold hardly shows on yellow → pearl necklace
      for (int k = 0; k < 9; k++) {
        float w = (52 + k * 9.5f) * 0.017453f;
        lv_obj_t *pearl = shape(jewelry_, 90 + (int) (r * cosf(w)) - 4, my + (int) (r * sinf(w)) - 4, 9, 9, 0xF4F4F8);
        border(pearl, 1, 0xC8C8D8);
      }
    }
    lv_obj_t *anh = shape(jewelry_, 81, my + r - 4, 18, 20, 0xFF3A8C, 9);  // pendant: pink gem in gold
    border(anh, 3, 0xB8860B);
    jewelry_shine_ = shape(anh, 3, 3, 5, 5, 0xFFFFFF);
    earrings_[0] = earrings_[1] = nullptr;
    if (kind_ == FOX)
      for (int i = 0; i < 2; i++) {
        earrings_[i] = shape(body_, i ? 144 : 26, 22, 11, 11, 0x7FE8FF);  // sparkling stone
        border(earrings_[i], 2, 0xB8860B);
      }
    jewelry(jewelry_on_);
  }

  zzz_ = lv_label_create(body_);
  lv_obj_set_style_text_color(zzz_, lv_color_hex(0x9FC4FF), 0);
  lv_label_set_text(zzz_, "z Z");
  lv_obj_set_pos(zzz_, p.zzz_x, p.zzz_y);

  // effects (all characters): sweat drop on the forehead, tear below the left eye,
  // blush = three strokes per cheek, sparkle = four little stars around the head
  sweat_ = shape(face_, 0, 0, 9, 13, 0x8FD3FF, 5);
  tear_ = shape(face_, 0, 0, 9, 13, 0x8FD3FF, 5);
  for (int i = 0; i < 2; i++)
    for (int k = 0; k < 3; k++) {
      blush_[i][k] = shape(cheek_[i], 2 + k * p.cheek_w / 3, 1, 2, std::max(4, p.cheek_h - 2), 0xE0405A, 1);
    }
  static const int SPARKLE[4][2] = {{-4, 6}, {176, 16}, {6, 96}, {168, 92}};
  for (int i = 0; i < 4; i++) {
    sparkle_[i] = shape(body_, SPARKLE[i][0], SPARKLE[i][1], 14, 14, 0, 0);
    transparent(sparkle_[i]);
    shape(sparkle_[i], 6, 0, 2, 14, 0xFFF3A0, 1);
    shape(sparkle_[i], 0, 6, 14, 2, 0xFFF3A0, 1);
  }
  effect_tick(esphome::millis());

  if (away_) show(body_, false);
  standby_ = nullptr;
  color_st_ = -1;
  apply_expression(base_);
  body_layout();
  face_layout();
  arms_layout();
  sign_layout();
  draw_tentacles();
  if (sleeping_) sleep_scene();
}

inline void Companion::color(int st) {
  if (kind_ == FOX && st != color_st_) {  // fox: puts his ears back when in a bad mood (tips out and down)
    color_st_ = st;
    bool on = st >= 70;
    for (int i = 0; i < 2; i++) {
      int s = i ? -1 : 1;
      for (int j = 0; j < 2; j++)
        for (int k = 0; k < 5; k++) {
          ear_pts_[i][j][k][0].x = EAR_TIP[i][j][0] - (on ? 16 * s : 0) + ARM_DX;
          ear_pts_[i][j][k][0].y = EAR_TIP[i][j][1] + (on ? 22 : 0) + ARM_DY;
        }
    }
    if (body_) lv_obj_invalidate(body_);
    return;
  }
  if (kind_ == ROBOT && visor_ && st != color_st_) {  // robot: the visor glows reddish when in a bad mood
    color_st_ = st;
    lv_obj_set_style_bg_color(visor_, lv_color_mix(lv_color_hex(0x4A1626), lv_color_hex(VISOR), (lv_opa_t) (st * 255 / 100)), 0);
    return;
  }
  if (kind_ != OKTI || tentacle_[0] == nullptr || st == color_st_) return;
  bool turn = color_st_ >= 0 && abs(st - color_st_) >= 40;
  color_st_ = st;
  lv_opa_t m = (lv_opa_t) (st * 255 / 100);
  lv_color_t up = lv_color_mix(lv_color_hex(RED), lv_color_hex(pink_ ? 0xFF9EC0 : YELLOW), m);  // Valentine's Day: pink
  lv_color_t down = lv_color_mix(lv_color_hex(YELLOW), lv_color_hex(RED), m);
  lv_obj_set_style_bg_color(head_, up, 0);
  for (int i = 0; i < ARMS; i++) {
    lv_obj_set_style_line_color(tentacle_[i], up, 0);
    lv_obj_set_style_line_color(underside_[i], down, 0);
  }
  if (turn) anim(cb_hop, 0, 16, 140, 160, 1, lv_anim_path_ease_out);  // flip hop
}

inline void Companion::apply_expression(Expression a) {
  expression_ = a;
  if (body_ == nullptr) return;
  const Blueprint &p = *plan_;
  int w = 24, h = 40, r = LV_RADIUS_CIRCLE;
  switch (a) {
    case SURPRISED: w = 32; h = 42; break;
    case TIRED: w = 26; h = 12; r = 6; break;
    case ANNOYED: w = 26; h = 20; r = 5; break;
    case SAD: w = 22; h = 34; break;
    case ASLEEP: w = 26; h = 4; r = 2; break;
    case YAWN: w = 26; h = 6; r = 3; break;
    default: break;
  }
  eyes_w_ = (int) lroundf(w * p.eye_scale);
  eyes_h_ = std::max(3, (int) lroundf(h * p.eye_scale));
  int st = (a == ANNOYED || a == SAD) ? 100 : std::max(0, std::min(100, mood_ + weather_mood_));
  color(st);
  bool grumpy = st >= 70;
  bool bad = a == NORMAL && st >= 35 && st < 70;  // in between: straight mouth
  for (int i = 0; i < 2; i++) {
    lv_obj_set_style_radius(eye_[i], r, 0);
    show(eye_[i], eyes_round());
    show(happy_[i], a == HAPPY);
    show(spiral_[i], a == DIZZY);
    show(brow_[i], grumpy && a != HAPPY && a != ASLEEP && a != SAD);  // sad without angry brows
    lv_obj_set_style_bg_opa(cheek_[i], a == HAPPY ? 220 : (lv_opa_t) (120 * (100 - st) / 100), 0);
  }
  if (a == DIZZY)
    anim(cb_spiral, 0, 360, 700, 0, LV_ANIM_REPEAT_INFINITE, lv_anim_path_linear);
  else
    lv_anim_delete(this, cb_spiral);
  show(zzz_, a == ASLEEP);
  if (a == ASLEEP)
    anim(cb_zzz, 0, 10, 1400, 1400, LV_ANIM_REPEAT_INFINITE);
  else
    lv_anim_delete(this, cb_zzz);

  // mouth
  int mb = p.mouth_arc;
  bool cross = grumpy && (a == NORMAL || a == ANNOYED || a == SAD);
  show(mouth_arc_, (a == NORMAL && !bad) || a == HAPPY || cross);
  show(mouth_o_, a == SURPRISED || a == DIZZY || a == YAWN || a == ASLEEP);
  show(mouth_line_, a == TIRED || bad);
  if (cross) {  // mouth corners down
    lv_arc_set_bg_angles(mouth_arc_, 220, 320);
    lv_obj_set_pos(mouth_arc_, p.mouth_x - mb / 2, p.mouth_y - mb / 20);
  } else if (a == NORMAL) {
    lv_arc_set_bg_angles(mouth_arc_, 45, 135);
    lv_obj_set_pos(mouth_arc_, p.mouth_x - mb / 2, p.mouth_y - mb * 7 / 10);
  } else if (a == HAPPY) {
    lv_arc_set_bg_angles(mouth_arc_, 20, 160);
    lv_obj_set_pos(mouth_arc_, p.mouth_x - mb / 2, p.mouth_y - mb * 3 / 4);
  }
  int ow = 14, oh = 16;
  if (a == YAWN) { ow = 20; oh = 26; }
  if (a == DIZZY) { ow = 10; oh = 12; }
  if (a == ASLEEP) { ow = 8; oh = 8; }
  ow = (int) lroundf(ow * p.eye_scale);
  oh = (int) lroundf(oh * p.eye_scale);
  lv_obj_set_size(mouth_o_, ow, oh);
  lv_obj_set_pos(mouth_o_, p.mouth_x - ow / 2, p.mouth_y - oh / 2);

  blink_ = 0;
  eyes_layout();
}

inline void Companion::tentacle_tick(uint32_t now_ms) {
  if (kind_ == ROBOT && heart_) {  // heart light pulses, antenna blinks
    lv_obj_set_style_bg_opa(heart_, (lv_opa_t) (150 + 105 * sinf(now_ms / 350.0f)), 0);
    if (antenna_) lv_obj_set_style_bg_color(antenna_, lv_color_hex((now_ms / 900) % 2 ? 0xFF5A7A : GLOW), 0);
  }
  bool wiggle = !passed(now_ms, wiggle_until_) || petting_active_;
  bool active = wiggle || grab_arm_ >= 0 || (wave_until_ && !passed(now_ms, wave_until_)) ||
               (arm_pos_ > 0 && arm_pos_ < 100) || kind_ == ROBOT;
  if (!active && now_ms - last_tentacle_ < 60) return;
  float dt = std::min(0.2f, (now_ms - last_tentacle_) / 1000.0f);
  last_tentacle_ = now_ms;
  phase_ += dt * (wiggle ? 5.0f : 0.7f) * (0.6f + tension_ / 60.0f) * (0.6f + energy_ / 250.0f);
  if (phase_ > 1000.0f) phase_ -= 1000.0f;
  draw_tentacles();
  robot_arms();
  fox_tail();
  if (kind_ == FOX) arms_layout();
}

inline void Companion::robot_arms() {
  if (kind_ != ROBOT || robot_arm_[0] == nullptr) return;
  uint32_t now_ms = esphome::millis();
  bool wiggle = !passed(now_ms, wiggle_until_) || petting_active_;
  bool waving = wave_until_ && !passed(now_ms, wave_until_);
  float h = arm_pos_ / 100.0f;
  static const float SHOULDER[2][2] = {{42, 132}, {138, 132}};
  for (int i = 0; i < 2; i++) {
    float o = i ? 60 : 120, u = i ? 80 : 100;          // rest: hang diagonally outwards
    float oz = i ? -55 : 235, uz = i ? -85 : 265;       // sign: up to the corners
    o += (oz - o) * h;
    u += (uz - u) * h;
    float l1 = 32, l2 = 30;
    if (wiggle) {
      o += 18 * sinf(phase_ * 2 + i);
      u += 25 * sinf(phase_ * 3 + i);
    }
    if (wilt_until_ && !passed(now_ms, wilt_until_)) {  // wilt: arms hang limp and vertical
      o = u = 90 + (i ? -4 : 4);
    }
    if (weather_wind_) {  // wind blows the arms to the right
      o -= 22 + 6 * sinf(phase_ * 3 + i);
      u -= 30 + 8 * sinf(phase_ * 3.5f + i);
    }
    if (waving && i == 1 && h <= 0) {
      o = -70 + 12 * sinf(phase_ * 4);
      u = -110 + 30 * sinf(phase_ * 6);
    }
    int gx = grab_x_, gy = grab_y_;
    bool holds = i != grab_arm_ && h <= 0 && !(waving && i == 1) && grip(i, gx, gy);  // the hand holds the prop
    if (i == grab_arm_ || holds) {  // grab: both segments towards the target, stretched
      float dx = gx - SHOULDER[i][0], dy = gy - SHOULDER[i][1];
      o = u = atan2f(dy, dx) * 57.2958f;
      l1 = l2 = std::min(48.0f, std::max(26.0f, sqrtf(dx * dx + dy * dy) / 2));
    }
    float x0 = SHOULDER[i][0], y0 = SHOULDER[i][1];
    float x1 = x0 + l1 * cosf(o * 0.017453f), y1 = y0 + l1 * sinf(o * 0.017453f);
    float x2 = x1 + l2 * cosf(u * 0.017453f), y2 = y1 + l2 * sinf(u * 0.017453f);
    robot_pts_[i][0].x = x0 + ARM_DX;
    robot_pts_[i][0].y = y0 + ARM_DY;
    robot_pts_[i][1].x = lroundf(x1) + ARM_DX;
    robot_pts_[i][1].y = lroundf(y1) + ARM_DY;
    robot_pts_[i][2].x = lroundf(x2) + ARM_DX;
    robot_pts_[i][2].y = lroundf(y2) + ARM_DY;
    lv_line_set_points(robot_arm_[i], robot_pts_[i], 3);
    if (robot_hand_[i]) lv_obj_set_pos(robot_hand_[i], (int) x2 - 8, (int) y2 - 8);
  }
}

inline void Companion::fox_tail() {
  if (kind_ != FOX || tail_ == nullptr) return;
  uint32_t now_ms = esphome::millis();
  bool doll = !passed(now_ms, wiggle_until_) || petting_active_ || expression_ == HAPPY;
  float amp = doll ? 22.0f : 7.0f;
  float x = 146, y = 150, w = 5;
  for (int k = 0; k <= 4; k++) {
    if (k > 0) {
      w = 5 - 19 * k + amp * sinf(phase_ * (doll ? 3.0f : 1.0f)) * k / 4;  // swings to the top right
      if (weather_wind_) w += (22 + 8 * sinf(phase_ * 3)) * k / 4;  // wind bends the tail to the right
      if (wilt_until_ && !passed(now_ms, wilt_until_)) w = 30 + 12 * k;  // wilt: tail hangs down
      if (sleeping_) w = 165 + 14 * k;  // sleeping: tail laid around the front of the body
      if (tail_fan_until_ && !passed(now_ms, tail_fan_until_))  // fans in front of the face
        w = -100 - 22 * k + 35 * sinf(now_ms / 70.0f) * k / 4;
      x += 20 * cosf(w * 0.017453f);
      y += 20 * sinf(w * 0.017453f);
    }
    tail_pts_[k].x = lroundf(x) + ARM_DX;
    tail_pts_[k].y = lroundf(y) + ARM_DY;
  }
  lv_line_set_points(tail_, tail_pts_, 5);
  lv_line_set_points(tail_edge_, &tail_pts_[1], 3);
  lv_line_set_points(tail_belly_, &tail_pts_[1], 3);
  lv_line_set_points(tail_tip_, &tail_pts_[3], 2);
  // slightly tapering: short, narrower extension beyond the last point
  float dx = tail_pts_[4].x - tail_pts_[3].x, dy = tail_pts_[4].y - tail_pts_[3].y;
  float l = std::max(1.0f, sqrtf(dx * dx + dy * dy));
  tip_pts_[0][0] = tail_pts_[4];
  tip_pts_[0][1].x = lroundf(tail_pts_[4].x + dx / l * 11);
  tip_pts_[0][1].y = lroundf(tail_pts_[4].y + dy / l * 11);
  if (tip_[0]) lv_line_set_points(tip_[0], tip_pts_[0], 2);
}

inline void Companion::draw_tentacles() {
  if (kind_ != OKTI || tentacle_[0] == nullptr) return;
  uint32_t now_ms = esphome::millis();
  bool wiggle = !passed(now_ms, wiggle_until_) || petting_active_;
  bool waving = wave_until_ && !passed(now_ms, wave_until_);
  float amp = wiggle ? 15.0f : 4.5f;
  float hold = arm_pos_ / 100.0f;
  // the arm pointing at himself must be in front of the head, otherwise it disappears behind it
  bool shows = !post_.empty() && hold <= 0 && grab_arm_ != 0;
  if (shows != arm0_front_) {
    arm0_front_ = shows;
    if (shows) {
      lv_obj_move_foreground(underside_[0]);
      lv_obj_move_foreground(tentacle_[0]);
    } else {
      lv_obj_move_to_index(underside_[0], 0);
      lv_obj_move_to_index(tentacle_[0], ARMS);
    }
  }
  bool wilt = wilt_until_ && !passed(now_ms, wilt_until_);
  for (int i = 0; i < ARMS; i++) {
    float w = ARM_ANGLE[i];
    float curve = i < ARMS / 2 ? 11.0f : -11.0f;  // tips curl up
    float wa = amp;
    if (wilt) {  // limp: all arms hang almost vertically, tips inwards
      w = 90 + (w - 90) * 0.35f;
      curve = i < ARMS / 2 ? -4.0f : 4.0f;
      wa = 1.5f;
    }
    if (weather_wind_ && !wilt) w -= 28.0f + 10.0f * sinf(phase_ * 2.5f + i * 0.4f);  // wind blows the arms to the right
    if (knot_until_ && !passed(now_ms, knot_until_)) {  // knotted: arms cross to the other side and twist
      w = 180 - w + 8 * sinf(phase_ * 6 + i);
      curve = (i % 2 ? 1 : -1) * 22.0f;
      wa = 2.0f;
    }
    // hold the sign: the outer arms reach up to the sign's corners
    if ((i == 0 || i == ARMS - 1) && hold > 0) {
      float target = i == 0 ? 248.0f : -68.0f;
      w = w + (target - w) * hold;
      curve = curve * (1 - hold) + (i == 0 ? 6.0f : -6.0f) * hold;
      wa = amp * (1 - hold) + 3.0f * hold;
    }
    // waving: the right outer arm up and back and forth
    if (waving && i == ARMS - 1 && hold <= 0) {
      w = -75.0f + 22.0f * sinf(phase_ * 4.0f);
      curve = -6.0f;
      wa = 4.0f;
    }
    float seg = ARM_LENGTH[i] / (float) SEGMENTS;
    // post waiting: the right outer arm holds the envelope (unless it is waving or holding the sign)
    if (i == ARMS - 1 && !post_.empty() && !waving && hold <= 0 && i != grab_arm_) {
      float dx = LETTER_X - 14 - ARM_X[i], dy = LETTER_Y + 14 - ARM_Y;
      w = atan2f(dy, dx) * 57.2958f;
      curve = 0;
      wa = 2.0f;
      seg = sqrtf(dx * dx + dy * dy) / SEGMENTS;
    }
    // post waiting: the left outer arm points at Yellow himself ("tap me")
    if (i == 0 && shows) {  // from the root up to the left cheek, the tip points at Yellow
      float dx = 50 - ARM_X[i], dy = 74 - ARM_Y;
      w = atan2f(dy, dx) * 57.2958f - 50;
      curve = 20.0f;
      wa = 3.0f;
      seg = sqrtf(dx * dx + dy * dy) * 1.3f / SEGMENTS;
    }
    // grab: the arm points to the finger and stretches (at most 130 px)
    if (i == grab_arm_) {
      float dx = grab_x_ - ARM_X[i], dy = grab_y_ - ARM_Y;
      w = atan2f(dy, dx) * 57.2958f;
      curve = 0;
      wa = 2.0f;
      seg = std::max(seg, std::min(sqrtf(dx * dx + dy * dy), 130.0f) / SEGMENTS);
    }
    float x = ARM_X[i], y = ARM_Y;
    arm_pkt_[i][0].x = x + ARM_DX;
    arm_pkt_[i][0].y = y + ARM_DY;
    under_pts_[i][0].x = x + ARM_DX;
    under_pts_[i][0].y = y + 3 + ARM_DY;
    // keep the points in the visible area (arms end at the edge instead of beyond it)
    float x_min = 10 - plan_->kx, x_max = 310 - plan_->kx, y_min = 26 - plan_->ky, y_max = 216 - plan_->ky;
    for (int k = 1; k <= SEGMENTS; k++) {
      float a = (w + curve * k + wa * sinf(phase_ + k * 0.9f + i * 0.7f) * k / SEGMENTS) * 0.017453f;
      x = std::max(x_min, std::min(x_max, x + seg * cosf(a)));
      y = std::max(y_min, std::min(y_max, y + seg * sinf(a)));
      arm_pkt_[i][k].x = lroundf(x) + ARM_DX;
      arm_pkt_[i][k].y = lroundf(y) + ARM_DY;
      under_pts_[i][k].x = lroundf(x) + ARM_DX;
      under_pts_[i][k].y = lroundf(y + 3) + ARM_DY;
    }
    lv_line_set_points(underside_[i], under_pts_[i], SEGMENTS + 1);
    lv_line_set_points(tentacle_[i], arm_pkt_[i], SEGMENTS + 1);
    for (int k = 0; k < 3; k++) {  // suckers on segments 2–4, slightly towards the underside
      lv_obj_t *s = sucker_[i][k];
      if (s) lv_obj_set_pos(s, (int) arm_pkt_[i][k + 2].x - ARM_DX - 2, (int) arm_pkt_[i][k + 2].y - ARM_DY + 1);
    }
  }
}

inline void Companion::eyes_layout() {
  if (body_ == nullptr) return;
  for (int i = 0; i < 2; i++) {
    int h = std::max(3, eyes_h_ * (100 - blink_) / 100);
    lv_obj_set_size(eye_[i], eyes_w_, h);
    lv_obj_set_pos(eye_[i], plan_->eye_x[i] - eyes_w_ / 2, plan_->eye_y - h / 2);
    show(shine_[i], h >= 14);
  }
}

inline void Companion::body_layout() {
  lv_obj_set_pos(body_, plan_->kx + wobble_ + visit_x_, plan_->ky - hop_ - breath_ + mouse_y_);
  int w = std::max(60, plan_->shadow_w - hop_ * 2);
  lv_obj_set_size(shadow_, w, 12);
  lv_obj_set_pos(shadow_, 160 - w / 2 + wobble_ + visit_x_, plan_->shadow_y);
}

inline void Companion::arms_layout() {
  if (kind_ == OKTI) {  // Okti: the sign arms move with arm_pos_
    draw_tentacles();
    return;
  }
  if (kind_ == ROBOT) {
    robot_arms();
    return;
  }
  if (arm_[0] == nullptr) return;  // fox: 0 = paws in front, 100 = holding the sign up
  int a = arm_pos_;
  uint32_t now_ms = esphome::millis();
  bool waving = wave_until_ && !passed(now_ms, wave_until_);
  bool wiggle = !passed(now_ms, wiggle_until_) || petting_active_;
  int gx, gy;
  for (int i = 0; i < 2; i++) {
    const int *u = plan_->arm_down[i], *o = plan_->arm_up[i];
    int x = u[0] + (o[0] - u[0]) * a / 100, y = u[1] + (o[1] - u[1]) * a / 100;
    if (a <= 0) {
      if (waving && i == 1) {  // right paw waves next to the head
        x = 156 + (int) (6 * sinf(phase_ * 4));
        y = 74 + (int) (8 * sinf(phase_ * 6));
      } else if (i != grab_arm_ && grip(i, gx, gy)) {  // the paw holds the prop
        x = gx - 13;
        y = gy - 9;
      } else if (i == grab_arm_) {  // the paw paws towards the finger, at most 40 px
        float dx = grab_x_ - (x + 11), dy = grab_y_ - (y + 7), d = sqrtf(dx * dx + dy * dy);
        float k = d > 40 ? 40 / d : 1;
        x += (int) (dx * k);
        y += (int) (dy * k);
      } else if (wiggle) {
        y += (int) (4 * sinf(phase_ * 3 + i));
      }
    }
    lv_obj_set_pos(arm_[i], x, y);
  }
}

}  // namespace yellow
