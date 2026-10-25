#pragma once
// Yellow: Spiele (Reaktion, Tic-Tac-Toe, Simon) und Fragen mit Antwortfeldern (Methoden von Figur, siehe yellow.h)
#include "yellow.h"

namespace yellow {

inline bool Figur::spiel(const std::string &name) {
  if (!sichtbar_ || spiel_ != KEIN_SPIEL) return false;
  uint32_t jetzt = esphome::millis();
  if (name == "reaktion") {
    spiel_ = REAKTION;
    reaktion_los_ = 0;
    reaktion_bereit_ = jetzt + 3500 + zufall(3500);
    zeitweise(NORMAL, 100);
    zeige_schild("Wenn ich springe: tippen!", 3);
    return true;
  }
  if (name == "simon") {
    spiel_ = SIMON;
    simon_len_ = 1;
    for (auto &f : simon_folge_) f = (int) zufall(4);
    simon_zeigen_ab(jetzt + 1200);
    for (int i = 0; i < 4; i++) simon_licht(i, false);
    lv_obj_move_foreground(simon_box_);
    zeigen(simon_box_, true);
    zeige_schild("Mach's mir nach.", 2);
    return true;
  }
  if (name == "ttt") {
    spiel_ = TTT;
    for (auto &f : ttt_) f = 0;
    ttt_yellow_bei_ = 0;
    ttt_ende_bei_ = 0;
    letzter_zug_ = jetzt;
    for (int i = 0; i < 9; i++) lv_label_set_text(ttt_lbl_[i], "");
    lv_obj_move_foreground(spiel_box_);
    zeigen(spiel_box_, true);
    zeige_schild("Du fängst an.", 2);
    return true;
  }
  return false;
}

inline void Figur::frage(const std::string &text, const std::string &optionen) {
  frage_text_ = text;
  size_t t = optionen.find('|');
  frage_opt_[0] = t == std::string::npos ? optionen : optionen.substr(0, t);
  frage_opt_[1] = t == std::string::npos ? std::string("Nein") : optionen.substr(t + 1);
  if (frage_opt_[0].empty()) frage_opt_[0] = "Ja";
  frage_aktiv_ = true;
  frage_bis_ = esphome::millis() + 600000;
  frage_gezeigt_ = false;
}

inline void Figur::frage_abbrechen() {
  if (!frage_aktiv_) return;
  frage_aktiv_ = false;
  frage_gezeigt_ = false;
  zeigen(frage_box_, false);
  schild_bis_ = esphome::millis() + 10;
}

inline void Figur::spiel_beenden(const std::string &name, int wert) {
  spiel_ = KEIN_SPIEL;
  zahl_[SPIELE]++;
  if (spiel_ende) spiel_ende(name, wert);
}

inline void Figur::ttt_setzen(int i, int wer) {
  ttt_[i] = wer;
  lv_label_set_text(ttt_lbl_[i], wer == 1 ? "X" : wer == 2 ? "O" : "");
  lv_obj_set_style_text_color(ttt_lbl_[i], lv_color_hex(wer == 1 ? 0x2E7DD8 : ROT), 0);
}

inline void Figur::ttt_auswerten(uint32_t jetzt) {
  int g = ttt_gewinner();
  if (!g) return;
  if (g == 1 && zufall(100) < 40) {  // verloren? manchmal schummeln: ein X wird zum O
    for (int i = 0; i < 9; i++)
      if (ttt_[i] == 1) {
        ttt_setzen(i, 2);
        zeige_schild("Hab ich nicht gesehen.", 3);
        zeitweise(FROH, 2500);
        g = ttt_gewinner();
        break;
      }
    if (!g) return;  // weiter geht's
  }
  ttt_ende_bei_ = jetzt + 3500;
  ttt_ergebnis_ = g == 1 ? 1 : g == 2 ? -1 : 0;
  if (g == 1) {
    zeitweise(GENERVT, 3500);
    zeige_schild("Na gut. Du.", 3);
  } else if (g == 2) {
    zeitweise(FROH, 3500);
    anim(cb_hop, 0, 14, 160, 180, 2, lv_anim_path_ease_out);
    zeige_schild("Ha! Gewonnen.", 3);
  } else {
    zeige_schild("Unentschieden.", 3);
  }
}

inline void Figur::simon_licht(int i, bool an) {
  lv_obj_set_style_bg_color(simon_feld_[i], lv_color_hex(an ? SIMON_HELL[i] : SIMON_DUNKEL[i]), 0);
  if (an && led) led(SIMON_HELL[i], 0.6f);
  if (!an && led) led(0, 0);
}

inline void Figur::simon_zeigen_ab(uint32_t t) {
  simon_phase_ = 1;  // zeigen
  simon_idx_ = 0;
  simon_an_ = -1;
  simon_naechst_ = t;
}

inline void Figur::simon_ende(uint32_t jetzt) {
  int runden = simon_len_ - 1;
  zeitweise(runden >= 5 ? FROH : GENERVT, 3000);
  zeige_schild(esphome::str_sprintf("%d Runde%s.", runden, runden == 1 ? "" : "n"), 3);
  simon_phase_ = 3;  // Abschluss
  simon_naechst_ = jetzt + 2500;
}

inline void Figur::spiel_tick(uint32_t jetzt) {
  if (spiel_ == SIMON) {
    if (simon_aus_bei_ && vorbei(jetzt, simon_aus_bei_)) {  // kurzes Aufleuchten nach Tipp
      simon_aus_bei_ = 0;
      for (int i = 0; i < 4; i++) simon_licht(i, false);
    }
    if (simon_phase_ == 1 && vorbei(jetzt, simon_naechst_)) {
      if (simon_an_ >= 0) {
        simon_licht(simon_an_, false);
        simon_an_ = -1;
        simon_idx_++;
        simon_naechst_ = jetzt + 180;
      } else if (simon_idx_ < simon_len_) {
        simon_an_ = simon_folge_[simon_idx_];
        simon_licht(simon_an_, true);
        simon_naechst_ = jetzt + std::max(220, 480 - simon_len_ * 15);
      } else {
        simon_phase_ = 2;  // eingeben
        simon_pos_ = 0;
        simon_naechst_ = jetzt + 8000;
      }
    } else if (simon_phase_ == 2 && vorbei(jetzt, simon_naechst_)) {
      simon_ende(jetzt);
    } else if (simon_phase_ == 3 && vorbei(jetzt, simon_naechst_)) {
      zeigen(simon_box_, false);
      spiel_beenden("simon", simon_len_ - 1);
    }
    return;
  }
  if (spiel_ == REAKTION) {
    if (!reaktion_los_ && vorbei(jetzt, reaktion_bereit_)) {
      reaktion_los_ = jetzt;
      zeitweise(UEBERRASCHT, 2000);
      anim(cb_hop, 0, 30, 140, 200, 1, lv_anim_path_ease_out);
      zappeln_bis_ = jetzt + 800;
      zeige_schild("JETZT!", 2);
      if (led) led(0x5AE07A, 0.8f);
      led_aus_bei_ = jetzt + 800;
    } else if (reaktion_los_ && jetzt - reaktion_los_ > 5000) {
      zeige_schild("Eingeschlafen?", 3);
      spiel_beenden("reaktion", -2);
    }
  } else if (spiel_ == TTT) {
    if (!ttt_ende_bei_ && jetzt - letzter_zug_ > 120000) {  // 2 min kein Zug: aufgeben
      zeigen(spiel_box_, false);
      zeige_schild("Keine Lust mehr?", 3);
      spiel_beenden("ttt", -2);
      return;
    }
    if (ttt_yellow_bei_ && vorbei(jetzt, ttt_yellow_bei_)) {
      ttt_yellow_bei_ = 0;
      int z = ttt_zug();
      if (z >= 0) ttt_setzen(z, 2);
      ttt_auswerten(jetzt);
    }
    if (ttt_ende_bei_ && vorbei(jetzt, ttt_ende_bei_)) {
      ttt_ende_bei_ = 0;
      zeigen(spiel_box_, false);
      spiel_beenden("ttt", ttt_ergebnis_);
    }
  }
}

inline void Figur::spiel_tipp(lv_point_t p, uint32_t jetzt) {
  if (spiel_ == SIMON) {
    if (simon_phase_ != 2) return;
    int i = (p.x >= 160 ? 1 : 0) + (p.y >= 120 ? 2 : 0);
    for (int k = 0; k < 4; k++) simon_licht(k, k == i);
    simon_aus_bei_ = jetzt + 220;
    if (i != simon_folge_[simon_pos_]) {
      simon_ende(jetzt);
      return;
    }
    simon_naechst_ = jetzt + 8000;
    if (++simon_pos_ == simon_len_) {
      if (simon_len_ < 32) simon_len_++;
      simon_zeigen_ab(jetzt + 900);
    }
    return;
  }
  if (spiel_ == REAKTION) {
    if (!reaktion_los_) {
      zeitweise(GENERVT, 2500);
      zeige_schild("Zu früh!", 3);
      spiel_beenden("reaktion", -2);
    } else {
      int ms = (int) (jetzt - reaktion_los_);
      zeitweise(ms < 300 ? FROH : NORMAL, 3000);
      zeige_schild(esphome::str_sprintf("%d ms", ms), 4);
      spiel_beenden("reaktion", ms);
    }
  } else if (spiel_ == TTT) {
    if (ttt_yellow_bei_ || ttt_ende_bei_) return;
    if (p.x < 70 || p.y < 30) return;
    int c = ((int) p.x - 70) / 62, r = ((int) p.y - 30) / 62;
    if (c > 2 || r > 2) return;
    int i = r * 3 + c;
    if (ttt_[i]) return;
    ttt_setzen(i, 1);
    letzter_zug_ = jetzt;
    ttt_auswerten(jetzt);
    if (!ttt_ende_bei_ && !ttt_gewinner()) ttt_yellow_bei_ = jetzt + 700;
  }
}

inline void Figur::frage_beenden(const std::string &a) {
  frage_aktiv_ = false;
  frage_gezeigt_ = false;
  zeigen(frage_box_, false);
  schild_bis_ = esphome::millis() + 10;  // Schild weg (über tick)
  if (antwort) antwort(a);
}

}  // namespace yellow
