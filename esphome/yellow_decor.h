#pragma once
// Yellow: decoration (occasions, seasons), weather view, hat (methods of Companion, see yellow.h)
#include "yellow.h"

namespace yellow {

inline void Companion::season(int month, int day) {
  month_ = month;
  day_ = day;
  apply_decor();
}

inline void Companion::decor(int mode) {
  decor_mode_ = mode;
  apply_decor();
}

inline void Companion::occasion(const std::string &a) {
  size_t t = a.find('|');
  occasion_ = a.substr(0, t);
  occasion_count_ = t == std::string::npos ? 0 : atoi(a.c_str() + t + 1);  // advent|2 = second Sunday of Advent
  apply_decor();
}

inline void Companion::hat() {
  show(pumpkin_, halloween_);
  show(stem_, halloween_);
  show(cap_, christmas_);
  bool g = decor_set_ != nullptr && decor_set_->hat[0];
  bool acc = !g && !halloween_ && !christmas_ && accessory_[0];  // the occasion hat comes first
  show(hat_glyph_, g || acc);
  if (hat_glyph_ && (g || acc)) {
    lv_label_set_text(hat_glyph_, g ? decor_set_->hat : accessory_);
    uint32_t hf = g ? decor_set_->hat_color : accessory_color_;
    if (kind_ == OKTI && (hf == 0xFFD23F || hf == 0xFFE08A)) hf = 0xD98E00;  // yellow on Okti's yellow head: darker gold
    lv_obj_set_style_text_color(hat_glyph_, lv_color_hex(hf), 0);
  }
}

inline void Companion::apply_decor() {
  // key: test > select (Off/Halloween/Christmas) > occasion from HA > date (if HA delivers nothing)
  std::string k;
  if (!decor_test_.empty()) k = decor_test_;
  else if (decor_mode_ == 1) k = "";
  else if (decor_mode_ == 2) k = "halloween";
  else if (decor_mode_ == 3) k = "christmas";
  else if (!occasion_.empty()) k = occasion_;
  else if ((month_ == 10 && day_ >= 17) || (month_ == 11 && day_ == 1)) k = "halloween";
  else if (month_ == 12 && day_ <= 26) k = "christmas";
  halloween_ = k == "halloween";
  christmas_ = k == "christmas" || k == "st_nicholas";
  show(decor_hat_, halloween_);
  show(decor_w_, k == "christmas");
  decor_set_ = nullptr;
  for (auto &d : DECO_SETS)
    if (k == d.key) decor_set_ = &d;
  for (int i = 0; i < 8; i++) {
    if (decor_g_[i] == nullptr) continue;
    bool on = decor_set_ != nullptr && decor_set_->glyph[i][0];
    show(decor_g_[i], on);
    lv_obj_set_pos(decor_g_[i], DECO_POS[i][0], DECO_POS[i][1]);
    lv_obj_set_style_text_opa(decor_g_[i], LV_OPA_COVER, 0);
    lv_obj_set_style_transform_rotation(decor_g_[i], 0, 0);
    if (on) {
      lv_label_set_text(decor_g_[i], decor_set_->glyph[i]);
      lv_obj_set_style_text_color(decor_g_[i], lv_color_hex(decor_set_->color[i]), 0);
    }
  }
  // Advent: only as many candles burn as Sundays of Advent have passed (slots 0, 2, 3, 5 are candles)
  if (k == "advent" && occasion_count_ > 0) {
    static const int CANDLE[4] = {0, 2, 3, 5};
    for (int n = 0; n < 4; n++)
      if (decor_g_[CANDLE[n]]) lv_obj_set_style_text_color(decor_g_[CANDLE[n]], lv_color_hex(n < occasion_count_ ? 0xFFE08A : 0x5A5A66), 0);
  }
  bool pink = k == "valentine";
  if (pink != pink_) {
    pink_ = pink;
    color_st_ = -1;  // set the color again
    if (body_) apply_expression(expression_);
  }
  decor_key_ = k;
  hat();
}

inline void Companion::build_decor() {
  // --- Halloween: moon, bats, pumpkins with faces ---
  decor_hat_ = shape(root_, 0, 0, 320, 240, 0, 0);
  lv_obj_set_style_bg_opa(decor_hat_, LV_OPA_TRANSP, 0);
  lv_obj_t *moon = shape(decor_hat_, 14, 14, 34, 34, 0xF5E6A8);
  shape(moon, 18, -4, 30, 30, BACKGROUND);  // crescent
  static const int BAT[3][2] = {{70, 40}, {238, 62}, {200, 26}};
  for (int i = 0; i < 3; i++) {
    shape(decor_hat_, BAT[i][0] - 3, BAT[i][1] - 3, 7, 7, 0x7A6AA8);
    wing_[i] = line(decor_hat_, 3, 0x7A6AA8);
    bat_pos_[i][0] = BAT[i][0];
    bat_pos_[i][1] = BAT[i][1];
  }
  set_bats(0);
  static const int PUMPKIN[2][3] = {{10, 196, 40}, {272, 202, 34}};  // x, y, width
  for (auto &k : PUMPKIN) {
    int w = k[2], h = w * 3 / 4;
    shape(decor_hat_, k[0] + w / 2 - 3, k[1] - 7, 6, 9, 0x3D7A2A, 2);
    lv_obj_t *kb = shape(decor_hat_, k[0], k[1], w, h, 0xF28C28);
    border(kb, 2, 0xA84E10);
    shape(kb, w / 4 - 3, h / 3 - 2, 7, 6, 0x2A1A0A, 1);
    shape(kb, w * 3 / 4 - 5, h / 3 - 2, 7, 6, 0x2A1A0A, 1);
    shape(kb, w / 2 - 8, h * 2 / 3 - 3, 15, 4, 0x2A1A0A, 2);
  }

  // --- Christmas: fir tree, fairy lights, present, snow ---
  decor_w_ = shape(root_, 0, 0, 320, 240, 0, 0);
  lv_obj_set_style_bg_opa(decor_w_, LV_OPA_TRANSP, 0);
  shape(decor_w_, 31, 218, 10, 16, 0x6B4226, 2);                 // trunk
  shape(decor_w_, 6, 190, 60, 30, 0x1F7A3A, 14);                  // tree bottom
  shape(decor_w_, 13, 164, 46, 30, 0x248A42, 14);                 // middle
  shape(decor_w_, 21, 140, 30, 28, 0x2A9A4A, 13);                 // top
  shape(decor_w_, 29, 128, 14, 14, 0xFFD23F);                     // star
  static const int BAUBLE[6][2] = {{18, 202}, {46, 206}, {30, 180}, {48, 176}, {26, 154}, {40, 196}};
  for (int i = 0; i < 6; i++) bauble_[i] = shape(decor_w_, BAUBLE[i][0], BAUBLE[i][1], 7, 7, RED);
  // fairy lights at the top (sagging), the clock top right stays free
  for (int i = 0; i < 9; i++) {
    float t = i / 8.0f;
    chain_pts_[i].x = 4 + t * 236;
    chain_pts_[i].y = 6 + 22 * 4 * t * (1 - t);
  }
  lv_obj_t *wire = line(decor_w_, 2, 0x1E3A24);
  lv_line_set_points(wire, chain_pts_, 9);
  for (int i = 0; i < 12; i++) {
    float t = (i + 0.5f) / 12.0f;
    int x = 4 + (int) (t * 236), y = 6 + (int) (22 * 4 * t * (1 - t));
    bulb_[i] = shape(decor_w_, x - 3, y + 1, 7, 9, 0xFFD23F);
  }
  // present bottom right
  lv_obj_t *gs = shape(decor_w_, 270, 204, 34, 28, RED, 3);
  shape(gs, 15, 0, 4, 28, 0xFFD23F, 0);
  shape(gs, 0, 11, 34, 4, 0xFFD23F, 0);
  // snow
  for (int i = 0; i < 14; i++) {
    flake_x_[i] = (float) rnd(320);
    flake_y_[i] = (float) rnd(240);
    flake_[i] = shape(decor_w_, (int) flake_x_[i], (int) flake_y_[i], 3, 3, 0xE8F0FF);
  }
  show(decor_hat_, false);
  show(decor_w_, false);
  for (int i = 0; i < 8; i++) {
    decor_g_[i] = lv_label_create(root_);
    if (decor_font_) lv_obj_set_style_text_font(decor_g_[i], decor_font_, 0);
    lv_obj_set_pos(decor_g_[i], DECO_POS[i][0], DECO_POS[i][1]);
    lv_label_set_text(decor_g_[i], "");
    show(decor_g_[i], false);
  }
  build_weather();
}

inline void Companion::build_weather() {
  weather_ = shape(root_, 0, 0, 320, 240, 0, 0);
  lv_obj_set_style_bg_opa(weather_, LV_OPA_TRANSP, 0);
  sun_ = shape(weather_, 0, 0, 320, 240, 0, 0);
  lv_obj_set_style_bg_opa(sun_, LV_OPA_TRANSP, 0);
  for (int i = 0; i < 8; i++) ray_[i] = line(sun_, 3, 0xFFD23F);
  shape(sun_, SUN_X - 15, SUN_Y - 15, 30, 30, 0xFFD23F);
  for (int i = 0; i < 3; i++) {  // fog bands
    fog_[i] = shape(weather_, -20, 120 + i * 34, 360, 16, 0x9AA4B8, 8);
    lv_obj_set_style_bg_opa(fog_[i], 70, 0);
  }
  for (int i = 0; i < 2; i++) {  // clouds from three circles and a flat bottom
    cloud_[i] = shape(weather_, 0, 0, 90, 46, 0, 0);
    lv_obj_set_style_bg_opa(cloud_[i], LV_OPA_TRANSP, 0);
    cloud_part_[i][0] = shape(cloud_[i], 0, 20, 90, 26, 0, 13);
    cloud_part_[i][1] = shape(cloud_[i], 12, 6, 34, 34, 0);
    cloud_part_[i][2] = shape(cloud_[i], 36, 0, 42, 42, 0);
    cloud_x_[i] = i ? 150.0f : -10.0f;
  }
  for (int i = 0; i < PARTICLES; i++) {
    drop_x_[i] = (float) rnd(340);
    drop_y_[i] = (float) rnd(240);
    drop_[i] = shape(weather_, 0, 0, 2, 9, 0x7FB8FF, 1);
  }
  flash_ = line(weather_, 4, 0xFFF3A0);
  static const int FLASH[5][2] = {{70, 34}, {58, 70}, {74, 72}, {60, 112}, {82, 66}};
  for (int i = 0; i < 5; i++) {
    flash_pts_[i].x = FLASH[i][0];
    flash_pts_[i].y = FLASH[i][1];
  }
  lv_line_set_points(flash_, flash_pts_, 5);
  show(flash_, false);
  apply_weather();  // the condition may have come from HA before building
}

inline void Companion::apply_weather() {
  if (weather_ == nullptr) return;
  const std::string &l = weather_test_.empty() ? weather_ha_ : weather_test_;
  auto hat = [&l](const char *s) { return l.find(s) != std::string::npos; };
  // weather mood: rain/thunderstorms make him a bit grumpier (color towards red, mouth corners), sun happier
  weather_mood_ = hat("lightning") || l == "pouring" ? 18 : l == "rainy" || l == "hail" || l == "snowy-rainy" ? 12
                  : l == "fog" ? 6 : l == "cloudy" ? 4 : l == "sunny" ? -10 : 0;
  if (body_ && !expression_until_) apply_expression(expression_);
  // weather on the face: on a change Yellow looks up and reacts briefly
  if (visible_ && !weather_old_.empty() && l != weather_old_ && !sign_until_) {
    look("up");
    if (hat("lightning") && kind_ == FOX) show_sign(txt::THUNDER, 3);
    else if (hat("lightning")) gesture("startle");
    else if (hat("snowy")) {
      for_a_while(HAPPY, 3000);
      show_sign(txt::SNOW, 3);
    } else if (l == "pouring" || l == "rainy") show_sign(l == "pouring" ? txt::UH_OH : txt::RAIN, 3);
    else if (l == "sunny") {
      for_a_while(HAPPY, 2500);
      show_sign(txt::SUN, 3);
    } else if (hat("windy")) {  // character: Okti gets shaken, fox leans into it, robot measures
      if (kind_ == OKTI) {
        anim(cb_wobble, 0, 1000, 1500);
        show_sign(txt::WHEEE, 3);
      } else if (kind_ == FOX) {
        for_a_while(HAPPY, 2500);
        show_sign(txt::FRESH_AIR, 3);
      } else {
        show_sign(esphome::str_sprintf(txt::WIND_ROBOT, 30 + (int) rnd(25)), 3);
      }
    }
  }
  weather_old_ = l;
  bool flash = hat("lightning"), snow = hat("snowy"), hail = l == "hail";
  bool rain = l == "rainy" || l == "pouring" || l == "lightning-rainy" || l == "snowy-rainy" || hail;
  bool clouds = rain || snow || flash || l == "cloudy" || l == "partlycloudy";
  bool dark = rain || flash || l == "cloudy";
  weather_wind_ = hat("windy");
  weather_flash_ = flash;
  flash_on_ = 0;
  next_flash_ = esphome::millis() + 2000;
  show(flash_, false);
  lv_obj_set_style_bg_color(root_, lv_color_hex(BACKGROUND), 0);
  show(sun_, l == "sunny" || l == "partlycloudy");
  for (int i = 0; i < 3; i++) show(fog_[i], l == "fog");
  for (int i = 0; i < 2; i++) {
    show(cloud_[i], clouds && (i == 0 || l != "partlycloudy"));
    for (auto *w : cloud_part_[i]) lv_obj_set_style_bg_color(w, lv_color_hex(dark ? 0x5C667A : 0xC8D0DC), 0);
  }
  // particles: 0 off, otherwise count; rain blue strokes, snow white dots, sleet/hail mixed
  int n = l == "pouring" || l == "lightning-rainy" ? PARTICLES : rain || snow ? PARTICLES * 2 / 3 : weather_wind_ ? 6 : 0;
  for (int i = 0; i < PARTICLES; i++) {
    bool flake = snow && (l == "snowy" || i % 2) || hail && i % 2;
    bool leaf = weather_wind_ && !rain && !snow;
    drop_kind_[i] = flake ? 1 : leaf ? 2 : 0;
    lv_obj_set_size(drop_[i], flake ? 4 : leaf ? 7 : 3, flake ? 4 : leaf ? 5 : 12);
    lv_obj_set_style_radius(drop_[i], flake ? 2 : leaf ? 3 : 1, 0);
    lv_obj_set_style_bg_color(drop_[i], lv_color_hex(flake ? 0xE8F0FF : leaf ? 0xC9782A : 0xA8D4FF), 0);
    show(drop_[i], i < n);
  }
  show(weather_, !l.empty());
}

inline void Companion::weather_tick(uint32_t now_ms) {
  if (weather_ == nullptr || lv_obj_has_flag(weather_, LV_OBJ_FLAG_HIDDEN) || now_ms - last_weather_ < 60) return;
  last_weather_ = now_ms;
  float wind = weather_wind_ ? 3.0f : 0.6f;
  for (int i = 0; i < 2; i++) {  // clouds drift (faster with wind), at the top, the back one a bit lower
    cloud_x_[i] += (0.25f + i * 0.12f) * wind;
    if (cloud_x_[i] > 330) cloud_x_[i] = -100;
    lv_obj_set_pos(cloud_[i], (int) cloud_x_[i], 18 + i * 22);
  }
  for (int i = 0; i < 3; i++)
    if (!lv_obj_has_flag(fog_[i], LV_OBJ_FLAG_HIDDEN))
      lv_obj_set_x(fog_[i], -20 + (int) (18 * sinf(now_ms / 2600.0f + i * 2.1f)));
  if (!lv_obj_has_flag(sun_, LV_OBJ_FLAG_HIDDEN)) {  // rays turn slowly and pulse
    float d = now_ms / 3000.0f, r = 22 + 2 * sinf(now_ms / 500.0f);
    for (int i = 0; i < 8; i++) {
      float w = d + i * 0.785f;
      ray_pts_[i][0].x = SUN_X + (int) (r * cosf(w));
      ray_pts_[i][0].y = SUN_Y + (int) (r * sinf(w));
      ray_pts_[i][1].x = SUN_X + (int) ((r + 9) * cosf(w));
      ray_pts_[i][1].y = SUN_Y + (int) ((r + 9) * sinf(w));
      lv_line_set_points(ray_[i], ray_pts_[i], 2);
    }
  }
  for (int i = 0; i < PARTICLES; i++) {
    if (lv_obj_has_flag(drop_[i], LV_OBJ_FLAG_HIDDEN)) continue;
    int kind = drop_kind_[i];
    drop_y_[i] += kind == 1 ? 1.2f + (i % 3) * 0.4f : kind == 2 ? 0.8f * sinf(now_ms / 300.0f + i) : 7.0f + (i % 3);
    drop_x_[i] += kind == 1 ? sinf(now_ms / 900.0f + i) * 0.5f + wind * 0.3f : kind == 2 ? 6.0f + (i % 3) : wind * 0.5f;
    if (drop_y_[i] > 240 || drop_x_[i] > 330) {
      drop_y_[i] = kind == 2 ? (float) (60 + rnd(160)) : -10.0f;
      drop_x_[i] = kind == 2 ? -10.0f : (float) rnd(330) - 10;
    }
    lv_obj_set_pos(drop_[i], (int) drop_x_[i], (int) drop_y_[i]);
  }
  if (weather_flash_) {  // thunderstorm: the background flashes briefly every 6–14 s
    if (!flash_on_ && passed(now_ms, next_flash_)) {
      flash_on_ = now_ms;
      int x = (int) cloud_x_[0] + 20;
      static const int ZIGZAG[5] = {0, -12, 4, -10, 12};
      for (int i = 0; i < 5; i++) flash_pts_[i].x = x + ZIGZAG[i];
      lv_line_set_points(flash_, flash_pts_, 5);
      show(flash_, true);
      lv_obj_set_style_bg_color(root_, lv_color_hex(0x4A5370), 0);
    } else if (flash_on_ && now_ms - flash_on_ > 140) {
      flash_on_ = 0;
      next_flash_ = now_ms + 6000 + rnd(8000);
      show(flash_, false);
      lv_obj_set_style_bg_color(root_, lv_color_hex(BACKGROUND), 0);
    }
  }
}

inline void Companion::decor_tick(uint32_t now_ms) {
  if (decor_key_ == "perseids" && decor_g_[0]) {  // a shooting star crosses the sky diagonally every 6 s
    uint32_t t = now_ms % 6000;
    lv_obj_set_pos(decor_g_[0], t < 2200 ? -30 + (int) (t * 360 / 2200) : -40, t < 2200 ? 6 + (int) (t * 70 / 2200) : -40);
  }
  if ((decor_key_ == "autumn" || decor_key_ == "harvest") && now_ms - last_leaves_ >= 70) {  // leaves fall and sway
    last_leaves_ = now_ms;
    static const int FALL[3] = {0, 3, 7};
    for (int n = 0; n < 3; n++) {
      lv_obj_t *g = decor_g_[FALL[n]];
      if (g == nullptr) continue;
      uint32_t t = (now_ms / 70 + n * 60) % 180;
      int x = DECO_POS[FALL[n]][0] + (int) (14 * sinf(t / 9.0f + n));
      lv_obj_set_pos(g, x, -30 + (int) t * 3 / 2);
    }
  }
  if (decor_key_ == "clock_change" && decor_g_[0]) {  // a clock turns (hour forward or back)
    lv_obj_set_style_transform_pivot_x(decor_g_[0], 15, 0);
    lv_obj_set_style_transform_pivot_y(decor_g_[0], 15, 0);
    lv_obj_set_style_transform_rotation(decor_g_[0], (int32_t) ((now_ms / 4) % 3600) * (month_ == 10 ? -1 : 1), 0);
  }
  if ((decor_key_ == "oktoberfest" || decor_key_ == "beer") && decor_g_[0] && decor_g_[3]) {  // mugs clink every 5 s
    uint32_t t = now_ms % 5000;
    int gone = t < 700 ? (int) (t * 100 / 700) : t < 1100 ? 100 : t < 1800 ? 100 - (int) ((t - 1100) * 100 / 700) : 0;
    lv_obj_set_pos(decor_g_[0], DECO_POS[0][0] + gone * (130 - DECO_POS[0][0]) / 100, DECO_POS[0][1] - gone * 20 / 100);
    lv_obj_set_pos(decor_g_[3], DECO_POS[3][0] - gone * (DECO_POS[3][0] - 162) / 100, DECO_POS[3][1] - gone * 30 / 100);
  }
  if (decor_key_ == "full_moon" && decor_g_[1]) {  // moon glows softly, stars sparkle
    lv_obj_set_style_text_opa(decor_g_[1], (lv_opa_t) (170 + 85 * sinf(now_ms / 900.0f)), 0);
    for (int i : {0, 2, 3, 5, 6, 7})
      if (decor_g_[i]) lv_obj_set_style_text_opa(decor_g_[i], ((now_ms / 400 + i) % 4) ? LV_OPA_COVER : 70, 0);
  }
  if (decor_key_ == "easter" && decor_g_[7]) {  // Easter bunny hops across the bottom of the picture, every 14 s
    uint32_t t = now_ms % 14000;
    int x = -40 + (int) (t * 400 / 14000);
    int y = 200 - (int) (22 * fabsf(sinf(t / 260.0f)));
    lv_obj_set_pos(decor_g_[7], x, y);
  }
  if (decor_key_ == "new_year" && now_ms - last_decor_ >= 450) {  // fireworks: rockets flash offset
    last_decor_ = now_ms;
    decor_beat_++;
    for (int i = 0; i < 8; i++)
      if (decor_g_[i]) lv_obj_set_style_text_opa(decor_g_[i], ((i + decor_beat_) % 3) ? LV_OPA_COVER : 60, 0);
  }
  if (halloween_ && now_ms - last_decor_ >= 300) {
    last_decor_ = now_ms;
    set_bats(++decor_beat_);
  }
  if (christmas_) {
    if (now_ms - last_decor_ >= 500) {
      last_decor_ = now_ms;
      decor_beat_++;
      static const uint32_t COLORS[4] = {0xFF4A4A, 0xFFD23F, 0x4AA8FF, 0x5AE07A};
      for (int i = 0; i < 12; i++)
        lv_obj_set_style_bg_color(bulb_[i], lv_color_hex(COLORS[(i + decor_beat_) % 4]), 0);
      for (int i = 0; i < 6; i++)
        lv_obj_set_style_bg_color(bauble_[i], lv_color_hex(COLORS[(i * 3 + decor_beat_ / 2) % 4]), 0);
    }
    if (now_ms - last_snow_ >= 90) {
      last_snow_ = now_ms;
      for (int i = 0; i < 14; i++) {
        flake_y_[i] += 0.8f + (i % 3) * 0.4f;
        flake_x_[i] += sinf((now_ms / 900.0f) + i) * 0.4f;
        if (flake_y_[i] > 240) {
          flake_y_[i] = -4;
          flake_x_[i] = (float) rnd(320);
        }
        lv_obj_set_pos(flake_[i], (int) flake_x_[i], (int) flake_y_[i]);
      }
    }
  }
}

}  // namespace yellow
