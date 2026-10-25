#pragma once
// Yellow: Figur bauen und zeichnen: Körper, Arme/Tentakel/Schwanz, Augen, Farbe, Ausdruck (Methoden von Figur, siehe yellow.h)
#include "yellow.h"

namespace yellow {

inline void Figur::figur_bauen() {
  if (koerper_ != nullptr) lv_obj_delete(koerper_);
  antenne_ = kopf_ = muetze_ = brille_ = schal_ = schirm_ = faecher_ = kanne_ = hut_glyph_ = tuete_ = nullptr;
  korb_ = tonne_ = kissen_ = nullptr;
  schmuck_ = schmuck_glanz_ = nullptr;
  visier_ = nullptr;
  glitch_start_ = 0;
  prop_[0] = prop_[1] = nullptr;
  prop_start_ = jagen_start_ = 0;
  zahnrad_[0] = zahnrad_[1] = nullptr;
  klappe_start_ = 0;
  schweiss_ = traene_ = nullptr;
  roet_an_ = false;
  arm0_vorn_ = false;
  braue_[0] = braue_[1] = nullptr;
  arm_[0] = arm_[1] = nullptr;
  for (int i = 0; i < ARME; i++) tentakel_[i] = unterseite_[i] = nullptr;
  for (auto &arm : saugnapf_)
    for (auto *&s : arm) s = nullptr;
  plan_ = art_ == OKTI ? &PLAN_OKTI : art_ == FUCHS ? &PLAN_FUCHS : &PLAN_ROBOTER;
  robo_arm_[0] = robo_arm_[1] = robo_hand_[0] = robo_hand_[1] = herz_ = nullptr;
  schwanz_ = schwanz_spitze_ = schwanz_rand_ = schwanz_bauch_ = nullptr;
  for (auto *&l : spitze_) l = nullptr;
  pfote_[0] = pfote_[1] = nullptr;
  const Bauplan &p = *plan_;
  koerper_ = form(root_, p.kx, p.ky, 180, 170, 0, 0);
  transparent(koerper_);
  // LVGL zeichnet Kinder nur im Bereich des Eltern-Objekts (+ ext_draw_size): Arme, Kürbis und Antenne
  // ragen hinaus, also den Zeichenbereich rundum erweitern
  lv_obj_add_event_cb(koerper_, cb_rand, LV_EVENT_REFR_EXT_DRAW_SIZE, nullptr);
  lv_obj_refresh_ext_draw_size(koerper_);
  lv_obj_t *traeger;  // trägt das Gesicht
  if (art_ == OKTI) {
    // Arme hinter dem Kopf: erst die andersfarbige Unterseite (breiter, leicht tiefer), dann der Arm
    for (int i = 0; i < ARME; i++) unterseite_[i] = linie(koerper_, 17, ROT);
    for (int i = 0; i < ARME; i++) tentakel_[i] = linie(koerper_, 13, GELB);
    for (int i = 0; i < ARME; i++) {
      for (lv_obj_t *l : {unterseite_[i], tentakel_[i]}) {
        lv_obj_set_pos(l, -ARM_DX, -ARM_DY);
        lv_obj_set_size(l, 360, 300);
      }
    }
    for (auto &arm : saugnapf_)  // Saugnäpfe vor den Armen, hinter dem Kopf
      for (auto *&s : arm) s = form(koerper_, 0, 0, 5, 5, 0xFFE2B0);
    kopf_ = form(koerper_, 15, 0, 150, 126, GELB, 62);
    lv_obj_t *glanz = form(kopf_, 30, 14, 36, 18, 0xFFFFFF, 9);  // Glanzlicht oben links
    lv_obj_set_style_bg_opa(glanz, 80, 0);
    traeger = kopf_;
  } else if (art_ == FUCHS) {
    // Fuchs: Schwanz (hinten), Ohren als Dreiecke aus aufgefächerten Linien, Körper mit Bauch, Kopf mit weißer Schnauze
    // buschig: dunkler Rand und bauchige Mitte unter dem eigentlichen Schwanz, große weiße Spitze
    schwanz_rand_ = glied(koerper_, 48, 0xC85E12);
    schwanz_bauch_ = glied(koerper_, 42, FUCHS_ORANGE);
    schwanz_ = glied(koerper_, 28, FUCHS_ORANGE);
    schwanz_spitze_ = glied(koerper_, 32, FUCHS_CREME);
    // runde Spitze wie vorher (spitz und „leicht zulaufend“ probiert, Ottes fand rund am schönsten, 25.10.)
    // Ohren etwas runder (dickere Linien, runde Enden); Knick am rechten Ohr probiert und verworfen (Ottes 25.10.)
    for (int i = 0; i < 2; i++) {
      auto x = [i](int v) { return i ? 180 - v : v; };
      dreieck_linien(koerper_, OHR_SPITZE[i][0][0], OHR_SPITZE[i][0][1], x(14), 36, x(70), 20, FUCHS_ORANGE, 20,
                     ohr_pkt_[i][0]);
      dreieck_linien(koerper_, OHR_SPITZE[i][1][0], OHR_SPITZE[i][1][1], x(26), 28, x(56), 20, FUCHS_CREME, 9,
                     ohr_pkt_[i][1]);
    }
    lv_obj_t *rumpf = form(koerper_, 35, 104, 110, 62, FUCHS_ORANGE, 30);
    form(rumpf, 32, 10, 46, 44, FUCHS_CREME);
    pfote_[0] = form(koerper_, 44, 156, 28, 14, FUCHS_BRAUN);
    pfote_[1] = form(koerper_, 108, 156, 28, 14, FUCHS_BRAUN);
    kopf_ = form(koerper_, 15, 12, 150, 110, FUCHS_ORANGE, 46);
    form(kopf_, 26, 56, 98, 58, FUCHS_CREME);  // weiße Schnauze/Wangen
    form(kopf_, 67, 60, 16, 11, FUCHS_BRAUN, 5);  // Nase
    traeger = kopf_;
  } else {
    // Roboter: Antenne, Ohrschrauben, Kopf mit dunklem Visier, Körper mit pulsierendem Herzlicht, Arme mit Händen
    form(koerper_, 88, -10, 4, 22, METALL, 2);
    antenne_ = form(koerper_, 83, -22, 14, 14, 0xFF5A7A);
    form(koerper_, -2, 40, 12, 32, METALL, 5);
    form(koerper_, 170, 40, 12, 32, METALL, 5);
    form(koerper_, 48, 164, 30, 10, METALL, 4);
    form(koerper_, 102, 164, 30, 10, METALL, 4);
    for (int i = 0; i < 2; i++) robo_arm_[i] = glied(koerper_, 9, METALL);
    lv_obj_t *rumpf = form(koerper_, 40, 118, 100, 50, GELB, 16);
    rand(rumpf, 3, GELB_RAND);
    herz_ = form(rumpf, 40, 14, 14, 14, 0xFF5A7A);
    lv_obj_t *kopf = form(koerper_, 5, 4, 170, 116, GELB, 28);
    rand(kopf, 3, GELB_RAND);
    for (int k = 0; k < 2; k++) {  // Zahnräder hinter dem Visier (sichtbar, wenn es hochklappt)
      zahnrad_[k] = form(kopf, k ? 84 : 36, k ? 34 : 24, k ? 34 : 44, k ? 34 : 44, 0x5A606E);
      rand(zahnrad_[k], 5, 0x8A93A6);
      lv_obj_set_style_border_side(zahnrad_[k], LV_BORDER_SIDE_FULL, 0);
      form(zahnrad_[k], k ? 10 : 15, k ? 10 : 15, 8, 8, 0x2A2F3A);
    }
    traeger = form(kopf, 12, 13, 140, 82, VISIER, 22);
    visier_ = traeger;
    for (auto *&s : scan_) {
      s = form(visier_, 0, 0, 140, 3, 0x7FE8FF, 0);
      lv_obj_set_style_bg_opa(s, 120, 0);
      zeigen(s, false);
    }
  }

  gesicht_ = form(traeger, 0, 0, 180, 150, 0, 0);
  transparent(gesicht_);
  int b = p.bogen;
  for (int i = 0; i < 2; i++) {
    wange_[i] = form(gesicht_, p.wange_x[i], p.wange_y, p.wange_b, p.wange_h, WANGE);
    auge_[i] = form(gesicht_, 0, 0, 24, 40, p.augen_farbe);
    glanz_[i] = form(auge_[i], 3, 4, 6, 6, 0xFFFFFF);
    froh_[i] = bogen(gesicht_, b, 5, p.augen_farbe);
    lv_arc_set_bg_angles(froh_[i], 200, 340);
    lv_obj_set_pos(froh_[i], p.auge_x[i] - b / 2, p.auge_y - b / 3);
    spirale_[i] = bogen(gesicht_, b, 4, p.augen_farbe);
    lv_arc_set_bg_angles(spirale_[i], 0, 290);
    lv_obj_set_pos(spirale_[i], p.auge_x[i] - b / 2, p.auge_y - b / 2);
  }
  {  // grummelige Brauen (alle Figuren; Roboter in Leuchtfarbe auf dem dunklen Visier)
    for (int i = 0; i < 2; i++) {
      int x = p.auge_x[i], s = i ? -1 : 1;
      braue_pkt_[i][0].x = x - 12 * s;
      braue_pkt_[i][0].y = p.auge_y - 24;
      braue_pkt_[i][1].x = x + 9 * s;
      braue_pkt_[i][1].y = p.auge_y - 15;
      braue_[i] = linie(gesicht_, 5, art_ == ROBOTER ? LEUCHT : AUGE);
      lv_line_set_points(braue_[i], braue_pkt_[i], 2);
    }
  }
  mund_bogen_ = bogen(gesicht_, p.mund_bogen, 4, p.mund_farbe);
  mund_o_ = form(gesicht_, 0, 0, 14, 16, p.mund_farbe);
  mund_strich_ = form(gesicht_, p.mund_x - 10, p.mund_y, 20, 4, p.mund_farbe, 2);

  // Requisiten: Sonnenbrille (im Gesicht), Schal (unter dem Gesicht), Regenschirm (über dem Kopf)
  brille_ = form(gesicht_, 0, 0, 180, 150, 0, 0);  // volle Größe: LVGL zeichnet Kinder nur im Elternbereich
  lv_obj_set_style_bg_opa(brille_, LV_OPA_TRANSP, 0);
  lv_obj_add_flag(brille_, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
  for (int i = 0; i < 2; i++) {
    lv_obj_t *g = form(brille_, p.auge_x[i] - 17, p.auge_y - 13, 34, 24, 0x101018, 9);
    rand(g, 2, 0x3A3A48);
    form(g, 5, 4, 8, 4, 0x6A6A88, 2);  // Glanz
  }
  form(brille_, (p.auge_x[0] + p.auge_x[1]) / 2 - 7, p.auge_y - 6, 14, 4, 0x101018, 2);
  schal_ = form(gesicht_, p.mund_x - 52, p.mund_y + 14, 104, 16, 0xC8463C, 8);
  form(schal_, 10, 0, 6, 16, 0xF4E4C1, 0);
  form(schal_, 40, 0, 6, 16, 0xF4E4C1, 0);
  form(schal_, 70, 0, 6, 16, 0xF4E4C1, 0);
  lv_obj_add_flag(schal_, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
  form(schal_, 72, 12, 16, 30, 0xC8463C, 6);  // herabhängendes Ende
  schirm_ = form(koerper_, 74, -26, 180, 170, 0, 0);  // seitlich rechts über der Schulter
  lv_obj_set_style_bg_opa(schirm_, LV_OPA_TRANSP, 0);
  lv_obj_add_flag(schirm_, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
  // Dach: Kreis in einem halb so hohen Kasten = gefülltes Halbrund, dazu Bogenkanten wie Schirm-Segmente
  lv_obj_t *dach = form(schirm_, 52, 2, 104, 52, 0, 0);
  lv_obj_set_style_bg_opa(dach, LV_OPA_TRANSP, 0);
  form(dach, 0, 0, 104, 104, 0x4A7AE8);
  for (int k = 0; k < 3; k++) form(dach, 8 + k * 32, 44, 24, 16, HINTERGRUND);  // gewellter Rand
  schirm_stiel_[0].x = 104;
  schirm_stiel_[0].y = 10;
  schirm_stiel_[1].x = 112;
  schirm_stiel_[1].y = 128;
  lv_obj_t *stiel = linie(schirm_, 4, 0x3A2418);
  lv_obj_set_size(stiel, 180, 170);
  lv_obj_set_pos(stiel, 0, 0);
  lv_line_set_points(stiel, schirm_stiel_, 2);
  lv_obj_move_background(schirm_);  // Stiel verschwindet hinter dem Kopf
  // Fächer links (Halbrund mit Streifen), Gießkanne rechts (Kanne, Tülle, Henkel)
  faecher_ = form(koerper_, -40, 70, 80, 40, 0, 0);
  lv_obj_set_style_bg_opa(faecher_, LV_OPA_TRANSP, 0);
  lv_obj_t *fb = form(faecher_, 0, 0, 80, 40, 0, 0);
  lv_obj_set_style_bg_opa(fb, LV_OPA_TRANSP, 0);
  form(fb, 0, 0, 80, 80, 0xFF8FA3);
  for (int k = 0; k < 4; k++) form(fb, 10 + k * 18, 0, 4, 40, 0xC8463C, 0);
  form(faecher_, 36, 34, 8, 6, 0x6B4226, 2);
  kanne_ = form(koerper_, 150, 96, 90, 60, 0, 0);
  lv_obj_set_style_bg_opa(kanne_, LV_OPA_TRANSP, 0);
  form(kanne_, 20, 20, 44, 36, 0x3D9A5A, 8);
  lv_obj_t *henkel = bogen(kanne_, 28, 5, 0x2E7A44);
  lv_arc_set_bg_angles(henkel, 180, 360);
  lv_obj_set_pos(henkel, 28, 8);
  kanne_tuelle_[0].x = 22;
  kanne_tuelle_[0].y = 40;
  kanne_tuelle_[1].x = 2;
  kanne_tuelle_[1].y = 22;
  lv_obj_t *tuelle = linie(kanne_, 6, 0x2E7A44);
  lv_obj_set_size(tuelle, 90, 60);
  lv_line_set_points(tuelle, kanne_tuelle_, 2);
  for (int k = 0; k < 3; k++) form(kanne_, 0 + k * 5, 26 + k * 7, 4, 6, 0x6FB8FF, 2);  // Tropfen
  // „Tüte“ für 16:20 (comichaft): weißes Röllchen am Mundwinkel, Glut, Rauchwölkchen steigen auf
  tuete_ = form(gesicht_, p.mund_x + 8, p.mund_y - 2, 30, 6, 0xF4F4F4, 3);
  lv_obj_add_flag(tuete_, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
  form(tuete_, 26, 0, 6, 6, 0xFF6A1A, 3);
  for (int k = 0; k < 3; k++) {  // Rauch direkt am Gesicht: als Kind der 6 px hohen Tüte wurde er nicht gezeichnet
    rauch_[k] = form(gesicht_, 0, 0, 10, 10, 0xB8BEC8);
    zeigen(rauch_[k], false);
  }
  // Wäschekorb vor dem Bauch (mit bunter Wäsche), Mülltonne rechts daneben, Kissen links am Kopf
  korb_ = form(koerper_, 46, 118, 88, 40, 0, 0);
  lv_obj_set_style_bg_opa(korb_, LV_OPA_TRANSP, 0);
  form(korb_, 6, 0, 22, 18, 0x4AA8FF);
  form(korb_, 30, 2, 24, 16, 0xFF8FA3);
  form(korb_, 56, 0, 24, 18, 0x5AE07A);
  lv_obj_t *kb = form(korb_, 0, 12, 88, 28, 0xC8A06A, 8);
  for (int k = 0; k < 4; k++) form(kb, 10 + k * 20, 4, 4, 20, 0x9A7444, 2);
  tonne_ = form(koerper_, 176, 92, 44, 66, 0, 0);
  lv_obj_set_style_bg_opa(tonne_, LV_OPA_TRANSP, 0);
  form(tonne_, 0, 0, 44, 10, 0x3A3F4A, 3);
  form(tonne_, 3, 9, 38, 50, 0x5A606E, 4);
  form(tonne_, 6, 56, 10, 10, 0x1A1A22);
  form(tonne_, 28, 56, 10, 10, 0x1A1A22);
  kissen_ = form(koerper_, -44, 52, 64, 36, 0xBFD8FF, 14);
  form(kissen_, 10, 8, 44, 4, 0x9FC0F0, 2);
  requisit_layout();

  if (art_ == ROBOTER) {  // Hände an den Armen (vor Kopf und Körper, damit sie das Schild halten)
    for (int i = 0; i < 2; i++) robo_hand_[i] = form(koerper_, 0, 0, 16, 16, 0x6A7386);
  } else if (art_ == FUCHS) {  // Vorderpfoten fürs Schild
    for (int i = 0; i < 2; i++) {
      arm_[i] = form(koerper_, 0, 0, 26, 18, FUCHS_BRAUN);
      form(arm_[i], 9, 4, 8, 6, 0x6A4430, 3);  // Ballen
    }
  }

  // Kürbis (Halloween)
  stiel_ = form(koerper_, p.hut_x + 29, p.hut_y - 9, 8, 12, 0x3D7A2A, 3);
  kuerbis_ = form(koerper_, p.hut_x, p.hut_y, 66, 30, 0xF28C28);
  rand(kuerbis_, 2, 0xA84E10);
  form(kuerbis_, 24, 2, 14, 22, 0xE07A1C);
  hut_glyph_ = lv_label_create(koerper_);
  if (deko_font_) lv_obj_set_style_text_font(hut_glyph_, deko_font_, 0);
  lv_obj_set_pos(hut_glyph_, p.hut_x + 18, p.hut_y + (art_ == OKTI ? 10 : -4));  // Okti: Kopf reicht bis oben
  // Hut/Krone/Stern 1,8-fach: 30-px-Symbol war auf dem Kopf zu winzig; wächst von der Unterkante nach oben
  lv_obj_set_style_transform_pivot_x(hut_glyph_, 15, 0);
  lv_obj_set_style_transform_pivot_y(hut_glyph_, 30, 0);
  lv_obj_set_style_transform_scale(hut_glyph_, 460, 0);
  lv_label_set_text(hut_glyph_, "");
  // Weihnachtsmütze: rot, weiße Krempe, Bommel
  muetze_ = form(koerper_, p.hut_x + 4, p.hut_y - 6, 58, 30, ROT, 14);
  form(muetze_, 40, -6, 14, 14, 0xF4F4F4);
  lv_obj_add_flag(muetze_, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
  lv_obj_t *krempe = form(koerper_, p.hut_x - 2, p.hut_y + 20, 70, 11, 0xF4F4F4, 5);
  lv_obj_set_parent(krempe, muetze_);
  lv_obj_set_pos(krempe, -6, 26);
  hut();

  // Geschlossener Briefumschlag mit Siegel, solange Post wartet (ein Arm hält ihn)
  brief_ = form(koerper_, BRIEF_X - 23, BRIEF_Y - 16, 46, 32, 0xF4E4C1, 4);
  rand(brief_, 2, 0x8B6B3E);
  lv_obj_add_flag(brief_, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
  brief_klappe_obj_ = linie(brief_, 2, 0x8B6B3E);
  lv_obj_set_size(brief_klappe_obj_, 46, 50);
  lv_obj_set_pos(brief_klappe_obj_, 0, -14);
  klappe(false);
  brief_siegel_ = form(brief_, 17, 10, 9, 9, ROT);
  brief_layout();

  // Schmuck: Kette als flacher Bogen dort, wo Kopf und Körper sich treffen; Lage je Figur (Mittelpunkt y, Radius)
  {
    int my = art_ == OKTI ? 70 : art_ == FUCHS ? 80 : 90, r = art_ == OKTI ? 60 : art_ == FUCHS ? 50 : 40;
    schmuck_ = form(koerper_, 0, 0, 180, 170, 0, 0);
    transparent(schmuck_);
    if (art_ == FUCHS) {  // Fuchs: Altgold-Kette auf der hellen Brust
      lv_obj_t *kette = bogen(schmuck_, 2 * r, 5, 0xB8860B);
      lv_arc_set_bg_angles(kette, 52, 128);
      lv_obj_set_pos(kette, 90 - r, my - r);
    } else {  // Okti/Roboter: auf Gelb sieht man Gold kaum → Perlenkette
      for (int k = 0; k < 9; k++) {
        float w = (52 + k * 9.5f) * 0.017453f;
        lv_obj_t *perle = form(schmuck_, 90 + (int) (r * cosf(w)) - 4, my + (int) (r * sinf(w)) - 4, 9, 9, 0xF4F4F8);
        rand(perle, 1, 0xC8C8D8);
      }
    }
    lv_obj_t *anh = form(schmuck_, 81, my + r - 4, 18, 20, 0xFF3A8C, 9);  // Anhänger: pinker Edelstein in Gold
    rand(anh, 3, 0xB8860B);
    schmuck_glanz_ = form(anh, 3, 3, 5, 5, 0xFFFFFF);
    ohrstecker_[0] = ohrstecker_[1] = nullptr;
    if (art_ == FUCHS)
      for (int i = 0; i < 2; i++) {
        ohrstecker_[i] = form(koerper_, i ? 144 : 26, 22, 11, 11, 0x7FE8FF);  // funkelnder Stein
        rand(ohrstecker_[i], 2, 0xB8860B);
      }
    schmuck(schmuck_an_);
  }

  zzz_ = lv_label_create(koerper_);
  lv_obj_set_style_text_color(zzz_, lv_color_hex(0x9FC4FF), 0);
  lv_label_set_text(zzz_, "z Z");
  lv_obj_set_pos(zzz_, p.zzz_x, p.zzz_y);

  // Effekt-Bausteine (alle Figuren): Schweißtropfen an der Stirn, Träne unter dem linken Auge,
  // Erröten = drei Striche je Wange, Funkeln = vier Sternchen um den Kopf
  schweiss_ = form(gesicht_, 0, 0, 9, 13, 0x8FD3FF, 5);
  traene_ = form(gesicht_, 0, 0, 9, 13, 0x8FD3FF, 5);
  for (int i = 0; i < 2; i++)
    for (int k = 0; k < 3; k++) {
      roet_[i][k] = form(wange_[i], 2 + k * p.wange_b / 3, 1, 2, std::max(4, p.wange_h - 2), 0xE0405A, 1);
    }
  static const int FUNKEL[4][2] = {{-4, 6}, {176, 16}, {6, 96}, {168, 92}};
  for (int i = 0; i < 4; i++) {
    funkel_[i] = form(koerper_, FUNKEL[i][0], FUNKEL[i][1], 14, 14, 0, 0);
    transparent(funkel_[i]);
    form(funkel_[i], 6, 0, 2, 14, 0xFFF3A0, 1);
    form(funkel_[i], 0, 6, 14, 2, 0xFFF3A0, 1);
  }
  effekt_tick(esphome::millis());

  if (verreist_) zeigen(koerper_, false);
  standby_ = nullptr;
  farb_st_ = -1;
  ausdruck_setzen(basis_);
  koerper_layout();
  gesicht_layout();
  arme_layout();
  schild_layout();
  tentakel_zeichnen();
  if (schlaeft_) schlaf_szene();
}

inline void Figur::farbe(int st) {
  if (art_ == FUCHS && st != farb_st_) {  // Fuchs: legt bei schlechter Laune die Ohren an (Spitzen nach außen unten)
    farb_st_ = st;
    bool an = st >= 70;
    for (int i = 0; i < 2; i++) {
      int s = i ? -1 : 1;
      for (int j = 0; j < 2; j++)
        for (int k = 0; k < 5; k++) {
          ohr_pkt_[i][j][k][0].x = OHR_SPITZE[i][j][0] - (an ? 16 * s : 0) + ARM_DX;
          ohr_pkt_[i][j][k][0].y = OHR_SPITZE[i][j][1] + (an ? 22 : 0) + ARM_DY;
        }
    }
    if (koerper_) lv_obj_invalidate(koerper_);
    return;
  }
  if (art_ == ROBOTER && visier_ && st != farb_st_) {  // Roboter: Visier glüht bei schlechter Laune rötlich
    farb_st_ = st;
    lv_obj_set_style_bg_color(visier_, lv_color_mix(lv_color_hex(0x4A1626), lv_color_hex(VISIER), (lv_opa_t) (st * 255 / 100)), 0);
    return;
  }
  if (art_ != OKTI || tentakel_[0] == nullptr || st == farb_st_) return;
  bool wende = farb_st_ >= 0 && abs(st - farb_st_) >= 40;
  farb_st_ = st;
  lv_opa_t m = (lv_opa_t) (st * 255 / 100);
  lv_color_t oben = lv_color_mix(lv_color_hex(ROT), lv_color_hex(rosa_ ? 0xFF9EC0 : GELB), m);  // Valentinstag: rosa
  lv_color_t unten = lv_color_mix(lv_color_hex(GELB), lv_color_hex(ROT), m);
  lv_obj_set_style_bg_color(kopf_, oben, 0);
  for (int i = 0; i < ARME; i++) {
    lv_obj_set_style_line_color(tentakel_[i], oben, 0);
    lv_obj_set_style_line_color(unterseite_[i], unten, 0);
  }
  if (wende) anim(cb_hop, 0, 16, 140, 160, 1, lv_anim_path_ease_out);  // Wende-Hüpfer
}

inline void Figur::ausdruck_setzen(Ausdruck a) {
  ausdruck_ = a;
  if (koerper_ == nullptr) return;
  const Bauplan &p = *plan_;
  int w = 24, h = 40, r = LV_RADIUS_CIRCLE;
  switch (a) {
    case UEBERRASCHT: w = 32; h = 42; break;
    case MUEDE: w = 26; h = 12; r = 6; break;
    case GENERVT: w = 26; h = 20; r = 5; break;
    case TRAURIG: w = 22; h = 34; break;
    case SCHLAF: w = 26; h = 4; r = 2; break;
    case GAEHNEN: w = 26; h = 6; r = 3; break;
    default: break;
  }
  augen_w_ = (int) lroundf(w * p.auge_skala);
  augen_h_ = std::max(3, (int) lroundf(h * p.auge_skala));
  int st = (a == GENERVT || a == TRAURIG) ? 100 : std::max(0, std::min(100, stimmung_ + wetter_laune_));
  farbe(st);
  bool grummelig = st >= 70;
  bool mies = a == NORMAL && st >= 35 && st < 70;  // dazwischen: Mund gerade
  for (int i = 0; i < 2; i++) {
    lv_obj_set_style_radius(auge_[i], r, 0);
    zeigen(auge_[i], augen_rund());
    zeigen(froh_[i], a == FROH);
    zeigen(spirale_[i], a == SCHWINDLIG);
    zeigen(braue_[i], grummelig && a != FROH && a != SCHLAF && a != TRAURIG);  // traurig ohne böse Brauen
    lv_obj_set_style_bg_opa(wange_[i], a == FROH ? 220 : (lv_opa_t) (120 * (100 - st) / 100), 0);
  }
  if (a == SCHWINDLIG)
    anim(cb_spirale, 0, 360, 700, 0, LV_ANIM_REPEAT_INFINITE, lv_anim_path_linear);
  else
    lv_anim_delete(this, cb_spirale);
  zeigen(zzz_, a == SCHLAF);
  if (a == SCHLAF)
    anim(cb_zzz, 0, 10, 1400, 1400, LV_ANIM_REPEAT_INFINITE);
  else
    lv_anim_delete(this, cb_zzz);

  // Mund
  int mb = p.mund_bogen;
  bool sauer = grummelig && (a == NORMAL || a == GENERVT || a == TRAURIG);
  zeigen(mund_bogen_, (a == NORMAL && !mies) || a == FROH || sauer);
  zeigen(mund_o_, a == UEBERRASCHT || a == SCHWINDLIG || a == GAEHNEN || a == SCHLAF);
  zeigen(mund_strich_, a == MUEDE || mies);
  if (sauer) {  // Mundwinkel runter
    lv_arc_set_bg_angles(mund_bogen_, 220, 320);
    lv_obj_set_pos(mund_bogen_, p.mund_x - mb / 2, p.mund_y - mb / 20);
  } else if (a == NORMAL) {
    lv_arc_set_bg_angles(mund_bogen_, 45, 135);
    lv_obj_set_pos(mund_bogen_, p.mund_x - mb / 2, p.mund_y - mb * 7 / 10);
  } else if (a == FROH) {
    lv_arc_set_bg_angles(mund_bogen_, 20, 160);
    lv_obj_set_pos(mund_bogen_, p.mund_x - mb / 2, p.mund_y - mb * 3 / 4);
  }
  int ow = 14, oh = 16;
  if (a == GAEHNEN) { ow = 20; oh = 26; }
  if (a == SCHWINDLIG) { ow = 10; oh = 12; }
  if (a == SCHLAF) { ow = 8; oh = 8; }
  ow = (int) lroundf(ow * p.auge_skala);
  oh = (int) lroundf(oh * p.auge_skala);
  lv_obj_set_size(mund_o_, ow, oh);
  lv_obj_set_pos(mund_o_, p.mund_x - ow / 2, p.mund_y - oh / 2);

  blinzel_ = 0;
  augen_layout();
}

inline void Figur::tentakel_tick(uint32_t jetzt) {
  if (art_ == ROBOTER && herz_) {  // Herzlicht pulsiert, Antenne blinkt
    lv_obj_set_style_bg_opa(herz_, (lv_opa_t) (150 + 105 * sinf(jetzt / 350.0f)), 0);
    if (antenne_) lv_obj_set_style_bg_color(antenne_, lv_color_hex((jetzt / 900) % 2 ? 0xFF5A7A : LEUCHT), 0);
  }
  bool zappeln = !vorbei(jetzt, zappeln_bis_) || streichel_aktiv_;
  bool aktiv = zappeln || greif_arm_ >= 0 || (winken_bis_ && !vorbei(jetzt, winken_bis_)) ||
               (arm_pos_ > 0 && arm_pos_ < 100) || art_ == ROBOTER;
  if (!aktiv && jetzt - letzte_tentakel_ < 60) return;
  float dt = std::min(0.2f, (jetzt - letzte_tentakel_) / 1000.0f);
  letzte_tentakel_ = jetzt;
  phase_ += dt * (zappeln ? 5.0f : 0.7f) * (0.6f + anspannung_ / 60.0f) * (0.6f + energie_ / 250.0f);
  if (phase_ > 1000.0f) phase_ -= 1000.0f;
  tentakel_zeichnen();
  roboter_arme();
  fuchs_schwanz();
  if (art_ == FUCHS) arme_layout();
}

inline void Figur::roboter_arme() {
  if (art_ != ROBOTER || robo_arm_[0] == nullptr) return;
  uint32_t jetzt = esphome::millis();
  bool zappeln = !vorbei(jetzt, zappeln_bis_) || streichel_aktiv_;
  bool wink = winken_bis_ && !vorbei(jetzt, winken_bis_);
  float h = arm_pos_ / 100.0f;
  static const float SCHULTER[2][2] = {{42, 132}, {138, 132}};
  for (int i = 0; i < 2; i++) {
    float o = i ? 60 : 120, u = i ? 80 : 100;          // Ruhe: hängen schräg nach außen
    float oz = i ? -55 : 235, uz = i ? -85 : 265;       // Schild: nach oben zu den Ecken
    o += (oz - o) * h;
    u += (uz - u) * h;
    float l1 = 32, l2 = 30;
    if (zappeln) {
      o += 18 * sinf(phase_ * 2 + i);
      u += 25 * sinf(phase_ * 3 + i);
    }
    if (welk_bis_ && !vorbei(jetzt, welk_bis_)) {  // welk: Arme hängen schlaff senkrecht
      o = u = 90 + (i ? -4 : 4);
    }
    if (wetter_wind_) {  // Wind weht die Arme nach rechts
      o -= 22 + 6 * sinf(phase_ * 3 + i);
      u -= 30 + 8 * sinf(phase_ * 3.5f + i);
    }
    if (wink && i == 1 && h <= 0) {
      o = -70 + 12 * sinf(phase_ * 4);
      u = -110 + 30 * sinf(phase_ * 6);
    }
    int gx = greif_x_, gy = greif_y_;
    bool haelt = i != greif_arm_ && h <= 0 && !(wink && i == 1) && griff(i, gx, gy);  // Hand hält das Requisit
    if (i == greif_arm_ || haelt) {  // Greifen: beide Glieder Richtung Ziel, gestreckt
      float dx = gx - SCHULTER[i][0], dy = gy - SCHULTER[i][1];
      o = u = atan2f(dy, dx) * 57.2958f;
      l1 = l2 = std::min(48.0f, std::max(26.0f, sqrtf(dx * dx + dy * dy) / 2));
    }
    float x0 = SCHULTER[i][0], y0 = SCHULTER[i][1];
    float x1 = x0 + l1 * cosf(o * 0.017453f), y1 = y0 + l1 * sinf(o * 0.017453f);
    float x2 = x1 + l2 * cosf(u * 0.017453f), y2 = y1 + l2 * sinf(u * 0.017453f);
    robo_pkt_[i][0].x = x0 + ARM_DX;
    robo_pkt_[i][0].y = y0 + ARM_DY;
    robo_pkt_[i][1].x = lroundf(x1) + ARM_DX;
    robo_pkt_[i][1].y = lroundf(y1) + ARM_DY;
    robo_pkt_[i][2].x = lroundf(x2) + ARM_DX;
    robo_pkt_[i][2].y = lroundf(y2) + ARM_DY;
    lv_line_set_points(robo_arm_[i], robo_pkt_[i], 3);
    if (robo_hand_[i]) lv_obj_set_pos(robo_hand_[i], (int) x2 - 8, (int) y2 - 8);
  }
}

inline void Figur::fuchs_schwanz() {
  if (art_ != FUCHS || schwanz_ == nullptr) return;
  uint32_t jetzt = esphome::millis();
  bool doll = !vorbei(jetzt, zappeln_bis_) || streichel_aktiv_ || ausdruck_ == FROH;
  float amp = doll ? 22.0f : 7.0f;
  float x = 146, y = 150, w = 5;
  for (int k = 0; k <= 4; k++) {
    if (k > 0) {
      w = 5 - 19 * k + amp * sinf(phase_ * (doll ? 3.0f : 1.0f)) * k / 4;  // schwingt nach rechts oben
      if (wetter_wind_) w += (22 + 8 * sinf(phase_ * 3)) * k / 4;  // Wind biegt den Schwanz nach rechts
      if (welk_bis_ && !vorbei(jetzt, welk_bis_)) w = 30 + 12 * k;  // welk: Schwanz hängt nach unten
      if (schlaeft_) w = 165 + 14 * k;  // schläft: Schwanz vorn um den Körper gelegt
      if (faecher_schwanz_bis_ && !vorbei(jetzt, faecher_schwanz_bis_))  // fächelt vor dem Gesicht
        w = -100 - 22 * k + 35 * sinf(jetzt / 70.0f) * k / 4;
      x += 20 * cosf(w * 0.017453f);
      y += 20 * sinf(w * 0.017453f);
    }
    schwanz_pkt_[k].x = lroundf(x) + ARM_DX;
    schwanz_pkt_[k].y = lroundf(y) + ARM_DY;
  }
  lv_line_set_points(schwanz_, schwanz_pkt_, 5);
  lv_line_set_points(schwanz_rand_, &schwanz_pkt_[1], 3);
  lv_line_set_points(schwanz_bauch_, &schwanz_pkt_[1], 3);
  lv_line_set_points(schwanz_spitze_, &schwanz_pkt_[3], 2);
  // leicht zulaufend: kurze, schmalere Verlängerung über den letzten Punkt hinaus
  float dx = schwanz_pkt_[4].x - schwanz_pkt_[3].x, dy = schwanz_pkt_[4].y - schwanz_pkt_[3].y;
  float l = std::max(1.0f, sqrtf(dx * dx + dy * dy));
  spitze_pkt_[0][0] = schwanz_pkt_[4];
  spitze_pkt_[0][1].x = lroundf(schwanz_pkt_[4].x + dx / l * 11);
  spitze_pkt_[0][1].y = lroundf(schwanz_pkt_[4].y + dy / l * 11);
  if (spitze_[0]) lv_line_set_points(spitze_[0], spitze_pkt_[0], 2);
}

inline void Figur::tentakel_zeichnen() {
  if (art_ != OKTI || tentakel_[0] == nullptr) return;
  uint32_t jetzt = esphome::millis();
  bool zappeln = !vorbei(jetzt, zappeln_bis_) || streichel_aktiv_;
  bool wink = winken_bis_ && !vorbei(jetzt, winken_bis_);
  float amp = zappeln ? 15.0f : 4.5f;
  float halten = arm_pos_ / 100.0f;
  // Der auf sich zeigende Arm muss vor den Kopf, sonst verschwindet er dahinter
  bool zeigt = !post_.empty() && halten <= 0 && greif_arm_ != 0;
  if (zeigt != arm0_vorn_) {
    arm0_vorn_ = zeigt;
    if (zeigt) {
      lv_obj_move_foreground(unterseite_[0]);
      lv_obj_move_foreground(tentakel_[0]);
    } else {
      lv_obj_move_to_index(unterseite_[0], 0);
      lv_obj_move_to_index(tentakel_[0], ARME);
    }
  }
  bool welk = welk_bis_ && !vorbei(jetzt, welk_bis_);
  for (int i = 0; i < ARME; i++) {
    float w = ARM_WINKEL[i];
    float kruemmung = i < ARME / 2 ? 11.0f : -11.0f;  // Spitzen rollen sich nach oben ein
    float wa = amp;
    if (welk) {  // schlaff: alle Arme hängen fast senkrecht, Spitzen nach innen
      w = 90 + (w - 90) * 0.35f;
      kruemmung = i < ARME / 2 ? -4.0f : 4.0f;
      wa = 1.5f;
    }
    if (wetter_wind_ && !welk) w -= 28.0f + 10.0f * sinf(phase_ * 2.5f + i * 0.4f);  // Wind weht die Arme nach rechts
    if (knoten_bis_ && !vorbei(jetzt, knoten_bis_)) {  // verknotet: Arme kreuzen zur Gegenseite und verdrehen sich
      w = 180 - w + 8 * sinf(phase_ * 6 + i);
      kruemmung = (i % 2 ? 1 : -1) * 22.0f;
      wa = 2.0f;
    }
    // Schild halten: die äußeren Arme greifen nach oben zu den Schildecken
    if ((i == 0 || i == ARME - 1) && halten > 0) {
      float ziel = i == 0 ? 248.0f : -68.0f;
      w = w + (ziel - w) * halten;
      kruemmung = kruemmung * (1 - halten) + (i == 0 ? 6.0f : -6.0f) * halten;
      wa = amp * (1 - halten) + 3.0f * halten;
    }
    // Winken: der rechte äußere Arm hoch und hin und her
    if (wink && i == ARME - 1 && halten <= 0) {
      w = -75.0f + 22.0f * sinf(phase_ * 4.0f);
      kruemmung = -6.0f;
      wa = 4.0f;
    }
    float seg = ARM_LAENGE[i] / (float) GLIEDER;
    // Post wartet: der rechte äußere Arm hält den Umschlag (wenn er nicht gerade winkt oder das Schild hält)
    if (i == ARME - 1 && !post_.empty() && !wink && halten <= 0 && i != greif_arm_) {
      float dx = BRIEF_X - 14 - ARM_X[i], dy = BRIEF_Y + 14 - ARM_Y;
      w = atan2f(dy, dx) * 57.2958f;
      kruemmung = 0;
      wa = 2.0f;
      seg = sqrtf(dx * dx + dy * dy) / GLIEDER;
    }
    // Post wartet: der linke äußere Arm zeigt auf Yellow selbst („tipp mich an“)
    if (i == 0 && zeigt) {  // vom Ansatz hoch zur linken Wange, Spitze zeigt auf Yellow
      float dx = 50 - ARM_X[i], dy = 74 - ARM_Y;
      w = atan2f(dy, dx) * 57.2958f - 50;
      kruemmung = 20.0f;
      wa = 3.0f;
      seg = sqrtf(dx * dx + dy * dy) * 1.3f / GLIEDER;
    }
    // Greifen: Arm zeigt zum Finger und streckt sich (höchstens 130 px)
    if (i == greif_arm_) {
      float dx = greif_x_ - ARM_X[i], dy = greif_y_ - ARM_Y;
      w = atan2f(dy, dx) * 57.2958f;
      kruemmung = 0;
      wa = 2.0f;
      seg = std::max(seg, std::min(sqrtf(dx * dx + dy * dy), 130.0f) / GLIEDER);
    }
    float x = ARM_X[i], y = ARM_Y;
    arm_pkt_[i][0].x = x + ARM_DX;
    arm_pkt_[i][0].y = y + ARM_DY;
    unten_pkt_[i][0].x = x + ARM_DX;
    unten_pkt_[i][0].y = y + 3 + ARM_DY;
    // Punkte im sichtbaren Bereich halten (Arme enden am Rand statt darüber hinaus)
    float x_min = 10 - plan_->kx, x_max = 310 - plan_->kx, y_min = 26 - plan_->ky, y_max = 216 - plan_->ky;
    for (int k = 1; k <= GLIEDER; k++) {
      float a = (w + kruemmung * k + wa * sinf(phase_ + k * 0.9f + i * 0.7f) * k / GLIEDER) * 0.017453f;
      x = std::max(x_min, std::min(x_max, x + seg * cosf(a)));
      y = std::max(y_min, std::min(y_max, y + seg * sinf(a)));
      arm_pkt_[i][k].x = lroundf(x) + ARM_DX;
      arm_pkt_[i][k].y = lroundf(y) + ARM_DY;
      unten_pkt_[i][k].x = lroundf(x) + ARM_DX;
      unten_pkt_[i][k].y = lroundf(y + 3) + ARM_DY;
    }
    lv_line_set_points(unterseite_[i], unten_pkt_[i], GLIEDER + 1);
    lv_line_set_points(tentakel_[i], arm_pkt_[i], GLIEDER + 1);
    for (int k = 0; k < 3; k++) {  // Saugnäpfe auf Glied 2–4, leicht zur Unterseite
      lv_obj_t *s = saugnapf_[i][k];
      if (s) lv_obj_set_pos(s, (int) arm_pkt_[i][k + 2].x - ARM_DX - 2, (int) arm_pkt_[i][k + 2].y - ARM_DY + 1);
    }
  }
}

inline void Figur::augen_layout() {
  if (koerper_ == nullptr) return;
  for (int i = 0; i < 2; i++) {
    int h = std::max(3, augen_h_ * (100 - blinzel_) / 100);
    lv_obj_set_size(auge_[i], augen_w_, h);
    lv_obj_set_pos(auge_[i], plan_->auge_x[i] - augen_w_ / 2, plan_->auge_y - h / 2);
    zeigen(glanz_[i], h >= 14);
  }
}

inline void Figur::koerper_layout() {
  lv_obj_set_pos(koerper_, plan_->kx + wackel_ + besuch_x_, plan_->ky - hop_ - atem_ + maus_y_);
  int w = std::max(60, plan_->schatten_b - hop_ * 2);
  lv_obj_set_size(schatten_, w, 12);
  lv_obj_set_pos(schatten_, 160 - w / 2 + wackel_ + besuch_x_, plan_->schatten_y);
}

inline void Figur::arme_layout() {
  if (art_ == OKTI) {  // Okti: Schild-Arme bewegen sich mit arm_pos_
    tentakel_zeichnen();
    return;
  }
  if (art_ == ROBOTER) {
    roboter_arme();
    return;
  }
  if (arm_[0] == nullptr) return;  // Fuchs: 0 = Pfoten vorn, 100 = halten oben das Schild
  int a = arm_pos_;
  uint32_t jetzt = esphome::millis();
  bool wink = winken_bis_ && !vorbei(jetzt, winken_bis_);
  bool zappeln = !vorbei(jetzt, zappeln_bis_) || streichel_aktiv_;
  int gx, gy;
  for (int i = 0; i < 2; i++) {
    const int *u = plan_->arm_unten[i], *o = plan_->arm_oben[i];
    int x = u[0] + (o[0] - u[0]) * a / 100, y = u[1] + (o[1] - u[1]) * a / 100;
    if (a <= 0) {
      if (wink && i == 1) {  // rechte Pfote winkt neben dem Kopf
        x = 156 + (int) (6 * sinf(phase_ * 4));
        y = 74 + (int) (8 * sinf(phase_ * 6));
      } else if (i != greif_arm_ && griff(i, gx, gy)) {  // Pfote hält das Requisit
        x = gx - 13;
        y = gy - 9;
      } else if (i == greif_arm_) {  // Pfote tapst Richtung Finger, höchstens 40 px
        float dx = greif_x_ - (x + 11), dy = greif_y_ - (y + 7), d = sqrtf(dx * dx + dy * dy);
        float k = d > 40 ? 40 / d : 1;
        x += (int) (dx * k);
        y += (int) (dy * k);
      } else if (zappeln) {
        y += (int) (4 * sinf(phase_ * 3 + i));
      }
    }
    lv_obj_set_pos(arm_[i], x, y);
  }
}

}  // namespace yellow
