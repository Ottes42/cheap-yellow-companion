#pragma once
// Yellow: Special Moves und große Effekte (Tinte, Mäuselsprung, Glitch, Matrix, Faustgruß …) (Methoden von Figur, siehe yellow.h)
#include "yellow.h"

namespace yellow {

inline void Figur::tinte() {
  if (!sichtbar_) return;
  uint32_t jetzt = esphome::millis();
  if (tinte_[0] == nullptr) {
    for (int i = 0; i < 6; i++) {
      tinte_[i] = form(root_, 0, 0, 10, 10, 0x22182E);
      rand(tinte_[i], 3, 0x3E2E55);
    }
    for (auto *&s : spritzer_) s = form(root_, 0, 0, 9, 9, 0x2E2240);
  }
  zeitweise(UEBERRASCHT, 600);
  anim(cb_hop, 0, 20, 150, 200, 1, lv_anim_path_ease_out);
  zappeln_bis_ = jetzt + 600;
  for (auto *k : tinte_) {
    lv_obj_move_foreground(k);
    lv_obj_set_style_opa(k, LV_OPA_COVER, 0);
    zeigen(k, true);
  }
  for (auto *s : spritzer_) {
    lv_obj_move_foreground(s);
    lv_obj_set_style_opa(s, LV_OPA_COVER, 0);
    zeigen(s, true);
  }
  lv_obj_move_foreground(schild_);  // Schild bleibt vor der Wolke
  tinte_start_ = jetzt;
  tinte_tick(jetzt);
}

inline void Figur::tinte_tick(uint32_t jetzt) {
  if (!tinte_start_) return;
  static const int KREIS[6][3] = {{160, 110, 120}, {120, 90, 90}, {200, 95, 95}, {140, 150, 100}, {185, 150, 95}, {160, 60, 80}};
  uint32_t t = jetzt - tinte_start_;
  for (int i = 0; i < 8; i++) {  // Spritzer fliegen in alle Richtungen und fallen
    float w = i * 0.785f + 0.3f, r = std::min(1.0f, t / 700.0f) * (70 + (i % 3) * 18);
    lv_obj_set_pos(spritzer_[i], 156 + (int) (r * cosf(w)), 108 + (int) (r * sinf(w)) + (int) (t * t / 30000));
    lv_obj_set_style_opa(spritzer_[i], (lv_opa_t) std::max(0, 255 - (int) t * 255 / 1200), 0);
  }
  if (t >= 2600) {
    tinte_start_ = 0;
    for (auto *k : tinte_) zeigen(k, false);
    for (auto *s : spritzer_) zeigen(s, false);
    if (tinte_weg_) {  // z. B. zwischendurch versteckt gewesen: Okti auf jeden Fall zurückholen
      tinte_weg_ = false;
      zeigen(koerper_, true);
      zeigen(schatten_, true);
    }
    return;
  }
  float wachs = std::min(1.0f, t / 450.0f);
  int auf = t > 700 ? (int) (t - 700) / 60 : 0;  // Wolke steigt beim Verblassen etwas auf
  lv_opa_t opa = t < 900 ? LV_OPA_COVER : (lv_opa_t) std::max(0, 255 - (int) (t - 900) * 255 / 1700);
  for (int i = 0; i < 6; i++) {
    int d = std::max(10, (int) (KREIS[i][2] * wachs));
    lv_obj_set_size(tinte_[i], d, d);
    lv_obj_set_pos(tinte_[i], KREIS[i][0] - d / 2, KREIS[i][1] - d / 2 - auf);
    lv_obj_set_style_opa(tinte_[i], opa, 0);
  }
  bool weg = t >= 450 && t < 1900;  // Okti ist in der Wolke verschwunden
  if (weg != tinte_weg_) {
    tinte_weg_ = weg;
    zeigen(koerper_, !weg);
    zeigen(schatten_, !weg);
    if (!weg) {  // wieder da: verlegen
      zeitweise(FROH, 2500);
      roet_bis_ = jetzt + 3500;
      anim(cb_hop, 0, 8, 140, 140, 1, lv_anim_path_ease_out);
    }
  }
}

inline void Figur::faustgruss() {
  if (!sichtbar_ || schild_bis_ || faust_bis_) return;
  uint32_t jetzt = esphome::millis();
  if (faust_ == nullptr) {
    faust_arm_ = form(root_, 0, 0, 10, 10, 0, 8);  // Unterarm von unten zur Faust
    faust_ = form(root_, 0, 0, 10, 10, 0);
    for (int i = 0; i < 3; i++) knoechel_[i] = form(faust_, 0, 0, 2, 2, 0);  // Fingerfugen
    for (auto *&f : funken_) f = form(root_, 0, 0, 7, 7, 0xFFF3A0);
    for (auto *f : funken_) zeigen(f, false);
  }
  uint32_t farbe = art_ == OKTI ? GELB : art_ == FUCHS ? FUCHS_ORANGE : METALL;
  uint32_t rand_f = art_ == OKTI ? GELB_RAND : art_ == FUCHS ? FUCHS_BRAUN : 0x5A6274;
  lv_obj_set_style_bg_color(faust_, lv_color_hex(farbe), 0);
  rand(faust_, 3, rand_f);
  lv_obj_set_style_bg_color(faust_arm_, lv_color_hex(farbe), 0);
  rand(faust_arm_, 3, rand_f);
  lv_obj_move_foreground(faust_arm_);
  zeigen(faust_arm_, true);
  for (auto *k : knoechel_) lv_obj_set_style_bg_color(k, lv_color_hex(rand_f), 0);
  lv_obj_move_foreground(faust_);
  zeigen(faust_, true);
  faust_start_ = jetzt;
  faust_bis_ = jetzt + 8000;
  zeitweise(FROH, 2000);
  anim(cb_blick_x, blick_x_, 0, 200);
  anim(cb_blick_y, blick_y_, 4, 200);
  naechster_blick_ = jetzt + 8000;
  zeige_schild("Faust?", 8);
}

inline void Figur::faust_treffer(uint32_t jetzt) {
  faust_bis_ = 0;
  zahl_[FAUST]++;
  zeigen(faust_, false);
  zeigen(faust_arm_, false);
  funken_start_ = jetzt;
  for (auto *f : funken_) {
    lv_obj_move_foreground(f);
    zeigen(f, true);
  }
  zeitweise(FROH, 2500);
  anim(cb_hop, 0, 14, 140, 180, 1, lv_anim_path_ease_out);
  zappeln_bis_ = jetzt + 1200;
  funkel_bis_ = jetzt + 2000;
  zeige_schild("Bumm!", 2);
  if (led) led(0xFFD23F, 0.6f);
  led_aus_bei_ = jetzt + 600;
}

inline void Figur::faust_tick(uint32_t jetzt) {
  if (faust_bis_) {
    if (vorbei(jetzt, faust_bis_)) {  // keiner hat eingeschlagen
      faust_bis_ = 0;
      zeigen(faust_, false);
      zeigen(faust_arm_, false);
      zeitweise(TRAURIG, 2000);
      nachher_text_ = "Na gut.";
      nachher_bei_ = jetzt + 300;
    } else {  // Faust kommt nach vorn: wächst mit Überschwinger auf 52 px, wippt dann leicht
      uint32_t t = jetzt - faust_start_;
      float k = t < 350 ? t / 350.0f * 1.15f : t < 500 ? 1.15f - (t - 350) / 150.0f * 0.15f : 1.0f;
      int d = std::max(10, (int) (66 * k)) + (t > 500 ? (int) (2 * sinf(t / 180.0f)) : 0);
      lv_obj_set_size(faust_, d, d * 4 / 5);
      lv_obj_set_style_radius(faust_, d / 3, 0);
      lv_obj_set_pos(faust_, 160 - d / 2, FAUST_Y - d * 2 / 5);
      int aw = d * 11 / 20;  // Unterarm: schmaler, reicht bis unter den Rand
      lv_obj_set_size(faust_arm_, aw, 240 - FAUST_Y + 10);
      lv_obj_set_pos(faust_arm_, 160 - aw / 2, FAUST_Y + d / 5);
      for (int i = 0; i < 3; i++) {
        lv_obj_set_size(knoechel_[i], 2, d / 4);
        lv_obj_set_pos(knoechel_[i], d * (i + 1) / 4 - 2, 2);
      }
    }
  }
  if (funken_start_) {  // acht Funken fliegen auseinander
    uint32_t t = jetzt - funken_start_;
    if (t > 600) {
      funken_start_ = 0;
      for (auto *f : funken_) zeigen(f, false);
      return;
    }
    for (int i = 0; i < 8; i++) {
      float w = i * 0.785f, r = 10 + t * 0.11f;
      lv_obj_set_pos(funken_[i], 157 + (int) (r * cosf(w)), FAUST_Y - 3 + (int) (r * sinf(w)));
      lv_obj_set_style_opa(funken_[i], (lv_opa_t) (255 - t * 255 / 600), 0);
    }
  }
}

inline void Figur::spezial() {
  int w = zufall(3);
  if (art_ == OKTI) w == 0 ? tinte() : w == 1 ? knoten() : jonglieren();
  else if (art_ == FUCHS) w == 0 ? maeuselsprung() : w == 1 ? schwanzfaecher() : schwanz_jagen();
  else w == 0 ? glitch() : w == 1 ? visier_auf() : propeller();
}

inline void Figur::jonglieren() {
  if (!sichtbar_ || art_ != OKTI || jongl_start_) return;
  if (ball_[0] == nullptr) {
    static const uint32_t FARBE[3] = {0xFF4A4A, 0x4AA8FF, 0x5AE07A};
    for (int i = 0; i < 3; i++) ball_[i] = form(root_, 0, 0, 16, 16, FARBE[i]);
  }
  for (auto *b : ball_) {
    lv_obj_move_foreground(b);
    zeigen(b, true);
  }
  jongl_start_ = esphome::millis();
  jongl_fallen_ = zufall(3) == 0;
  zeitweise(FROH, 3500);
  zappeln_bis_ = jongl_start_ + 3500;
}

inline void Figur::schwanz_jagen() {
  if (!sichtbar_ || art_ != FUCHS || jagen_start_) return;
  jagen_start_ = esphome::millis();
  zeitweise(UEBERRASCHT, 2800);
  zappeln_bis_ = jagen_start_ + 2800;
}

inline void Figur::propeller() {
  if (!sichtbar_ || art_ != ROBOTER || prop_start_) return;
  if (prop_[0] == nullptr)
    for (auto *&p : prop_) p = glied(koerper_, 5, 0xC8D0DC);
  for (auto *p : prop_) zeigen(p, true);
  prop_start_ = esphome::millis();
  zeitweise(FROH, 3800);
  zeige_schild("Abheben!", 2);
}

inline void Figur::spezial3_tick(uint32_t jetzt) {
  if (jongl_start_) {
    uint32_t t = jetzt - jongl_start_;
    if (t > 3600) {
      jongl_start_ = 0;
      for (auto *b : ball_) zeigen(b, false);
      if (jongl_fallen_) {
        zeitweise(GENERVT, 1500);
        zeige_schild("Hoppla.", 2);
      } else {
        zeige_schild("Ta-da!", 2);
      }
    } else {
      for (int i = 0; i < 3; i++) {  // Bälle auf versetzten Bögen zwischen den äußeren Armen
        float p = fmodf(t / 900.0f + i / 3.0f, 1.0f);
        int x = 90 + (int) (140 * p), y = 70 - (int) (90 * 4 * p * (1 - p));
        if (jongl_fallen_ && i == 2 && t > 2200) {  // dieser fällt runter
          x = 200;
          y = 60 + (int) ((t - 2200) * (t - 2200) / 6000);
        }
        lv_obj_set_pos(ball_[i], x, y);
      }
    }
  }
  if (jagen_start_) {
    uint32_t t = jetzt - jagen_start_;
    if (t > 2800) {
      jagen_start_ = 0;
      besuch_x_ = 0;
      koerper_layout();
      zeitweise(SCHWINDLIG, 2000);
      anim(cb_wackel, 0, 1000, 1500);
      zeige_schild(zufall(2) ? "Fast!" : "Der ist schnell.", 2);
    } else {
      besuch_x_ = (int) (40 * sinf(t / 180.0f));
      koerper_layout();
    }
  }
  if (prop_start_ && prop_[0]) {
    uint32_t t = jetzt - prop_start_;
    if (t > 3800) {
      prop_start_ = 0;
      maus_y_ = 0;
      koerper_layout();
      for (auto *p : prop_) zeigen(p, false);
      anim(cb_hop, 0, 6, 100, 120, 1, lv_anim_path_ease_out);
    } else {
      float a = t / 40.0f;  // Rotorblätter drehen sich (zwei Linien über der Antenne)
      for (int k = 0; k < 2; k++) {
        float w = a + k * 1.5708f, r = 26 * fabsf(cosf(w));
        prop_pkt_[k][0].x = lroundf(90 - r) + ARM_DX;
        prop_pkt_[k][0].y = -26 + ARM_DY;
        prop_pkt_[k][1].x = lroundf(90 + r) + ARM_DX;
        prop_pkt_[k][1].y = -26 + ARM_DY;
        lv_line_set_points(prop_[k], prop_pkt_[k], 2);
      }
      int hoch = t < 800 ? 0 : t < 1600 ? (int) ((t - 800) * 40 / 800) : t < 2800 ? 40 : (int) (40 - (t - 2800) * 40 / 1000);
      maus_y_ = -hoch;
      koerper_layout();
    }
  }
}

inline void Figur::knoten() {
  if (!sichtbar_ || art_ != OKTI) return;
  uint32_t jetzt = esphome::millis();
  knoten_bis_ = jetzt + 3200;
  zeitweise(SCHWINDLIG, 3200);
  nachher_text_ = zufall(2) ? "Moment." : "Welcher Arm war das?";
  nachher_bei_ = jetzt + 600;
  knoten_los_ = jetzt + 3200;
}

inline void Figur::schwanzfaecher() {
  if (!sichtbar_ || art_ != FUCHS) return;
  uint32_t jetzt = esphome::millis();
  faecher_schwanz_bis_ = jetzt + 3500;
  zeitweise(FROH, 3500);
  if (schwanz_) for (auto *l : {schwanz_rand_, schwanz_bauch_, schwanz_, schwanz_spitze_}) if (l) lv_obj_move_foreground(l);
  for (auto *l : spitze_) if (l) lv_obj_move_foreground(l);
  nachher_text_ = "Puh.";
  nachher_bei_ = jetzt + 3600;
}

inline void Figur::visier_auf() {
  if (!sichtbar_ || art_ != ROBOTER || visier_ == nullptr || klappe_start_) return;
  klappe_start_ = esphome::millis();
  zeitweise(UEBERRASCHT, 600);
}

inline void Figur::spezial2_tick(uint32_t jetzt) {
  if (knoten_los_ && vorbei(jetzt, knoten_los_)) {  // losgeschüttelt
    knoten_los_ = 0;
    anim(cb_wackel, 0, 1000, 900);
    zappeln_bis_ = jetzt + 900;
    zeitweise(GENERVT, 1500);
  }
  if (faecher_schwanz_bis_ && vorbei(jetzt, faecher_schwanz_bis_)) {
    faecher_schwanz_bis_ = 0;
    figur_bauen();  // Schwanz wieder hinter den Körper
  }
  if (klappe_start_ && visier_) {
    uint32_t t = jetzt - klappe_start_;
    int y = t < 500 ? 13 - (int) (t * 90 / 500) : t < 3500 ? -77 : t < 4000 ? -77 + (int) ((t - 3500) * 90 / 500) : 13;
    lv_obj_set_y(visier_, y);
    for (int k = 0; k < 2; k++)  // Zahnräder „drehen“: Nabe wandert im Kreis
      if (zahnrad_[k]) {
        lv_obj_t *nabe = lv_obj_get_child(zahnrad_[k], 0);
        float a = (k ? -1 : 1) * t / 200.0f;
        int r = k ? 6 : 9, m = k ? 13 : 18;
        if (nabe) lv_obj_set_pos(nabe, m - 4 + (int) (r * cosf(a)), m - 4 + (int) (r * sinf(a)));
      }
    if (t >= 600 && t < 650) zeige_schild(zufall(2) ? "Wartung." : "Kurz reinschauen.", 3);
    if (t >= 4000) {
      klappe_start_ = 0;
      lv_obj_set_y(visier_, 13);
      zeitweise(FROH, 1500);
    }
  }
}

inline void Figur::maeuselsprung() {
  if (!sichtbar_ || art_ != FUCHS || maus_start_) return;
  if (huegel_ == nullptr) {
    huegel_ = form(root_, 70, 196, 180, 100, 0x6B4A2E, 50);
    form(huegel_, 40, 6, 100, 10, 0x8A6440, 5);  // hellere Kuppe
    zeigen(huegel_, false);
  }
  lv_obj_move_foreground(huegel_);
  lv_obj_move_foreground(schild_);
  zeigen(huegel_, true);
  zeitweise(UEBERRASCHT, 700);
  naechster_blick_ = esphome::millis() + 3000;
  anim(cb_blick_y, blick_y_, 8, 200);
  maus_start_ = esphome::millis();
}

inline void Figur::maus_tick(uint32_t jetzt) {
  if (!maus_start_) return;
  uint32_t t = jetzt - maus_start_;
  int y;
  if (t < 350) y = -(int) (60 * sinf(t / 350.0f * 1.5708f));                          // Absprung
  else if (t < 700) y = -60 + (int) (190 * ((t - 350) / 350.0f) * ((t - 350) / 350.0f));  // Kopfsprung
  else if (t < 2300) y = 130;                                                          // steckt drin
  else if (t < 2700) y = 130 - (int) (140 * (t - 2300) / 400.0f);                      // kommt raus
  else if (t < 2900) y = -10 + (int) (10 * (t - 2700) / 200.0f);
  else y = 0;
  maus_y_ = y;
  if (t >= 700 && t < 2300) lv_obj_set_x(huegel_, 70 + (int) (3 * sinf(t / 60.0f)));  // es wühlt
  koerper_layout();
  zeigen(schatten_, t < 350 || t >= 2700);
  if (t >= 2300 && !maus_raus_) {
    maus_raus_ = true;
    zeitweise(zufall(3) ? GENERVT : FROH, 2500);
    static const char *const fang[] = {"Maus? Nein.", "Nix.", "Erde. Lecker.", "Fast!"};
    nachher_text_ = fang[zufall(4)];
    nachher_bei_ = jetzt + 600;
  }
  if (t >= 3600) {
    maus_start_ = 0;
    maus_raus_ = false;
    maus_y_ = 0;
    zeigen(huegel_, false);
    zeigen(schatten_, true);
    koerper_layout();
  }
}

inline void Figur::glitch() {
  if (!sichtbar_ || art_ != ROBOTER || visier_ == nullptr || glitch_start_) return;
  glitch_start_ = esphome::millis();
  led_blitz(0x7FE8FF, 1600);
}

inline void Figur::glitch_tick(uint32_t jetzt) {
  if (!glitch_start_ || visier_ == nullptr) return;
  uint32_t t = jetzt - glitch_start_;
  if (t >= 1800) {
    glitch_start_ = 0;
    lv_obj_set_style_bg_color(visier_, lv_color_hex(VISIER), 0);
    farb_st_ = -1;  // Stimmungsfarbe gleich wieder setzen
    for (auto *s : scan_) zeigen(s, false);
    zeigen(gesicht_, true);
    gesicht_layout();
    zeitweise(UEBERRASCHT, 1200);
    zeige_schild(zufall(2) ? "Neustart… fertig." : "Das war nichts.", 3);
    return;
  }
  static const uint32_t FARBEN[4] = {VISIER, 0x3A1E40, 0x1E4038, 0x2A3550};
  lv_obj_set_style_bg_color(visier_, lv_color_hex(FARBEN[zufall(4)]), 0);
  for (auto *s : scan_) {
    lv_obj_set_y(s, (int) zufall(80));
    zeigen(s, zufall(3) != 0);
  }
  bool schwarz = t > 1100 && t < 1450;  // kurz Bild weg
  zeigen(gesicht_, !schwarz);
  if (!schwarz) lv_obj_set_pos(gesicht_, blick_x_ + (int) zufall(17) - 8, blick_y_ + (int) zufall(5) - 2);
}

inline void Figur::matrix(int sekunden) {
  if (root_ == nullptr) return;
  if (matrix_[0] == nullptr) {
    for (int i = 0; i < 14; i++) {
      matrix_[i] = lv_label_create(root_);
      lv_obj_set_style_text_color(matrix_[i], lv_color_hex(i % 3 ? 0x2E9E5A : 0x7AF09A), 0);
      lv_label_set_text(matrix_[i], "1\n0\n1");
      matrix_y_[i] = -(float) zufall(240);
      lv_obj_move_to_index(matrix_[i], 1);  // hinter Deko und Figur
      zeigen(matrix_[i], false);
    }
  }
  matrix_bis_ = sekunden > 0 ? esphome::millis() + sekunden * 1000 : 0;
  for (auto *m : matrix_) zeigen(m, matrix_bis_ != 0);
}

inline void Figur::matrix_tick(uint32_t jetzt) {
  if (!matrix_bis_) return;
  if (vorbei(jetzt, matrix_bis_)) {
    matrix(0);
    return;
  }
  if (jetzt - letzte_matrix_ < 80) return;
  letzte_matrix_ = jetzt;
  for (int i = 0; i < 14; i++) {
    matrix_y_[i] += 4 + (i % 4);
    if (matrix_y_[i] > 240) {
      matrix_y_[i] = -60;
      char s[8] = {(char) ('0' + zufall(2)), '\n', (char) ('0' + zufall(2)), '\n', (char) ('0' + zufall(2)), 0};
      lv_label_set_text(matrix_[i], s);
    }
    lv_obj_set_pos(matrix_[i], 6 + i * 22, (int) matrix_y_[i]);
  }
}

inline void Figur::schlafen(bool an) {
  if (an == schlaeft_) return;
  schlaeft_ = an;
  if (root_ == nullptr) return;
  if (an) {
    schlaf_szene();
    return;
  }
  zeigen(bett_, false);
  welk_bis_ = 0;
  if (!maus_start_) maus_y_ = 0;
  basis_ = NORMAL;
  lw_stufe_ = 0;
  letzte_beruehrung_ = esphome::millis();
  figur_bauen();  // stellt Reihenfolge, Gesicht und Visier wieder her
  if (sichtbar_) {
    zeitweise(GAEHNEN, 2200);
    nachher_text_ = art_ == ROBOTER ? "System hochgefahren." : art_ == FUCHS ? "Bin wach. Ganz wach." : "Schon morgen?";
    nachher_bei_ = esphome::millis() + 2400;
  }
}

inline void Figur::schlaf_szene() {
  if (!schlaeft_ || koerper_ == nullptr) return;
  uint32_t jetzt = esphome::millis();
  basis_ = SCHLAF;
  ausdruck_bis_ = 0;
  schild_bis_ = schild_bis_ ? jetzt + 10 : 0;
  ausdruck_setzen(SCHLAF);
  welk_bis_ = jetzt + 600000;  // Arme hängen; schlaf_tick verlängert
  if (art_ == OKTI) {
    if (bett_ == nullptr) {
      bett_ = form(root_, 0, 0, 320, 240, 0, 0);
      transparent(bett_);
      form(bett_, 60, 196, 200, 14, 0x8B5A2B, 4);   // Bettgestell
      form(bett_, 66, 206, 10, 22, 0x6B4226, 2);    // Beine
      form(bett_, 244, 206, 10, 22, 0x6B4226, 2);
      lv_obj_t *decke = form(bett_, 70, 146, 180, 58, 0x5A7AE8, 18);
      rand(decke, 3, 0x3A5AC8);
      for (int k = 0; k < 4; k++) form(decke, 18 + k * 40, 8, 18, 18, 0x7FA0FF, 9);  // Tupfen
      lv_obj_t *umschlag = form(bett_, 76, 146, 168, 14, 0xF4F4F4, 7);  // umgeschlagenes Laken
      (void) umschlag;
    }
    zeigen(bett_, true);
    lv_obj_move_foreground(bett_);
    lv_obj_move_foreground(schild_);
  } else if (art_ == ROBOTER && visier_) {
    zeigen(gesicht_, false);
    zeigen(zzz_, false);
    lv_obj_set_style_bg_color(visier_, lv_color_hex(0x07090E), 0);
    if (standby_ == nullptr) standby_ = form(visier_, 62, 34, 16, 16, 0xFF8A1A);
    zeigen(standby_, true);
    if (antenne_) lv_obj_set_style_bg_color(antenne_, lv_color_hex(0x3A3F4A), 0);
  } else if (art_ == FUCHS) {
    if (!maus_start_) maus_y_ = 22;  // sitzt tiefer, eingerollt
    if (schwanz_) {  // Schwanz nach vorn
      for (auto *l : {schwanz_rand_, schwanz_bauch_, schwanz_, schwanz_spitze_}) if (l) lv_obj_move_foreground(l);
      for (auto *l : spitze_) if (l) lv_obj_move_foreground(l);
    }
    koerper_layout();
  }
}

inline void Figur::schlaf_tick(uint32_t jetzt) {
  welk_bis_ = jetzt + 600000;
  if (standby_) lv_obj_set_style_bg_opa(standby_, (lv_opa_t) (110 + 145 * (0.5f + 0.5f * sinf(jetzt / 900.0f))), 0);
  if (art_ == ROBOTER && herz_) lv_obj_set_style_bg_opa(herz_, 40, 0);
}

inline void Figur::licht_schreck() {
  if (schlaeft_ || verreist_) return;
  schreck_bei_ = esphome::millis() + (art_ == FUCHS ? 2300 : 600);  // Fuchs läuft erst herein
}

inline void Figur::schreck_tick(uint32_t jetzt) {
  if (!vorbei(jetzt, schreck_bei_)) return;
  schreck_bei_ = 0;
  if (!sichtbar_) return;
  led_blitz(0xFFFFFF, 300);
  if (art_ == FUCHS) {  // mutig: nur kurz blinzeln
    anim(cb_blinzel, 0, 80, 90, 250, 1);
    zeigen_spaeter("Ich war wach. Ehrlich.", 900);
    return;
  }
  zeitweise(UEBERRASCHT, 1300);
  anim(cb_hop, 0, 18, 120, 220, 1, lv_anim_path_ease_out);
  anim(cb_blinzel, 0, 90, 80, 600, 1);  // Augen zukneifen
  zappeln_bis_ = jetzt + 900;
  zeigen_spaeter(art_ == ROBOTER ? "Helligkeit +800 %." : (zufall(2) ? "Licht! Augen!" : "Wer hat das angemacht?"), 1300);
}

}  // namespace yellow
