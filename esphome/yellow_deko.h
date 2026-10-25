#pragma once
// Yellow: Deko (Anlässe, Jahreszeiten), Wetter-Ansicht, Hut (Methoden von Figur, siehe yellow.h)
#include "yellow.h"

namespace yellow {

inline void Figur::saison(int monat, int tag) {
  monat_ = monat;
  tag_ = tag;
  deko_anwenden();
}

inline void Figur::deko(int modus) {
  deko_modus_ = modus;
  deko_anwenden();
}

inline void Figur::anlass(const std::string &a) {
  size_t t = a.find('|');
  anlass_ = a.substr(0, t);
  anlass_zahl_ = t == std::string::npos ? 0 : atoi(a.c_str() + t + 1);  // advent|2 = zweiter Advent
  deko_anwenden();
}

inline void Figur::hut() {
  zeigen(kuerbis_, halloween_);
  zeigen(stiel_, halloween_);
  zeigen(muetze_, weihnacht_);
  bool g = deko_satz_ != nullptr && deko_satz_->hut[0];
  bool acc = !g && !halloween_ && !weihnacht_ && accessoire_[0];  // Anlass-Hut geht vor
  zeigen(hut_glyph_, g || acc);
  if (hut_glyph_ && (g || acc)) {
    lv_label_set_text(hut_glyph_, g ? deko_satz_->hut : accessoire_);
    uint32_t hf = g ? deko_satz_->hut_farbe : accessoire_farbe_;
    if (art_ == OKTI && (hf == 0xFFD23F || hf == 0xFFE08A)) hf = 0xD98E00;  // Gelb auf Oktis gelbem Kopf: dunkleres Gold
    lv_obj_set_style_text_color(hut_glyph_, lv_color_hex(hf), 0);
  }
}

inline void Figur::deko_anwenden() {
  // Schlüssel: Test > Auswahl (Aus/Halloween/Weihnachten) > Anlass von HA > Datum (falls HA nichts liefert)
  std::string k;
  if (!deko_test_.empty()) k = deko_test_;
  else if (deko_modus_ == 1) k = "";
  else if (deko_modus_ == 2) k = "halloween";
  else if (deko_modus_ == 3) k = "weihnachten";
  else if (!anlass_.empty()) k = anlass_;
  else if ((monat_ == 10 && tag_ >= 17) || (monat_ == 11 && tag_ == 1)) k = "halloween";
  else if (monat_ == 12 && tag_ <= 26) k = "weihnachten";
  halloween_ = k == "halloween";
  weihnacht_ = k == "weihnachten" || k == "nikolaus";
  zeigen(deko_h_, halloween_);
  zeigen(deko_w_, k == "weihnachten");
  deko_satz_ = nullptr;
  for (auto &d : DEKO_SAETZE)
    if (k == d.key) deko_satz_ = &d;
  for (int i = 0; i < 8; i++) {
    if (deko_g_[i] == nullptr) continue;
    bool an = deko_satz_ != nullptr && deko_satz_->glyph[i][0];
    zeigen(deko_g_[i], an);
    lv_obj_set_pos(deko_g_[i], DEKO_POS[i][0], DEKO_POS[i][1]);
    lv_obj_set_style_text_opa(deko_g_[i], LV_OPA_COVER, 0);
    lv_obj_set_style_transform_rotation(deko_g_[i], 0, 0);
    if (an) {
      lv_label_set_text(deko_g_[i], deko_satz_->glyph[i]);
      lv_obj_set_style_text_color(deko_g_[i], lv_color_hex(deko_satz_->farbe[i]), 0);
    }
  }
  // Advent: nur so viele Kerzen leuchten, wie Advent ist (Slots 0, 2, 3, 5 sind Kerzen)
  if (k == "advent" && anlass_zahl_ > 0) {
    static const int KERZE[4] = {0, 2, 3, 5};
    for (int n = 0; n < 4; n++)
      if (deko_g_[KERZE[n]]) lv_obj_set_style_text_color(deko_g_[KERZE[n]], lv_color_hex(n < anlass_zahl_ ? 0xFFE08A : 0x5A5A66), 0);
  }
  bool rosa = k == "valentin";
  if (rosa != rosa_) {
    rosa_ = rosa;
    farb_st_ = -1;  // Farbe neu setzen
    if (koerper_) ausdruck_setzen(ausdruck_);
  }
  deko_key_ = k;
  hut();
}

inline void Figur::deko_bauen() {
  // --- Halloween: Mond, Fledermäuse, Kürbisse mit Gesicht ---
  deko_h_ = form(root_, 0, 0, 320, 240, 0, 0);
  lv_obj_set_style_bg_opa(deko_h_, LV_OPA_TRANSP, 0);
  lv_obj_t *mond = form(deko_h_, 14, 14, 34, 34, 0xF5E6A8);
  form(mond, 18, -4, 30, 30, HINTERGRUND);  // Sichel
  static const int FLEDER[3][2] = {{70, 40}, {238, 62}, {200, 26}};
  for (int i = 0; i < 3; i++) {
    form(deko_h_, FLEDER[i][0] - 3, FLEDER[i][1] - 3, 7, 7, 0x7A6AA8);
    fluegel_[i] = linie(deko_h_, 3, 0x7A6AA8);
    fleder_pos_[i][0] = FLEDER[i][0];
    fleder_pos_[i][1] = FLEDER[i][1];
  }
  fledermaeuse_setzen(0);
  static const int KUERBIS[2][3] = {{10, 196, 40}, {272, 202, 34}};  // x, y, Breite
  for (auto &k : KUERBIS) {
    int w = k[2], h = w * 3 / 4;
    form(deko_h_, k[0] + w / 2 - 3, k[1] - 7, 6, 9, 0x3D7A2A, 2);
    lv_obj_t *kb = form(deko_h_, k[0], k[1], w, h, 0xF28C28);
    rand(kb, 2, 0xA84E10);
    form(kb, w / 4 - 3, h / 3 - 2, 7, 6, 0x2A1A0A, 1);
    form(kb, w * 3 / 4 - 5, h / 3 - 2, 7, 6, 0x2A1A0A, 1);
    form(kb, w / 2 - 8, h * 2 / 3 - 3, 15, 4, 0x2A1A0A, 2);
  }

  // --- Weihnachten: Tannenbaum, Lichterkette, Geschenk, Schnee ---
  deko_w_ = form(root_, 0, 0, 320, 240, 0, 0);
  lv_obj_set_style_bg_opa(deko_w_, LV_OPA_TRANSP, 0);
  form(deko_w_, 31, 218, 10, 16, 0x6B4226, 2);                 // Stamm
  form(deko_w_, 6, 190, 60, 30, 0x1F7A3A, 14);                  // Baum unten
  form(deko_w_, 13, 164, 46, 30, 0x248A42, 14);                 // Mitte
  form(deko_w_, 21, 140, 30, 28, 0x2A9A4A, 13);                 // oben
  form(deko_w_, 29, 128, 14, 14, 0xFFD23F);                     // Stern
  static const int KUGEL[6][2] = {{18, 202}, {46, 206}, {30, 180}, {48, 176}, {26, 154}, {40, 196}};
  for (int i = 0; i < 6; i++) kugel_[i] = form(deko_w_, KUGEL[i][0], KUGEL[i][1], 7, 7, ROT);
  // Lichterkette oben (hängt durch), Uhr oben rechts bleibt frei
  for (int i = 0; i < 9; i++) {
    float t = i / 8.0f;
    kette_pkt_[i].x = 4 + t * 236;
    kette_pkt_[i].y = 6 + 22 * 4 * t * (1 - t);
  }
  lv_obj_t *draht = linie(deko_w_, 2, 0x1E3A24);
  lv_line_set_points(draht, kette_pkt_, 9);
  for (int i = 0; i < 12; i++) {
    float t = (i + 0.5f) / 12.0f;
    int x = 4 + (int) (t * 236), y = 6 + (int) (22 * 4 * t * (1 - t));
    birne_[i] = form(deko_w_, x - 3, y + 1, 7, 9, 0xFFD23F);
  }
  // Geschenk unten rechts
  lv_obj_t *gs = form(deko_w_, 270, 204, 34, 28, ROT, 3);
  form(gs, 15, 0, 4, 28, 0xFFD23F, 0);
  form(gs, 0, 11, 34, 4, 0xFFD23F, 0);
  // Schnee
  for (int i = 0; i < 14; i++) {
    flocke_x_[i] = (float) zufall(320);
    flocke_y_[i] = (float) zufall(240);
    flocke_[i] = form(deko_w_, (int) flocke_x_[i], (int) flocke_y_[i], 3, 3, 0xE8F0FF);
  }
  zeigen(deko_h_, false);
  zeigen(deko_w_, false);
  for (int i = 0; i < 8; i++) {
    deko_g_[i] = lv_label_create(root_);
    if (deko_font_) lv_obj_set_style_text_font(deko_g_[i], deko_font_, 0);
    lv_obj_set_pos(deko_g_[i], DEKO_POS[i][0], DEKO_POS[i][1]);
    lv_label_set_text(deko_g_[i], "");
    zeigen(deko_g_[i], false);
  }
  wetter_bauen();
}

inline void Figur::wetter_bauen() {
  wetter_ = form(root_, 0, 0, 320, 240, 0, 0);
  lv_obj_set_style_bg_opa(wetter_, LV_OPA_TRANSP, 0);
  sonne_ = form(wetter_, 0, 0, 320, 240, 0, 0);
  lv_obj_set_style_bg_opa(sonne_, LV_OPA_TRANSP, 0);
  for (int i = 0; i < 8; i++) strahl_[i] = linie(sonne_, 3, 0xFFD23F);
  form(sonne_, SONNE_X - 15, SONNE_Y - 15, 30, 30, 0xFFD23F);
  for (int i = 0; i < 3; i++) {  // Nebelbänder
    nebel_[i] = form(wetter_, -20, 120 + i * 34, 360, 16, 0x9AA4B8, 8);
    lv_obj_set_style_bg_opa(nebel_[i], 70, 0);
  }
  for (int i = 0; i < 2; i++) {  // Wolken aus drei Kreisen und flachem Boden
    wolke_[i] = form(wetter_, 0, 0, 90, 46, 0, 0);
    lv_obj_set_style_bg_opa(wolke_[i], LV_OPA_TRANSP, 0);
    wolke_teil_[i][0] = form(wolke_[i], 0, 20, 90, 26, 0, 13);
    wolke_teil_[i][1] = form(wolke_[i], 12, 6, 34, 34, 0);
    wolke_teil_[i][2] = form(wolke_[i], 36, 0, 42, 42, 0);
    wolke_x_[i] = i ? 150.0f : -10.0f;
  }
  for (int i = 0; i < TEILCHEN; i++) {
    tropfen_x_[i] = (float) zufall(340);
    tropfen_y_[i] = (float) zufall(240);
    tropfen_[i] = form(wetter_, 0, 0, 2, 9, 0x7FB8FF, 1);
  }
  blitz_ = linie(wetter_, 4, 0xFFF3A0);
  static const int BLITZ[5][2] = {{70, 34}, {58, 70}, {74, 72}, {60, 112}, {82, 66}};
  for (int i = 0; i < 5; i++) {
    blitz_pkt_[i].x = BLITZ[i][0];
    blitz_pkt_[i].y = BLITZ[i][1];
  }
  lv_line_set_points(blitz_, blitz_pkt_, 5);
  zeigen(blitz_, false);
  wetter_anwenden();  // Lage kam evtl. schon vor dem Aufbau aus HA
}

inline void Figur::wetter_anwenden() {
  if (wetter_ == nullptr) return;
  const std::string &l = wetter_test_.empty() ? wetter_ha_ : wetter_test_;
  auto hat = [&l](const char *s) { return l.find(s) != std::string::npos; };
  // Wetterlaune: Regen/Gewitter machen etwas grummeliger (Farbe Richtung rot, Mundwinkel), Sonne froher
  wetter_laune_ = hat("lightning") || l == "pouring" ? 18 : l == "rainy" || l == "hail" || l == "snowy-rainy" ? 12
                  : l == "fog" ? 6 : l == "cloudy" ? 4 : l == "sunny" ? -10 : 0;
  if (koerper_ && !ausdruck_bis_) ausdruck_setzen(ausdruck_);
  // Wetter im Gesicht: bei einem Wechsel schaut Yellow nach oben und reagiert kurz
  if (sichtbar_ && !wetter_alt_.empty() && l != wetter_alt_ && !schild_bis_) {
    blick("oben");
    if (hat("lightning") && art_ == FUCHS) zeige_schild("Pff. Donner.", 3);
    else if (hat("lightning")) geste("erschrecken");
    else if (hat("snowy")) {
      zeitweise(FROH, 3000);
      zeige_schild("Schnee!", 3);
    } else if (l == "pouring" || l == "rainy") zeige_schild(l == "pouring" ? "Oh oh." : "Regen.", 3);
    else if (l == "sunny") {
      zeitweise(FROH, 2500);
      zeige_schild("Sonne!", 3);
    } else if (hat("windy")) {  // Charakter: Okti wird durchgeschüttelt, Fuchs stellt sich rein, Roboter misst
      if (art_ == OKTI) {
        anim(cb_wackel, 0, 1000, 1500);
        zeige_schild("Huiii!", 3);
      } else if (art_ == FUCHS) {
        zeitweise(FROH, 2500);
        zeige_schild("Frische Luft.", 3);
      } else {
        zeige_schild(esphome::str_sprintf("Wind: %d km/h. Geschätzt.", 30 + (int) zufall(25)), 3);
      }
    }
  }
  wetter_alt_ = l;
  bool blitz = hat("lightning"), schnee = hat("snowy"), hagel = l == "hail";
  bool regen = l == "rainy" || l == "pouring" || l == "lightning-rainy" || l == "snowy-rainy" || hagel;
  bool wolken = regen || schnee || blitz || l == "cloudy" || l == "partlycloudy";
  bool dunkel = regen || blitz || l == "cloudy";
  wetter_wind_ = hat("windy");
  wetter_blitz_ = blitz;
  blitz_an_ = 0;
  naechster_blitz_ = esphome::millis() + 2000;
  zeigen(blitz_, false);
  lv_obj_set_style_bg_color(root_, lv_color_hex(HINTERGRUND), 0);
  zeigen(sonne_, l == "sunny" || l == "partlycloudy");
  for (int i = 0; i < 3; i++) zeigen(nebel_[i], l == "fog");
  for (int i = 0; i < 2; i++) {
    zeigen(wolke_[i], wolken && (i == 0 || l != "partlycloudy"));
    for (auto *w : wolke_teil_[i]) lv_obj_set_style_bg_color(w, lv_color_hex(dunkel ? 0x5C667A : 0xC8D0DC), 0);
  }
  // Teilchen: 0 aus, sonst Anzahl; Regen blaue Striche, Schnee weiße Punkte, Schneeregen/Hagel gemischt
  int n = l == "pouring" || l == "lightning-rainy" ? TEILCHEN : regen || schnee ? TEILCHEN * 2 / 3 : wetter_wind_ ? 6 : 0;
  for (int i = 0; i < TEILCHEN; i++) {
    bool flocke = schnee && (l == "snowy" || i % 2) || hagel && i % 2;
    bool blatt = wetter_wind_ && !regen && !schnee;
    tropfen_art_[i] = flocke ? 1 : blatt ? 2 : 0;
    lv_obj_set_size(tropfen_[i], flocke ? 4 : blatt ? 7 : 3, flocke ? 4 : blatt ? 5 : 12);
    lv_obj_set_style_radius(tropfen_[i], flocke ? 2 : blatt ? 3 : 1, 0);
    lv_obj_set_style_bg_color(tropfen_[i], lv_color_hex(flocke ? 0xE8F0FF : blatt ? 0xC9782A : 0xA8D4FF), 0);
    zeigen(tropfen_[i], i < n);
  }
  zeigen(wetter_, !l.empty());
}

inline void Figur::wetter_tick(uint32_t jetzt) {
  if (wetter_ == nullptr || lv_obj_has_flag(wetter_, LV_OBJ_FLAG_HIDDEN) || jetzt - letztes_wetter_ < 60) return;
  letztes_wetter_ = jetzt;
  float wind = wetter_wind_ ? 3.0f : 0.6f;
  for (int i = 0; i < 2; i++) {  // Wolken ziehen (mit Wind schneller), oben, die hintere etwas tiefer
    wolke_x_[i] += (0.25f + i * 0.12f) * wind;
    if (wolke_x_[i] > 330) wolke_x_[i] = -100;
    lv_obj_set_pos(wolke_[i], (int) wolke_x_[i], 18 + i * 22);
  }
  for (int i = 0; i < 3; i++)
    if (!lv_obj_has_flag(nebel_[i], LV_OBJ_FLAG_HIDDEN))
      lv_obj_set_x(nebel_[i], -20 + (int) (18 * sinf(jetzt / 2600.0f + i * 2.1f)));
  if (!lv_obj_has_flag(sonne_, LV_OBJ_FLAG_HIDDEN)) {  // Strahlen drehen sich langsam und pulsieren
    float d = jetzt / 3000.0f, r = 22 + 2 * sinf(jetzt / 500.0f);
    for (int i = 0; i < 8; i++) {
      float w = d + i * 0.785f;
      strahl_pkt_[i][0].x = SONNE_X + (int) (r * cosf(w));
      strahl_pkt_[i][0].y = SONNE_Y + (int) (r * sinf(w));
      strahl_pkt_[i][1].x = SONNE_X + (int) ((r + 9) * cosf(w));
      strahl_pkt_[i][1].y = SONNE_Y + (int) ((r + 9) * sinf(w));
      lv_line_set_points(strahl_[i], strahl_pkt_[i], 2);
    }
  }
  for (int i = 0; i < TEILCHEN; i++) {
    if (lv_obj_has_flag(tropfen_[i], LV_OBJ_FLAG_HIDDEN)) continue;
    int art = tropfen_art_[i];
    tropfen_y_[i] += art == 1 ? 1.2f + (i % 3) * 0.4f : art == 2 ? 0.8f * sinf(jetzt / 300.0f + i) : 7.0f + (i % 3);
    tropfen_x_[i] += art == 1 ? sinf(jetzt / 900.0f + i) * 0.5f + wind * 0.3f : art == 2 ? 6.0f + (i % 3) : wind * 0.5f;
    if (tropfen_y_[i] > 240 || tropfen_x_[i] > 330) {
      tropfen_y_[i] = art == 2 ? (float) (60 + zufall(160)) : -10.0f;
      tropfen_x_[i] = art == 2 ? -10.0f : (float) zufall(330) - 10;
    }
    lv_obj_set_pos(tropfen_[i], (int) tropfen_x_[i], (int) tropfen_y_[i]);
  }
  if (wetter_blitz_) {  // Gewitter: alle 6–14 s blitzt der Hintergrund kurz auf
    if (!blitz_an_ && vorbei(jetzt, naechster_blitz_)) {
      blitz_an_ = jetzt;
      int x = (int) wolke_x_[0] + 20;
      static const int ZACKE[5] = {0, -12, 4, -10, 12};
      for (int i = 0; i < 5; i++) blitz_pkt_[i].x = x + ZACKE[i];
      lv_line_set_points(blitz_, blitz_pkt_, 5);
      zeigen(blitz_, true);
      lv_obj_set_style_bg_color(root_, lv_color_hex(0x4A5370), 0);
    } else if (blitz_an_ && jetzt - blitz_an_ > 140) {
      blitz_an_ = 0;
      naechster_blitz_ = jetzt + 6000 + zufall(8000);
      zeigen(blitz_, false);
      lv_obj_set_style_bg_color(root_, lv_color_hex(HINTERGRUND), 0);
    }
  }
}

inline void Figur::deko_tick(uint32_t jetzt) {
  if (deko_key_ == "perseiden" && deko_g_[0]) {  // Sternschnuppe zieht alle 6 s schräg über den Himmel
    uint32_t t = jetzt % 6000;
    lv_obj_set_pos(deko_g_[0], t < 2200 ? -30 + (int) (t * 360 / 2200) : -40, t < 2200 ? 6 + (int) (t * 70 / 2200) : -40);
  }
  if ((deko_key_ == "herbst" || deko_key_ == "erntedank") && jetzt - letzte_laub_ >= 70) {  // Laub fällt und schaukelt
    letzte_laub_ = jetzt;
    static const int FALL[3] = {0, 3, 7};
    for (int n = 0; n < 3; n++) {
      lv_obj_t *g = deko_g_[FALL[n]];
      if (g == nullptr) continue;
      uint32_t t = (jetzt / 70 + n * 60) % 180;
      int x = DEKO_POS[FALL[n]][0] + (int) (14 * sinf(t / 9.0f + n));
      lv_obj_set_pos(g, x, -30 + (int) t * 3 / 2);
    }
  }
  if (deko_key_ == "zeitumstellung" && deko_g_[0]) {  // eine Uhr dreht sich (Stunde vor oder zurück)
    lv_obj_set_style_transform_pivot_x(deko_g_[0], 15, 0);
    lv_obj_set_style_transform_pivot_y(deko_g_[0], 15, 0);
    lv_obj_set_style_transform_rotation(deko_g_[0], (int32_t) ((jetzt / 4) % 3600) * (monat_ == 10 ? -1 : 1), 0);
  }
  if ((deko_key_ == "oktoberfest" || deko_key_ == "bier") && deko_g_[0] && deko_g_[3]) {  // Krüge stoßen alle 5 s an
    uint32_t t = jetzt % 5000;
    int weg = t < 700 ? (int) (t * 100 / 700) : t < 1100 ? 100 : t < 1800 ? 100 - (int) ((t - 1100) * 100 / 700) : 0;
    lv_obj_set_pos(deko_g_[0], DEKO_POS[0][0] + weg * (130 - DEKO_POS[0][0]) / 100, DEKO_POS[0][1] - weg * 20 / 100);
    lv_obj_set_pos(deko_g_[3], DEKO_POS[3][0] - weg * (DEKO_POS[3][0] - 162) / 100, DEKO_POS[3][1] - weg * 30 / 100);
  }
  if (deko_key_ == "vollmond" && deko_g_[1]) {  // Mond leuchtet sanft, Sterne funkeln
    lv_obj_set_style_text_opa(deko_g_[1], (lv_opa_t) (170 + 85 * sinf(jetzt / 900.0f)), 0);
    for (int i : {0, 2, 3, 5, 6, 7})
      if (deko_g_[i]) lv_obj_set_style_text_opa(deko_g_[i], ((jetzt / 400 + i) % 4) ? LV_OPA_COVER : 70, 0);
  }
  if (deko_key_ == "ostern" && deko_g_[7]) {  // Osterhase hüpft am Boden quer durchs Bild, alle 14 s
    uint32_t t = jetzt % 14000;
    int x = -40 + (int) (t * 400 / 14000);
    int y = 200 - (int) (22 * fabsf(sinf(t / 260.0f)));
    lv_obj_set_pos(deko_g_[7], x, y);
  }
  if (deko_key_ == "silvester" && jetzt - letzte_deko_ >= 450) {  // Feuerwerk: Raketen blitzen versetzt auf
    letzte_deko_ = jetzt;
    deko_takt_++;
    for (int i = 0; i < 8; i++)
      if (deko_g_[i]) lv_obj_set_style_text_opa(deko_g_[i], ((i + deko_takt_) % 3) ? LV_OPA_COVER : 60, 0);
  }
  if (halloween_ && jetzt - letzte_deko_ >= 300) {
    letzte_deko_ = jetzt;
    fledermaeuse_setzen(++deko_takt_);
  }
  if (weihnacht_) {
    if (jetzt - letzte_deko_ >= 500) {
      letzte_deko_ = jetzt;
      deko_takt_++;
      static const uint32_t FARBEN[4] = {0xFF4A4A, 0xFFD23F, 0x4AA8FF, 0x5AE07A};
      for (int i = 0; i < 12; i++)
        lv_obj_set_style_bg_color(birne_[i], lv_color_hex(FARBEN[(i + deko_takt_) % 4]), 0);
      for (int i = 0; i < 6; i++)
        lv_obj_set_style_bg_color(kugel_[i], lv_color_hex(FARBEN[(i * 3 + deko_takt_ / 2) % 4]), 0);
    }
    if (jetzt - letzte_schnee_ >= 90) {
      letzte_schnee_ = jetzt;
      for (int i = 0; i < 14; i++) {
        flocke_y_[i] += 0.8f + (i % 3) * 0.4f;
        flocke_x_[i] += sinf((jetzt / 900.0f) + i) * 0.4f;
        if (flocke_y_[i] > 240) {
          flocke_y_[i] = -4;
          flocke_x_[i] = (float) zufall(320);
        }
        lv_obj_set_pos(flocke_[i], (int) flocke_x_[i], (int) flocke_y_[i]);
      }
    }
  }
}

}  // namespace yellow
