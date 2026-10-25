#pragma once
// Yellow: Schilder, Post und Liebesbriefe, Verreist, Hilfe, Netzstatus (Methoden von Figur, siehe yellow.h)
#include "yellow.h"

namespace yellow {

inline void Figur::zeige_schild(const std::string &text, int sekunden) {
  if (!sichtbar_ || verreist_ || schlaeft_) return;
  letztes_schild_ = text;
  letztes_schild_zeit_ = esphome::millis();
  if (schild_font_ == nullptr) schild_font_ = lv_obj_get_style_text_font(schild_text_, LV_PART_MAIN);
  lv_obj_set_style_text_font(schild_text_, schild_font_, 0);  // nach dem Countdown wieder Handschrift
  if (schild_y_ > SCHILD_DA + 50) {  // lag nach dem Fallenlassen unten: wieder von oben hereinschieben
    schild_y_ = SCHILD_WEG;
    schild_layout();
  }
  if (art_ == ROBOTER) led_blitz(LEUCHT, 150);  // Roboter piept (LED) bei jedem Schild
  patzer_bei_ = 0;
  lv_anim_delete(this, cb_schild_dreh);
  lv_obj_set_style_transform_rotation(schild_, 0, 0);
  lv_label_set_text(schild_text_, text.c_str());
  anim(cb_schild, schild_y_, SCHILD_DA, 380, 0, 1, lv_anim_path_overshoot);
  anim(cb_arm, arm_pos_, 100, 400);
  schild_bis_ = esphome::millis() + std::max(1, sekunden) * 1000;
}

inline void Figur::schild_patzer(const std::string &text) {
  zeige_schild(text, 5);
  if (!sichtbar_) return;
  std::string f = text;
  std::vector<size_t> stellen;  // nur ASCII-Buchstabenpaare, damit kein UTF-8-Zeichen zerreißt
  for (size_t i = 0; i + 1 < f.size(); i++)
    if (isalpha((unsigned char) f[i]) && isalpha((unsigned char) f[i + 1]) && f[i] != f[i + 1]) stellen.push_back(i);
  // Kopfstand braucht eine LVGL-Ebene (Breite × Höhe × 4 Byte am Stück); fehlt der Platz, wartet LVGL endlos →
  // Watchdog (25.10.: 53 KB nötig, 49 KB frei). Dann lieber der Buchstabendreher.
  lv_obj_update_layout(schild_);
  const size_t ebene = (size_t) lv_obj_get_width(schild_) * lv_obj_get_height(schild_) * 4 + 16384;
  const bool kopfstand_passt = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT) > ebene;
  if (!stellen.empty() && (zufall(2) || !kopfstand_passt)) {
    size_t i = stellen[zufall(stellen.size())];
    std::swap(f[i], f[i + 1]);
    lv_label_set_text(schild_text_, f.c_str());
    patzer_art_ = 2;
  } else if (kopfstand_passt) {
    lv_obj_set_style_transform_pivot_x(schild_, lv_obj_get_width(schild_) / 2, 0);
    lv_obj_set_style_transform_pivot_y(schild_, lv_obj_get_height(schild_) / 2, 0);
    lv_obj_set_style_transform_rotation(schild_, 1800, 0);
    patzer_art_ = 1;
  } else {
    return;  // kein Patzer möglich: Schild bleibt einfach richtig
  }
  patzer_bei_ = esphome::millis() + 1900;
}

inline void Figur::patzer_tick(uint32_t jetzt) {
  if (!vorbei(jetzt, patzer_bei_)) return;
  patzer_bei_ = 0;
  zeitweise(GENERVT, 1500);
  if (patzer_art_ == 1)
    anim(cb_schild_dreh, 1800, 0, 500);
  else
    lv_label_set_text(schild_text_, letztes_schild_.c_str());
  anim(cb_hop, 0, 6, 120, 120, 1, lv_anim_path_ease_out);
  schild_bis_ = jetzt + 2500;
  if (art_ == ROBOTER) {
    nachher_text_ = "Fehler korrigiert.";
    nachher_bei_ = jetzt + 2700;
  }
}

inline void Figur::nachricht(const std::string &text, bool warten, int sekunden) {
  if (text.empty()) return;
  if (!warten) {
    if (!sichtbar_) return;
    schilder_planen(text, sekunden);
    return;
  }
  if (post_.size() >= 5) post_.pop_front();
  post_.push_back(text);
  naechster_ruf_ = esphome::millis() + 500;
  brief_layout();
}

inline void Figur::post_abspielen() {
  if (post_.empty()) return;
  std::string t = post_.front();
  post_.pop_front();
  uint32_t jetzt = esphome::millis();
  const std::string original = t;  // mit Präfix an HA, damit die Lesebestätigung „Liebesbrief“ sagen kann
  bool liebe = ist_liebe(t);
  if (liebe) t = t.substr(strlen(LIEBE_PRAEFIX));
  brief_offen_bis_ = jetzt + 1200;
  klappe(true);
  brief_layout();
  if (sichtbar_) {
    zeitweise(FROH, liebe ? 6000 : 2000);
    schilder_planen(t, liebe ? 5 : 4);
    if (liebe) {  // verlegen: rot, Herzen steigen, Fuchs wedelt
      roet_bis_ = jetzt + 8000;
      zappeln_bis_ = jetzt + 2500;
      herzen_start();
      if (led) led(0xFF4A7A, 0.4f);
      led_aus_bei_ = jetzt + 4000;
    }
  }
  naechster_ruf_ = jetzt + 15000;
  if (gelesen) gelesen(original);
}

inline void Figur::post_leeren() {
  post_.clear();
  brief_offen_bis_ = 0;
  brief_layout();
}

inline void Figur::verreist(bool an, const std::string &karte) {
  bool war = verreist_;
  verreist_ = an;
  karte_ = karte;
  if (root_ == nullptr) return;
  if (koffer_ == nullptr) {
    koffer_ = form(root_, 0, 0, 320, 240, 0, 0);
    transparent(koffer_);
    lv_obj_t *griff = form(koffer_, 142, 112, 36, 22, 0, 8);
    transparent(griff);
    rand(griff, 5, 0x6B3A18);
    lv_obj_t *k = form(koffer_, 104, 126, 112, 78, 0x9A5B2E, 12);
    rand(k, 3, 0x6B3A18);
    form(k, 22, 0, 7, 78, 0x6B3A18, 0);  // Riemen
    form(k, 80, 0, 7, 78, 0x6B3A18, 0);
    form(k, 36, 14, 30, 18, GELB, 4);  // Aufkleber
    form(k, 40, 44, 16, 16, 0x4AA8FF);
    form(k, 92, 46, 12, 18, 0xFF5A7A, 3);
    form(koffer_, 118, 204, 14, 8, 0x3A2418, 3);  // Rollen
    form(koffer_, 188, 204, 14, 8, 0x3A2418, 3);
    lv_obj_t *z = form(koffer_, 30, 22, 260, 74, 0xF4E4C1, 4);
    rand(z, 2, 0x8B6B3E);
    zettel_text_ = lv_label_create(z);
    lv_obj_set_width(zettel_text_, 244);
    lv_obj_set_style_text_align(zettel_text_, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(zettel_text_, lv_color_hex(0x3A2A1A), 0);
    lv_label_set_long_mode(zettel_text_, LV_LABEL_LONG_WRAP);
    lv_obj_align(zettel_text_, LV_ALIGN_CENTER, 0, 0);
  }
  lv_label_set_text(zettel_text_, karte.empty() ? "Bin verreist. Bis bald!" : karte.c_str());
  zeigen(koffer_, an);
  zeigen(koerper_, !an);
  zeigen(schatten_, !an);
  if (an) {
    schild_bis_ = schild_bis_ ? esphome::millis() + 10 : 0;  // laufendes Schild wegräumen
  } else if (war && sichtbar_) {  // wieder da, mit Souvenir
    uint32_t jetzt = esphome::millis();
    static const char *const souvenir[] = {"sonnenbrille", "schal", "faecher"};
    requisit(souvenir[zufall(3)], 20);
    zeitweise(FROH, 4000);
    funkel_bis_ = jetzt + 3000;
    anim(cb_hop, 0, 14, 160, 180, 2, lv_anim_path_ease_out);
    zeige_schild("Wieder da!", 4);
  }
}

inline void Figur::herzen_start() {
  if (root_ == nullptr || deko_font_ == nullptr) return;
  if (herz_g_[0] == nullptr)
    for (int i = 0; i < 6; i++) {
      herz_g_[i] = lv_label_create(root_);
      lv_obj_set_style_text_font(herz_g_[i], deko_font_, 0);
      lv_label_set_text(herz_g_[i], "\U000F02D1");
      lv_obj_set_style_text_color(herz_g_[i], lv_color_hex(i % 2 ? 0xFF4A6A : 0xFF8FA3), 0);
      zeigen(herz_g_[i], false);
    }
  for (auto *h : herz_g_) {
    lv_obj_move_foreground(h);
    zeigen(h, true);
  }
  lv_obj_move_foreground(schild_);
  herzen_start_ = esphome::millis();
}

inline void Figur::herzen_tick(uint32_t jetzt) {
  if (!herzen_start_) return;
  uint32_t t = jetzt - herzen_start_;
  if (t > 4000) {
    herzen_start_ = 0;
    for (auto *h : herz_g_) zeigen(h, false);
    return;
  }
  for (int i = 0; i < 6; i++) {
    int ti = (int) t - i * 350;
    if (ti < 0) {
      lv_obj_set_y(herz_g_[i], 260);
      continue;
    }
    lv_obj_set_pos(herz_g_[i], 40 + i * 44 + (int) (10 * sinf(ti / 250.0f + i)), 210 - ti * 220 / 3000);
    lv_obj_set_style_text_opa(herz_g_[i], (lv_opa_t) std::max(0, 255 - ti * 255 / 3200), 0);
  }
}

inline void Figur::hilfe_bauen() {
  hilfe_knopf_ = form(root_, 298, 218, 19, 19, 0, 10);  // kleiner Kreis mit „?“
  transparent(hilfe_knopf_);
  rand(hilfe_knopf_, 2, 0x7A88A8);
  lv_obj_t *fz = lv_label_create(hilfe_knopf_);
  lv_label_set_text(fz, "?");
  lv_obj_set_style_text_color(fz, lv_color_hex(0x9AA6C4), 0);
  lv_obj_align(fz, LV_ALIGN_CENTER, 0, 0);
  hilfe_tafel_ = form(root_, 0, 0, 320, 240, 0x0E1220, 0);
  lv_obj_set_style_bg_opa(hilfe_tafel_, 240, 0);
  lv_obj_t *titel = lv_label_create(hilfe_tafel_);
  lv_label_set_text(titel, "So geht Yellow");
  lv_obj_set_style_text_color(titel, lv_color_hex(GELB), 0);
  lv_obj_set_pos(titel, 14, 8);
  lv_obj_t *text = lv_label_create(hilfe_tafel_);
  lv_obj_set_style_text_color(text, lv_color_hex(0xD8DEE9), 0);
  lv_obj_set_style_text_line_space(text, 1, 0);
  lv_obj_set_pos(text, 14, 34);
  lv_label_set_text(text,
                    "Antippen: freut sich, holt Post ab\n"
                    "4x schnell: lieber nicht ...\n"
                    "Wischen: streicheln\n"
                    "Halten: Wecker, Termin, Wetter\n"
                    "Uhr oben rechts: 15 s Uhr\n"
                    "Im Dunkeln antippen: Uhr\n"
                    "Faust? Antippen!");
  lv_obj_t *zu = lv_label_create(hilfe_tafel_);
  lv_label_set_text(zu, "Antippen schließt");
  lv_obj_set_style_text_color(zu, lv_color_hex(0x5A6680), 0);
  lv_obj_set_pos(zu, 14, 214);
  zeigen(hilfe_tafel_, false);

  spiele_knopf_ = form(root_, 3, 218, 19, 19, 0, 10);  // Kreis mit Gamepad, gleiche Machart wie das „?“
  transparent(spiele_knopf_);
  rand(spiele_knopf_, 2, 0x7A88A8);
  lv_obj_t *sk = lv_label_create(spiele_knopf_);
  if (status_font_) lv_obj_set_style_text_font(sk, status_font_, 0);
  lv_label_set_text(sk, status_font_ ? "\U000F0297" : "S");
  lv_obj_set_style_text_color(sk, lv_color_hex(0x9AA6C4), 0);
  lv_obj_align(sk, LV_ALIGN_CENTER, 0, 0);
  spiele_tafel_ = form(root_, 0, 0, 320, 240, 0x0E1220, 0);
  lv_obj_set_style_bg_opa(spiele_tafel_, 240, 0);
  lv_obj_t *st = lv_label_create(spiele_tafel_);
  lv_label_set_text(st, "Was spielen wir?");
  lv_obj_set_style_text_color(st, lv_color_hex(GELB), 0);
  lv_obj_set_pos(st, 14, 8);
  static const char *const NAMEN[3] = {"Reaktionstest", "Tic-Tac-Toe", "Simon Says"};
  for (int i = 0; i < 3; i++) {  // drei große Felder, je 50 px hoch ab y = 44
    lv_obj_t *f = form(spiele_tafel_, 14, 44 + i * 56, 292, 48, 0x1E2740, 10);
    rand(f, 2, 0x3A4868);
    lv_obj_t *l = lv_label_create(f);
    lv_label_set_text(l, NAMEN[i]);
    lv_obj_set_style_text_color(l, lv_color_hex(0xD8DEE9), 0);
    lv_obj_align(l, LV_ALIGN_CENTER, 0, 0);
  }
  lv_obj_t *sz = lv_label_create(spiele_tafel_);
  lv_label_set_text(sz, "Daneben tippen schließt");
  lv_obj_set_style_text_color(sz, lv_color_hex(0x5A6680), 0);
  lv_obj_set_pos(sz, 14, 214);
  zeigen(spiele_tafel_, false);
}
inline void Figur::spiele_menue(bool an) {
  if (spiele_tafel_ == nullptr) return;
  zeigen(spiele_tafel_, an);
  if (an) lv_obj_move_foreground(spiele_tafel_);
  spiele_bis_ = an ? esphome::millis() + 20000 : 0;
}
inline void Figur::spiele_tipp(int y) {
  spiele_menue(false);
  int i = (y - 44) / 56;
  static const char *const SPIELE[3] = {"reaktion", "ttt", "simon"};
  if (y >= 44 && i < 3 && (y - 44) % 56 < 48) spiel(SPIELE[i]);
}

inline void Figur::hilfe(bool an) {
  if (hilfe_tafel_ == nullptr) return;
  zeigen(hilfe_tafel_, an);
  if (an) {
    lv_obj_move_foreground(hilfe_tafel_);
    hilfe_bis_ = esphome::millis() + 20000;
  } else {
    hilfe_bis_ = 0;
  }
}

inline void Figur::netz(int stufe, const std::string &ssid) {
  if (root_ == nullptr) return;
  if (netz_symbol_ == nullptr) {
    netz_symbol_ = lv_label_create(root_);
    if (status_font_) lv_obj_set_style_text_font(netz_symbol_, status_font_, 0);
    lv_obj_set_pos(netz_symbol_, 276, 220);
    netz_text_ = lv_label_create(root_);
    lv_obj_set_width(netz_text_, 300);
    lv_obj_set_style_text_align(netz_text_, LV_TEXT_ALIGN_RIGHT, 0);
    lv_obj_set_pos(netz_text_, 12, 196);
  }
  if (stufe > 0 && stufe != netz_stufe_) {
    static const char *const SYMBOL[] = {"", "\U000F05AA", "\U000F0164", "\U000F0A8E"};
    const bool rot = stufe == 1;
    lv_label_set_text(netz_symbol_, status_font_ ? SYMBOL[stufe] : "!");
    lv_obj_set_style_text_color(netz_symbol_, lv_color_hex(rot ? 0xFF4A4A : 0xFF9A2A), 0);
    lv_obj_set_style_text_color(netz_text_, lv_color_hex(rot ? 0xFF7A6A : 0xFFB060), 0);
  }
  if (stufe == 1) lv_label_set_text(netz_text_, ("wartet auf Netz" + (ssid.empty() ? std::string() : ": " + ssid)).c_str());
  else if (stufe == 2) lv_label_set_text(netz_text_, "Server nicht erreichbar");
  else if (stufe == 3) lv_label_set_text(netz_text_, ("kein Internet" + (ssid.empty() ? std::string() : " in " + ssid)).c_str());
  netz_stufe_ = stufe;
  zeigen(netz_symbol_, stufe > 0);
  zeigen(netz_text_, stufe > 0);
}

inline void Figur::schilder_planen(const std::string &text, int sekunden) {
  schilder_.clear();
  std::string zeile, wort;
  auto wort_dazu = [&]() {
    if (wort.empty()) return;
    if (!zeile.empty() && zeile.size() + 1 + wort.size() > 24) {
      schilder_.push_back(zeile);
      zeile.clear();
    }
    zeile += (zeile.empty() ? "" : " ") + wort;
    wort.clear();
  };
  for (char c : text) {
    if (c == '\n') {  // Zeilenumbruch = neues Schild
      wort_dazu();
      if (!zeile.empty()) schilder_.push_back(zeile);
      zeile.clear();
    } else if (c == ' ') {
      wort_dazu();
    } else {
      wort += c;
    }
  }
  wort_dazu();
  if (!zeile.empty()) schilder_.push_back(zeile);
  if (schilder_.empty()) return;
  if (schilder_.size() > 8) schilder_.resize(8);
  schild_dauer_ = std::max(2, sekunden);
  zeige_schild(schilder_.front(), schild_dauer_);
  schilder_.pop_front();
}

}  // namespace yellow
