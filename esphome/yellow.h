#pragma once
// Yellow: kleine stumme Figur fürs CYD (LVGL 9, Querformat 320×240), Plan in docs/yellow.md.
// Baut sich als Kind eines LVGL-Objekts auf (das auch die Touch-Ereignisse bekommt) und bewegt
// sich nur mit lv_anim und Linien-Punkten, ohne Bitmaps. Zwei Figuren: Okti (Wende-Oktopus,
// gelb/froh oder rot/grummelig, 8 Arme) oder Roboter.
// Bausteine: Formen, Bögen, Linien. Alles, was LVGL anfasst, erst nach aufbauen() (siehe bereit()).
// Das Schild (Objekt + Label mit eigener Schrift) kommt aus dem YAML, weil die Schriften dort definiert sind.

#include "yellow_daten.h"

namespace yellow {

class Figur {
 public:
  // LED am Rücken: Farbe 0xRRGGBB, Helligkeit 0..1 (0 = aus)
  std::function<void(uint32_t, float)> led;
  // Nachricht wurde nach Antippen gezeigt (Text), für das Ereignis „gelesen“ in HA
  std::function<void(const std::string &)> gelesen;
  // Yellow wurde angetippt, während ein Timer klingelt (HA stoppt dann Nabus Timer)
  std::function<void()> timer_aus;
  // Yellow wurde durch Antippen geweckt (YAML setzt dann den Schalter „Yellow schläft“ zurück)
  std::function<void()> aufgewacht;
  // Yellow wurde gestreichelt; Text des Schilds, wenn eins in den letzten 15 s zu sehen war (= Lieblingsspruch)
  std::function<void(const std::string &)> gestreichelt;

  // Schmuck (Bling-Bling, kombinierbar mit Hut/Krone): goldene Kette mit funkelndem Anhänger, Fuchs mit Ohrsteckern
  void schmuck(bool an) {
    schmuck_an_ = an;
    if (schmuck_ == nullptr) return;
    zeigen(schmuck_, an);
    for (auto *o : ohrstecker_) zeigen(o, an);
  }
  // Accessoire, wenn kein Anlass-Hut aktiv ist: hut, krone, stern, blume, keins
  void accessoire(const std::string &name) {
    accessoire_ = name == "hut" ? "\U000F0BA4" : name == "krone" ? "\U000F01A5" : name == "stern" ? "\U000F04CE"
                : name == "blume" ? "\U000F024A" : "";
    accessoire_farbe_ = name == "hut" ? 0x2A2A36 : name == "krone" ? 0xFFD23F : name == "stern" ? 0xFFE08A : 0xFF8FA3;
    hut();
  }
  // Antwort auf eine Frage (Text der gewählten Option, leer = keine Antwort in der Zeit)
  std::function<void(const std::string &)> antwort;
  // Spiel zu Ende: spiel (reaktion, ttt), wert (ms bzw. 1 gewonnen / 0 unentschieden / -1 verloren / -2 abgebrochen)
  std::function<void(const std::string &, int)> spiel_ende;

  bool spiel(const std::string &name);

  // Frage mit zwei Antwortfeldern unten (optionen „Ja|Nein“). Wartet, bis Yellow sichtbar ist; nach 10 min ohne Antwort weg.
  void frage(const std::string &text, const std::string &optionen);

  // Timer von Nabu: sek > 0 läuft (Rest in Sekunden), 0 = vorbei/abgebrochen, < 0 = klingelt jetzt
  void timer(int sek, const std::string &name) {
    uint32_t jetzt = esphome::millis();
    timer_name_ = name;
    if (sek > 0) {
      bool neu = timer_ende_ == 0;
      timer_ende_ = jetzt + (uint32_t) sek * 1000;
      timer_klingelt_ = false;
      timer_zahl_ = -1;
      naechste_timer_info_ = jetzt + 90000;
      if (neu && sichtbar_) {
        anim(cb_blick_y, 0, 7, 180, 180, 2);  // nicken
        zeige_schild((name.empty() ? std::string("Timer: ") : name + ": ") + dauer_text(sek), 3);
      }
    } else if (sek == 0) {
      bool klang = timer_klingelt_;
      timer_ende_ = 0;
      timer_klingelt_ = false;
      if (klang && sichtbar_) zeitweise(NORMAL, 100);
    } else {
      timer_ende_ = 0;
      if (!timer_klingelt_) {
        timer_klingelt_ = true;
        klingel_bis_ = jetzt + 120000;
        naechstes_klingeln_ = jetzt;
      }
    }
  }

  void aufbauen(lv_obj_t *flaeche, lv_obj_t *schild, lv_obj_t *schild_text) {
    schild_ = schild;
    schild_text_ = schild_text;
    root_ = form(flaeche, 0, 0, 320, 240, HINTERGRUND, 0);
    lv_obj_move_background(root_);  // Assist-Panel und Schild liegen darüber
    lv_obj_add_flag(root_, LV_OBJ_FLAG_HIDDEN);

    uhr_ = lv_label_create(root_);
    lv_obj_set_pos(uhr_, 258, 8);
    lv_obj_set_style_text_color(uhr_, lv_color_hex(0x5A6274), 0);
    lv_label_set_text(uhr_, "");

    deko_bauen();
    schatten_ = form(root_, 95, 222, 130, 12, SCHATTEN);
    lv_obj_add_event_cb(flaeche, cb_touch, LV_EVENT_ALL, this);
    figur_bauen();
    hilfe_bauen();
    // Mini-Nabu für Besuche: grauer Kopf mit dunklem Visier, zwei Leuchtaugen, Antenne
    nabu_mini_ = form(root_, 330, 150, 52, 64, 0, 0);
    lv_obj_set_style_bg_opa(nabu_mini_, LV_OPA_TRANSP, 0);
    form(nabu_mini_, 24, 0, 4, 12, METALL, 2);
    form(nabu_mini_, 20, -4, 12, 12, 0xFF5A7A);
    lv_obj_t *nk = form(nabu_mini_, 0, 10, 52, 40, 0xC8CCD8, 12);
    lv_obj_t *nv = form(nk, 6, 7, 40, 24, VISIER, 8);
    form(nv, 8, 7, 8, 10, LEUCHT);
    form(nv, 24, 7, 8, 10, LEUCHT);
    form(nabu_mini_, 10, 50, 10, 12, METALL, 3);
    form(nabu_mini_, 32, 50, 10, 12, METALL, 3);
    zeigen(nabu_mini_, false);

    // Antwortfelder für Fragen (unten links/rechts)
    frage_box_ = form(root_, 0, 186, 320, 54, 0, 0);
    lv_obj_set_style_bg_opa(frage_box_, LV_OPA_TRANSP, 0);
    static const uint32_t FARBE[2] = {0x2E9E5A, 0xC8463C};
    for (int i = 0; i < 2; i++) {
      lv_obj_t *b = form(frage_box_, i ? 166 : 10, 2, 144, 46, FARBE[i], 14);
      rand(b, 2, 0xF4E4C1);
      frage_lbl_[i] = lv_label_create(b);
      lv_obj_set_style_text_color(frage_lbl_[i], lv_color_hex(0xFFFFFF), 0);
      lv_obj_center(frage_lbl_[i]);
    }
    zeigen(frage_box_, false);

    // Tic-Tac-Toe: abgedunkelter Hintergrund, 3×3 Felder; Schrift wie auf dem Schild
    spiel_box_ = form(root_, 0, 0, 320, 240, 0x0B0D13, 0);
    lv_obj_set_style_bg_opa(spiel_box_, 200, 0);
    const lv_font_t *gross = lv_obj_get_style_text_font(schild_text_, LV_PART_MAIN);
    for (int i = 0; i < 9; i++) {
      lv_obj_t *z = form(spiel_box_, 70 + (i % 3) * 62, 30 + (i / 3) * 62, 58, 58, 0xF4E4C1, 10);
      ttt_lbl_[i] = lv_label_create(z);
      lv_obj_set_style_text_font(ttt_lbl_[i], gross, 0);
      lv_obj_center(ttt_lbl_[i]);
    }
    zeigen(spiel_box_, false);

    // Simon Says: vier große Farbfelder
    simon_box_ = form(root_, 0, 0, 320, 240, 0x0B0D13, 0);
    lv_obj_set_style_bg_opa(simon_box_, 220, 0);
    for (int i = 0; i < 4; i++) simon_feld_[i] = form(simon_box_, (i % 2) ? 166 : 12, (i / 2) ? 126 : 14, 142, 100, SIMON_DUNKEL[i], 18);
    zeigen(simon_box_, false);
  }

  // Figur wechseln (geht jederzeit, auch vor aufbauen())
  void art(Art a) {
    if (a == art_ && koerper_ != nullptr) return;
    if (root_ == nullptr) {
      art_ = a;
      return;
    }
    bool war = sichtbar_;
    sichtbar(false);  // noch mit der alten Figur: farbe() fasste sonst Okti-Arme an, die es nicht gibt (Absturz 25.10.)
    art_ = a;
    figur_bauen();
    sichtbar(war);
  }

  bool ist_sichtbar() const { return sichtbar_; }
  // Erst nach aufbauen() (on_boot) darf irgendwer LVGL-Objekte anfassen. Selects lösen on_value schon
  // beim Setup aus, vor LVGL: das hängte das Board (Task-WDT, Bootschleife, 24.10.2026).
  bool bereit() const { return root_ != nullptr; }
  // Wischen läuft gerade oder ist eben zu Ende: Tippen im YAML dann ignorieren
  bool streichelt() const {
    return streichel_aktiv_ || (streichel_bis_ && !vorbei(esphome::millis(), streichel_bis_));
  }

  void sichtbar(bool s) {
    if (!bereit() || s == sichtbar_) return;
    sichtbar_ = s;
    lv_anim_delete(this, nullptr);
    blinzel_ = hop_ = atem_ = wackel_ = blick_x_ = blick_y_ = arm_pos_ = besuch_x_ = 0;
    besuch_los_ = besuch_zurueck_ = 0;
    schild_y_ = SCHILD_WEG;
    schild_bis_ = ausdruck_bis_ = nachher_bei_ = winken_bis_ = zappeln_bis_ = 0;
    schilder_.clear();
    naechster_ruf_ = esphome::millis() + 2000;  // wartende Post meldet sich nach dem Erscheinen
    if (spiel_ != KEIN_SPIEL) {
      zeigen(spiel_box_, false);
      zeigen(simon_box_, false);
      spiel_beenden(spiel_ == REAKTION ? "reaktion" : spiel_ == TTT ? "ttt" : "simon", -2);
    }
    frage_gezeigt_ = false;
    zeigen(frage_box_, false);
    streichel_aktiv_ = gedrueckt_ = false;
    greif_arm_ = -1;
    if (s) {
      uint32_t jetzt = esphome::millis();
      lv_obj_clear_flag(root_, LV_OBJ_FLAG_HIDDEN);
      lv_obj_clear_flag(schild_, LV_OBJ_FLAG_HIDDEN);
      atem_starten();
      naechstes_blinzeln_ = jetzt + 1500;
      naechster_blick_ = jetzt + 2500;
      naechste_idee_ = jetzt + 15000;
      letzte_beruehrung_ = jetzt;
      basis_ = NORMAL;
      lw_stufe_ = 0;
      schmollen_ = false;
      if (schlaeft_) {
        schlaf_szene();
      } else {  // kommt herein: Fuchs läuft, Okti krabbelt (Arme zappeln), Roboter rollt
        anim(cb_besuch, -340, 0, art_ == OKTI ? 2600 : 2000, 0, 1, lv_anim_path_ease_out);
        if (art_ == OKTI) zappeln_bis_ = jetzt + 2600;
      }
    } else {
      lv_obj_add_flag(root_, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(schild_, LV_OBJ_FLAG_HIDDEN);
      if (led_aus_bei_ && led) led(0, 0);
      led_aus_bei_ = 0;
    }
    ausdruck_setzen(basis_);
    koerper_layout();
    gesicht_layout();
    arme_layout();
    schild_layout();
    tentakel_zeichnen();
  }

  void uhr(int stunde, int minute) {
    if (uhr_) lv_label_set_text(uhr_, esphome::str_sprintf("%02d:%02d", stunde, minute).c_str());
  }
  // Jahreszeit aus dem Datum (Auto) oder fest per Auswahl „Deko“: 0 Auto, 1 Aus, 2 Halloween, 3 Weihnachten
  void saison(int monat, int tag);
  void deko(int modus);
  // Heutiger Anlass von HA (sensor.yellow_anlass_heute: „ostern“, „geburtstag|Name“, „herbst“ …)
  void anlass(const std::string &a);
  void deko_test(const std::string &key) {  // zum Ausprobieren, leer = wieder normal
    deko_test_ = key;
    deko_anwenden();
  }
  void deko_schrift(const lv_font_t *f) { deko_font_ = f; }
  // Lage wie HA-weather (sunny, rainy, pouring, lightning-rainy, snowy, fog, windy …); Test hat Vorrang, leer = HA
  void wetter(const std::string &lage) { wetter_ha_ = lage; wetter_anwenden(); }
  void wetter_test(const std::string &lage) { wetter_test_ = lage; wetter_anwenden(); }

  // Einzelner Tipp
  void anstupsen() {
    if (!sichtbar_) return;
    uint32_t jetzt = esphome::millis();
    if (abgeholt_bis_ && !vorbei(jetzt, abgeholt_bis_)) return;  // der Tipp hat eine Nachricht abgeholt
    if (verreist_) return;
    if (schlaeft_) {  // Antippen weckt
      schlafen(false);
      if (aufgewacht) aufgewacht();
      return;
    }
    zahl_[TIPPS]++;
    if (schmollen_) {  // Antippen reicht nicht, Streicheln schon
      static const char *const hmpf[] = {"Hmpf.", "Bin beleidigt.", "Jetzt auf einmal?", "Streicheln. Zweimal."};
      zeitweise(GENERVT, 1500);
      anim(cb_blick_x, blick_x_, -12, 200);
      zeige_schild(hmpf[zufall(4)], 2);
      return;
    }
    if (jetzt - stups_start_ > 3000) {
      stups_start_ = jetzt;
      stupser_ = 0;
    }
    ESP_LOGD("yellow", "Tipp %d in %u ms", stupser_ + 1, (unsigned) (jetzt - stups_start_));
    if (++stupser_ >= 4 && art_ == OKTI && zufall(2)) {  // Okti: jedes zweite Mal Tintenwolke statt schwindlig
      stupser_ = 0;
      tinte();
      nachher_text_ = "Bitte nicht.";
      nachher_bei_ = jetzt + 2600;
    } else if (stupser_ >= 4 && art_ == ROBOTER && zufall(2)) {  // Roboter: jedes zweite Mal Visier-Glitch
      stupser_ = 0;
      glitch();
    } else if (stupser_ >= 4) {
      stupser_ = 0;
      zeitweise(SCHWINDLIG, 3200);
      anim(cb_wackel, 0, 1000, 1600);
      zappeln_bis_ = jetzt + 3000;
      led_blitz(0x8A4DFF, 3000);
      nachher_text_ = "Bitte nicht.";
      nachher_bei_ = jetzt + 3300;
    } else {
      zeitweise(FROH, 1300);
      anim(cb_hop, 0, 14, 160, 180, 1, lv_anim_path_ease_out);
      zappeln_bis_ = jetzt + 900;
      led_blitz(plan_->licht, 400);
    }
  }

  void zeige_schild(const std::string &text, int sekunden);

  // Absichtlich unperfekt: Schild falsch herum (dreht es dann um) oder mit Buchstabendreher (korrigiert genervt)
  void schild_patzer(const std::string &text);
  void patzer_tick(uint32_t jetzt);

  // Nachricht zeigen. warten = erst Aufmerksamkeit (winken, „!“, LED pulsiert), Nachricht erst nach Antippen.
  // Lange Texte werden auf mehrere Schilder verteilt. Wartende Nachrichten überleben die Uhr-Ansicht
  // und das Dunkel; Yellow meldet sich, sobald er wieder sichtbar ist. Höchstens 5 in der Schlange.
  void nachricht(const std::string &text, bool warten, int sekunden = 4);
  int wartend() const { return (int) post_.size(); }
  // Frage still entfernen (ohne Antwort-Ereignis)
  void frage_abbrechen();
  // Post per HA: nächste Nachricht zeigen (wie Antippen) oder alle verwerfen
  void post_abspielen();
  void post_leeren();
  // Stimmung 0 (gelb, froh) … 100 (rot, grummelig), Regler in HA. Genervt/traurig ziehen zeitweise auf 100.
  void stimmung(int v) {
    stimmung_ = std::max(0, std::min(100, v));
    if (bereit()) ausdruck_setzen(ausdruck_);
  }
  // Anspannung 0 (entspannt) … 100 (super nervös): Arm-/Schwanz-Tempo, Blinzeln, Blick, Atem, Schweiß, Zusammenzucken
  void anspannung(int v) {
    anspannung_ = std::max(0, std::min(100, v));
    if (bereit() && sichtbar_) atem_starten();
  }
  // Energie 0 (todmüde) … 100 (hellwach): Atemtempo, Gähnen, Langeweile schneller, halb geschlossene Augen
  void energie(int v) {
    energie_ = std::max(0, std::min(100, v));
    if (!bereit()) return;
    if (sichtbar_) atem_starten();
    // sehr müde: Augen dauerhaft halb zu; wieder wach: zurück (außer die Langeweile-Treppe hat schon „müde“ gesetzt)
    if (energie_ < 20 && basis_ == NORMAL) basis_ = MUEDE;
    else if (energie_ >= 20 && basis_ == MUEDE && lw_stufe_ < 3) basis_ = NORMAL;
    if (!ausdruck_bis_ && koerper_) ausdruck_setzen(basis_);
  }
  // Atem: müde = langsam und tief, nervös = schnell und flach
  void atem_starten() {
    int ms = 1300 + (100 - energie_) * 14 - anspannung_ * 6;
    int tief = energie_ < 30 ? 4 : anspannung_ > 70 ? 2 : 3;
    anim(cb_atem, 0, tief, ms, ms, LV_ANIM_REPEAT_INFINITE);
  }
  static std::string dauer_text(int sek) {
    if (sek >= 90) return esphome::str_sprintf("%d min", (sek + 30) / 60);
    return esphome::str_sprintf("%d s", sek);
  }
  // Requisit zeigen: sonnenbrille, schirm, schal, keins; sekunden 0 = bleibt
  // Yellow verreist: Figur weg, Koffer mit Aufklebern und Zettel (Text = letzte Postkarte); zurück mit Souvenir
  void verreist(bool an, const std::string &karte);

  bool requisit(const std::string &name, int sekunden) {
    int r = name == "sonnenbrille" ? 1 : name == "schirm" ? 2 : name == "schal" ? 3 : name == "faecher" ? 4
          : name == "giesskanne" ? 5 : name == "tuete" ? 6 : name == "waeschekorb" ? 7 : name == "muelltonne" ? 8
          : name == "kissen" ? 9 : name == "keins" ? 0 : -1;
    if (r < 0) return false;
    if (r == 4 && art_ == FUCHS) {  // Fuchs braucht keinen Fächer: fächelt mit dem Schwanz
      schwanzfaecher();
      return true;
    }
    requisit_ = r;
    requisit_bis_ = (r && sekunden > 0) ? esphome::millis() + sekunden * 1000 : 0;
    requisit_layout();
    return true;
  }
  // Wo Pfote/Hand i (0 links, 1 rechts) ein Requisit hält (Körper-Koordinaten); false = Hand frei
  bool griff(int i, int &x, int &y) const {
    static const int G[10][2][3] = {
        {{0}, {0}}, {{0}, {0}}, {{0}, {1, 186, 100}}, {{0}, {0}}, {{1, -4, 106}, {0}}, {{0}, {1, 180, 108}},
        {{0}, {0}}, {{1, 48, 132}, {1, 132, 132}}, {{0}, {1, 196, 96}}, {{1, 4, 72}, {0}}};
    if (requisit_ < 0 || requisit_ > 9 || !G[requisit_][i][0]) return false;
    x = G[requisit_][i][1];
    y = G[requisit_][i][2];
    return true;
  }
  void requisit_layout() {
    zeigen(brille_, requisit_ == 1);
    zeigen(schirm_, requisit_ == 2);
    zeigen(schal_, requisit_ == 3);
    zeigen(faecher_, requisit_ == 4);
    zeigen(kanne_, requisit_ == 5);
    zeigen(tuete_, requisit_ == 6);
    for (auto *r : rauch_) zeigen(r, requisit_ == 6);
    zeigen(korb_, requisit_ == 7);
    zeigen(tonne_, requisit_ == 8);
    zeigen(kissen_, requisit_ == 9);
  }
  // Liebesbriefe sind Post mit Präfix LIEBE_PRAEFIX: rosa Umschlag, Herz-Siegel, Herzen beim Öffnen
  static bool ist_liebe(const std::string &t) { return t.rfind(LIEBE_PRAEFIX, 0) == 0; }
  void brief_layout() {
    zeigen(brief_, !post_.empty() || (brief_offen_bis_ && !vorbei(esphome::millis(), brief_offen_bis_)));
    if (brief_ == nullptr) return;
    bool liebe = !post_.empty() && ist_liebe(post_.front());
    lv_obj_set_style_bg_color(brief_, lv_color_hex(liebe ? 0xFFC8D8 : 0xF4E4C1), 0);
    if (brief_siegel_) {
      lv_obj_set_style_bg_color(brief_siegel_, lv_color_hex(liebe ? 0xFF2A5A : ROT), 0);
      lv_obj_set_size(brief_siegel_, liebe ? 11 : 9, liebe ? 11 : 9);
    }
  }
  // Klappe zu (V nach unten) oder offen (Spitze nach oben); Linie liegt 14 px höher, damit „offen“ hineinpasst
  void klappe(bool offen) {
    if (brief_klappe_obj_ == nullptr) return;
    brief_klappe_[0].x = 0;
    brief_klappe_[0].y = 14;
    brief_klappe_[1].x = 21;
    brief_klappe_[1].y = offen ? 0 : 29;
    brief_klappe_[2].x = 42;
    brief_klappe_[2].y = 14;
    lv_line_set_points(brief_klappe_obj_, brief_klappe_, 3);
  }

  void setze_ausdruck(const std::string &name, int sekunden) {
    if (!sichtbar_) return;
    static const char *const namen[] = {"normal", "froh", "ueberrascht", "schwindlig", "muede",
                                        "genervt", "traurig", "schlaf", "gaehnen"};
    for (int i = 0; i <= GAEHNEN; i++) {
      if (name != namen[i]) continue;
      if (sekunden <= 0) {  // dauerhaft, bis zur nächsten Berührung
        basis_ = (Ausdruck) i;
        ausdruck_bis_ = 0;
        ausdruck_setzen(basis_);
      } else {
        zeitweise((Ausdruck) i, sekunden * 1000);
      }
      if (i == SCHWINDLIG) anim(cb_wackel, 0, 1000, 1600);
      return;
    }
  }

  // Einzelne Geste (für HA/LLM): huepfen, wackeln, wedeln, nicken, kopfschuetteln, winken, erschrecken, orakel
  bool geste(const std::string &g) {
    if (!sichtbar_) return false;
    uint32_t jetzt = esphome::millis();
    if (g == "huepfen") anim(cb_hop, 0, 14, 160, 180, 2, lv_anim_path_ease_out);
    else if (g == "wackeln") anim(cb_wackel, 0, 1000, 1500);
    else if (g == "wedeln") zappeln_bis_ = jetzt + 2000;
    else if (g == "nicken") anim(cb_blick_y, 0, 7, 180, 180, 2);
    else if (g == "kopfschuetteln") anim(cb_schuetteln, 0, 1000, 1200);
    else if (g == "winken") winken(2500);
    else if (g == "tinte" && art_ == OKTI) tinte();
    else if (g == "faustgruss") faustgruss();
    else if (g == "spezial") spezial();
    else if (g == "maeuselsprung" && art_ == FUCHS) maeuselsprung();
    else if (g == "glitch" && art_ == ROBOTER) glitch();
    else if (g == "knoten" && art_ == OKTI) knoten();
    else if (g == "lichtschreck") licht_schreck();
    else if (g == "herzen") herzen_start();
    else if (g == "hilfe") hilfe(true);
    else if (g == "jonglieren" && art_ == OKTI) jonglieren();
    else if (g == "schwanz_jagen" && art_ == FUCHS) schwanz_jagen();
    else if (g == "propeller" && art_ == ROBOTER) propeller();
    else if (g == "schwanzfaecher" && art_ == FUCHS) schwanzfaecher();
    else if (g == "visier" && art_ == ROBOTER) visier_auf();
    else if (g == "patzer") schild_patzer(spruch());
    else if (g == "welk") {  // Arme hängen schlaff, traurig, 6 s
      zeitweise(TRAURIG, 6000);
      welk_bis_ = jetzt + 6000;
    } else if (g == "tanzen") {  // Freudentanz: hüpfen, wackeln, alle Arme zappeln, 4 s
      zeitweise(FROH, 4000);
      anim(cb_hop, 0, 12, 180, 180, 5, lv_anim_path_ease_out);
      anim(cb_wackel, 0, 1000, 3600);
      zappeln_bis_ = jetzt + 4000;
      funkel_bis_ = jetzt + 4000;
      if (led) led(0xFFD23F, 0.5f);
      led_aus_bei_ = jetzt + 4000;
    } else if (g == "nabu_kommt") {  // Mini-Nabu läuft von rechts herein, bleibt, Yellow winkt, Nabu geht wieder
      lv_obj_move_foreground(nabu_mini_);
      zeigen(nabu_mini_, true);
      anim(cb_nabu, 330, 248, 1800, 0, 1, lv_anim_path_ease_out);
      anim(cb_blick_x, blick_x_, 12, 400);
      naechster_blick_ = jetzt + 9000;
      nabu_gruss_bei_ = jetzt + 1900;
      nabu_geht_bei_ = jetzt + 9000;
    } else if (g == "besuch") {  // „Bin kurz bei Nabu.“, läuft rechts raus, kommt nach 10 s von links zurück
      zeige_schild("Bin kurz bei Nabu.", 2);
      besuch_los_ = jetzt + 2200;
      besuch_zurueck_ = jetzt + 13000;
    } else if (g == "orakel") {
      static const char *const antworten[] = {"Ja.", "Nein.", "Eindeutig.", "Lieber nicht.", "Frag Nabu.",
                                              "Pizza.", "Morgen.", "Frag später nochmal.", "Na klar.",
                                              "Träum weiter.", "Auf jeden Fall.", "Kein Kommentar."};
      zeitweise(UEBERRASCHT, 1500);
      anim(cb_wackel, 0, 1000, 1200);
      zappeln_bis_ = jetzt + 1200;
      nachher_text_ = antworten[zufall(sizeof(antworten) / sizeof(antworten[0]))];
      nachher_bei_ = jetzt + 1400;
    } else if (g == "erschrecken" && art_ == FUCHS) {
      zeitweise(UEBERRASCHT, 700);
      anim(cb_hop, 0, 8, 120, 160, 1, lv_anim_path_ease_out);
      nachher_text_ = "Hab mich nicht erschreckt.";
      nachher_bei_ = jetzt + 900;
    } else if (g == "erschrecken") {
      zeitweise(UEBERRASCHT, 2000);
      anim(cb_hop, 0, 26, 180, 240, 1, lv_anim_path_ease_out);
      zappeln_bis_ = jetzt + 1200;
    } else
      return false;
    naechster_blick_ = jetzt + 2500;
    return true;
  }
  // Tintenwolke (nur Okti): dunkle Kreise quellen auf, Okti verschwindet darin, die Wolke verblasst,
  // Okti taucht verlegen (rot) wieder auf
  void tinte();
  void tinte_tick(uint32_t jetzt);

  // Faustgruß: eine Faust kommt nach vorn (wächst), Schild „Faust?“; antippen → „Bumm!“ mit Funken, sonst nach 8 s enttäuscht
  void faustgruss();
  void faust_treffer(uint32_t jetzt);
  void faust_tick(uint32_t jetzt);

  // Tageszahlen (Pwnagotchi-Prinzip): Statuszeile unten, Sprüche daraus, für HA als "tipps|streicheln|spiele|faust"
  enum Zahl { TIPPS, STREICHELN, SPIELE, FAUST };
  std::string zahlen_text() const {
    return esphome::str_sprintf("%d|%d|%d|%d", zahl_[TIPPS], zahl_[STREICHELN], zahl_[SPIELE], zahl_[FAUST]);
  }
  void neuer_tag() {
    for (int &z : zahl_) z = 0;
  }
  std::string zahlen_spruch() const {
    int t = zahl_[TIPPS], s = zahl_[STREICHELN], g = zahl_[SPIELE];
    switch (zufall(4)) {
      case 0:
        if (t >= 10) return esphome::str_sprintf("Heute %d× angetippt. Reicht.", t);
        break;
      case 1:
        if (t >= 3 && s == 0) return "Heute noch nicht gestreichelt. Nur so.";
        if (s >= 3) return esphome::str_sprintf("%d× gestreichelt. Ich zähle mit.", s);
        break;
      case 2:
        if (g >= 3) return esphome::str_sprintf("%d Spiele heute. Ich bin erschöpft.", g);
        break;
      default:
        if (t == 0 && s == 0) return "Heute: 0 Berührungen. Statistik lügt nicht.";
    }
    return "";
  }
  // Statuszeile unten: Symbol + Zahl je Wert (Tippen, Streicheln, Spiele, Faust), Faust erst ab 1
  void zahlen_tick() {
    static const char *const SYM[4] = {"\U000F0741", "\U000F10F1", "\U000F0297", "\U000F0FD7"};
    if (status_ == nullptr) {
      status_ = form(root_, 0, 221, 320, 19, 0, 0);
      transparent(status_);
      lv_obj_set_flex_flow(status_, LV_FLEX_FLOW_ROW);
      lv_obj_set_flex_align(status_, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
      lv_obj_set_style_pad_column(status_, 4, 0);
      for (int i = 0; i < 4; i++) {
        status_sym_[i] = lv_label_create(status_);
        if (status_font_) lv_obj_set_style_text_font(status_sym_[i], status_font_, 0);
        lv_label_set_text(status_sym_[i], status_font_ ? SYM[i] : "");
        status_zahl_[i] = lv_label_create(status_);
        lv_obj_set_style_pad_right(status_zahl_[i], 10, 0);
        for (auto *l : {status_sym_[i], status_zahl_[i]}) lv_obj_set_style_text_color(l, lv_color_hex(0x5A6680), 0);
      }
      status_stand_ = -1;
    }
    int stand = zahl_[TIPPS] * 1000000 + zahl_[STREICHELN] * 10000 + zahl_[SPIELE] * 100 + zahl_[FAUST];
    if (stand == status_stand_) return;
    status_stand_ = stand;
    for (int i = 0; i < 4; i++) {
      lv_label_set_text(status_zahl_[i], esphome::to_string(zahl_[i]).c_str());
      bool an = i < 3 || zahl_[FAUST] > 0;
      zeigen(status_sym_[i], an);
      zeigen(status_zahl_[i], an);
    }
  }
  void status_schrift(const lv_font_t *f) { status_font_ = f; }
  void ziffer_schrift(const lv_font_t *f) { ziffer_font_ = f; }

  // Special Move der aktuellen Figur: Okti Tintenwolke, Fuchs Mäuselsprung, Roboter Visier-Glitch
  void spezial();
  // Okti jongliert mit drei Bällen über dem Kopf; tollpatschig: manchmal fällt einer runter
  void jonglieren();
  // Fuchs jagt den eigenen Schwanz: rennt hin und her, wird schwindlig
  void schwanz_jagen();
  // Roboter: Propeller fährt aus, er hebt kurz ab und landet wieder
  void propeller();
  void spezial3_tick(uint32_t jetzt);
  // Okti: verknotet sich die Arme, zappelt, „Moment.“, schüttelt sie los
  void knoten();
  // Fuchs: fächelt sich mit dem Schwanz Luft zu (schnell vor dem Gesicht)
  void schwanzfaecher();
  // Roboter: Visier klappt hoch, Zahnräder drehen sich, „Wartung.“, klappt wieder zu
  void visier_auf();
  void spezial2_tick(uint32_t jetzt);

  // Fuchs: springt hoch und taucht kopfüber in einen Erdhügel (Ohren und Schwanz schauen raus), kommt wieder hoch
  void maeuselsprung();
  void maus_tick(uint32_t jetzt);

  // Roboter: Visier flackert, Gesicht springt, Scanlinien laufen, kurz schwarz, dann „Neustart… fertig.“
  void glitch();
  void glitch_tick(uint32_t jetzt);

  // Matrix-Regen (13:37, Nerd-Event des Roboters): grüne Nullen und Einsen fallen hinter der Figur
  void matrix(int sekunden);
  void matrix_tick(uint32_t jetzt);

  // Schlafen legen (Schalter „Yellow schläft“, auch per Sprache): Okti im Bett mit Decke, Arme hängen raus;
  // Fuchs zusammengerollt, Schwanz vorn herum; Roboter im Standby (Gesicht aus, Standby-Punkt pulsiert)
  void schlafen(bool an);
  bool schlaeft() const { return schlaeft_; }
  void schlaf_szene();
  void schlaf_tick(uint32_t jetzt);

  // Erschrecken bei plötzlichem Licht (Raum war dunkel, Lampe geht an): zuckt, kneift die Augen zu, je Figur anders
  void licht_schreck();
  void schreck_tick(uint32_t jetzt);
  void zeigen_spaeter(const char *text, uint32_t ms) {
    nachher_text_ = text;
    nachher_bei_ = esphome::millis() + ms;
  }

  // Herzen steigen beim Liebesbrief auf (MDI-Herz aus der Deko-Schrift)
  void herzen_start();
  void herzen_tick(uint32_t jetzt);

  // Hilfe: kleines „?“ unten rechts; Antippen zeigt, wie man Yellow bedient (später kommt hier ein Settings-Knopf dazu)
  void hilfe_bauen();
  void hilfe(bool an);
  bool hilfe_offen() const { return hilfe_bis_ != 0; }
  // Spiele-Knopf unten links: Auswahl Reaktion / Tic-Tac-Toe / Simon
  void spiele_menue(bool an);
  void spiele_tipp(int y);
  bool spiele_offen() const { return spiele_bis_ != 0; }

  // Netzstatus neben dem „?“: 0 = alles gut, 1 = WLAN fehlt (rot, „wartet auf Netz: …“),
  // 2 = MQTT getrennt, Internet da (orange Wolke), 3 = WLAN ohne Internet (orange Weltkugel)
  int netz_stufe() const { return netz_stufe_; }
  void netz(int stufe, const std::string &ssid);

  // Langeweile-Treppe ab der letzten Berührung: 5 min neugierig, 10 min gelangweilt (tippt mit dem Arm, „…“),
  // 20 min müde, 40 min döst, 60 min schmollt (rot, schaut weg; Antippen reicht nicht, zweimal Streicheln versöhnt)
  void langeweile(uint32_t jetzt) {
    // müde Yellows langweilen sich schneller, wache langsamer
    uint32_t m = (jetzt - letzte_beruehrung_) / 60000 * (50 + (100 - energie_)) / 100;
    int stufe = m >= 60 ? 5 : m >= 40 ? 4 : m >= 20 ? 3 : m >= 10 ? 2 : m >= 5 ? 1 : 0;
    if (stufe <= lw_stufe_ || schild_bis_ || ausdruck_bis_ || !post_.empty() || spiel_ != KEIN_SPIEL) return;
    if (stufe >= 3 && basis_ != NORMAL && basis_ != MUEDE && basis_ != SCHLAF) return;  // von HA gesetzt: nicht überschreiben
    lw_stufe_ = stufe;
    switch (stufe) {
      case 1:
        anim(cb_blick_x, blick_x_, 0, 200);
        anim(cb_blick_y, blick_y_, 2, 200);
        naechster_blick_ = jetzt + 4000;
        zeige_schild(zufall(2) ? "Hallo?" : "Jemand da?", 3);
        break;
      case 2:
        zappeln_bis_ = jetzt + 1500;
        anim(cb_wackel, 0, 1000, 1200);
        zeige_schild(zufall(2) ? "…" : "Mir ist langweilig.", 3);
        break;
      case 3:
        basis_ = MUEDE;
        ausdruck_setzen(basis_);
        break;
      case 4:
        basis_ = SCHLAF;
        ausdruck_setzen(basis_);
        break;
      case 5:
        schmollen_ = true;
        versoehnt_ = 0;
        basis_ = GENERVT;
        ausdruck_setzen(basis_);
        anim(cb_blick_x, blick_x_, -12, 400);
        naechster_blick_ = jetzt + 3000;
        zeige_schild("Hmpf.", 3);
        break;
    }
  }
  // Test: so tun, als läge die letzte Berührung so viele Minuten zurück
  void langeweile_test(int minuten) {
    letzte_beruehrung_ = esphome::millis() - (uint32_t) minuten * 60000;
    lw_stufe_ = 0;
    schmollen_ = false;
    basis_ = NORMAL;
    ausdruck_bis_ = 0;
    if (schild_bis_) schild_bis_ = esphome::millis() + 10;  // laufendes Schild wegräumen
  }

  // Spruchliste aus der Ferne (eine Zeile = ein Spruch, „-“ oder leer = keine)
  void fern_sprueche(const std::string &t) {
    fern_.clear();
    size_t a = 0;
    while (t != "-" && a < t.size()) {
      size_t e = t.find('\n', a);
      if (e == std::string::npos) e = t.size();
      if (e > a) fern_.push_back(t.substr(a, e - a));
      a = e + 1;
    }
  }

  // Zufälliger Leerlauf-Spruch: jeder dritte aus der Ferne (falls da), sonst halb gemeinsam, halb von der Figur
  const char *spruch() const {
    if (!fern_.empty() && zufall(3) == 0) return fern_[zufall(fern_.size())].c_str();
    if (zufall(2)) return SPRUECHE[zufall(sizeof(SPRUECHE) / sizeof(SPRUECHE[0]))];
    if (art_ == ROBOTER) return SPRUECHE_ROBOTER[zufall(sizeof(SPRUECHE_ROBOTER) / sizeof(SPRUECHE_ROBOTER[0]))];
    if (art_ == FUCHS) return SPRUECHE_FUCHS[zufall(sizeof(SPRUECHE_FUCHS) / sizeof(SPRUECHE_FUCHS[0]))];
    return SPRUECHE_OKTI[zufall(sizeof(SPRUECHE_OKTI) / sizeof(SPRUECHE_OKTI[0]))];
  }

  // Effekt-Baustein für ein paar Sekunden: schweiss, traene, erroeten, funkeln, keins (0 s = 5 s)
  bool effekt(const std::string &e, int sekunden) {
    uint32_t bis = esphome::millis() + (sekunden > 0 ? sekunden : 5) * 1000;
    if (e == "schweiss") schweiss_bis_ = bis;
    else if (e == "traene") traene_bis_ = bis;
    else if (e == "erroeten") roet_bis_ = bis;
    else if (e == "funkeln") funkel_bis_ = bis;
    else if (e == "keins") schweiss_bis_ = traene_bis_ = roet_bis_ = funkel_bis_ = 0;
    else return false;
    return true;
  }
  // Blickrichtung für ein paar Sekunden: links, rechts, oben, unten, mitte
  bool blick(const std::string &r) {
    if (!sichtbar_) return false;
    int tx = 0, ty = 0;
    if (r == "links") tx = -12;
    else if (r == "rechts") tx = 12;
    else if (r == "oben") ty = -7;
    else if (r == "unten") ty = 8;
    else if (r != "mitte") return false;
    anim(cb_blick_x, blick_x_, tx, 250);
    anim(cb_blick_y, blick_y_, ty, 250);
    naechster_blick_ = esphome::millis() + 5000;
    return true;
  }

  void klingel() {
    if (!sichtbar_) return;
    if (art_ == OKTI) {  // Okti erschrickt sich in die eigene Tinte und hält danach das Schild hoch
      tinte();
      nachher_text_ = "TÜR!!";
      nachher_bei_ = esphome::millis() + 2500;
      led_blitz(0xFFFFFF, 1500);
      return;
    }
    zeitweise(UEBERRASCHT, 3000);
    anim(cb_hop, 0, 30, 200, 260, 1, lv_anim_path_ease_out);
    zappeln_bis_ = esphome::millis() + 2000;
    if (art_ == FUCHS) {  // mutig: kein Satz, schaut zur Tür
      ausdruck_bis_ = 0;
      zeitweise(NORMAL, 3000);
      anim(cb_blick_x, blick_x_, 12, 200);
      naechster_blick_ = esphome::millis() + 4000;
      zeige_schild(zufall(2) ? "Ich geh nachsehen." : "Wer da? Keine Angst.", 6);
      return;
    }
    zeige_schild(art_ == ROBOTER ? "TÜR!! (98 %)" : "TÜR!!", 8);
    led_blitz(0xFFFFFF, 1500);
  }

  // Wo Nabu steht (aus HA): links, rechts, oben (= anderer Raum)
  void nabu_richtung(const std::string &r) {
    nabu_blick_x_ = r == "rechts" ? 12 : r == "oben" ? 0 : -12;
    nabu_blick_y_ = r == "oben" ? -7 : -4;
  }
  // Nabus LED-Ring geht an: Yellow schaut aufmerksam hin, neidisch oder neugierig (höchstens alle 20 s)
  void nabu_ring() {
    if (!sichtbar_ || gedrueckt_ || streichel_aktiv_) return;
    uint32_t jetzt = esphome::millis();
    if (nabu_ruhe_ && !vorbei(jetzt, nabu_ruhe_)) return;
    nabu_ruhe_ = jetzt + 20000;
    anim(cb_blick_x, blick_x_, nabu_blick_x_, 200);  // dahin, wo Nabu steht
    anim(cb_blick_y, blick_y_, nabu_blick_y_, 200);
    naechster_blick_ = jetzt + 4500;
    if (zufall(2)) {
      zeitweise(GENERVT, 4500);
      zeige_schild("Angeber.", 3);
    } else {
      zeitweise(UEBERRASCHT, 3500);
      anim(cb_hop, 0, 8, 150, 150, 1, lv_anim_path_ease_out);
      if (zufall(2)) zeige_schild(zufall(2) ? "Ooh!" : "Wie machst du das?", 3);
    }
  }

  // alle 50 ms aus dem YAML
  void tick() {
    if (!sichtbar_) return;
    uint32_t jetzt = esphome::millis();
    if (schlaeft_) {  // schläft: nur atmen, Deko und Wetter laufen weiter
      schlaf_tick(jetzt);
      deko_tick(jetzt);
      wetter_tick(jetzt);
      tentakel_tick(jetzt);
      return;
    }
    if (verreist_) {  // Koffer wippt leicht, sonst passiert nichts
      if (koffer_) lv_obj_set_y(koffer_, (jetzt / 700) % 2 ? -1 : 0);
      deko_tick(jetzt);
      wetter_tick(jetzt);
      return;
    }
    tentakel_tick(jetzt);
    deko_tick(jetzt);
    wetter_tick(jetzt);
    timer_tick(jetzt);
    effekt_tick(jetzt);
    patzer_tick(jetzt);
    tinte_tick(jetzt);
    faust_tick(jetzt);
    zahlen_tick();
    maus_tick(jetzt);
    glitch_tick(jetzt);
    matrix_tick(jetzt);
    spezial2_tick(jetzt);
    schreck_tick(jetzt);
    if (hilfe_bis_ && vorbei(jetzt, hilfe_bis_)) hilfe(false);
    if (spiele_bis_ && vorbei(jetzt, spiele_bis_)) spiele_menue(false);
    if (schmuck_an_ && schmuck_glanz_)  // Bling: Glanzpunkt blitzt kurz auf
      lv_obj_set_style_bg_opa(schmuck_glanz_, (jetzt / 120) % 18 < 2 ? LV_OPA_COVER : 90, 0);
    spezial3_tick(jetzt);
    herzen_tick(jetzt);
    if (vorbei(jetzt, nabu_gruss_bei_)) {
      nabu_gruss_bei_ = 0;
      zeitweise(FROH, 3000);
      winken(2500);
      zeige_schild("Oh, Besuch!", 3);
    }
    if (vorbei(jetzt, nabu_geht_bei_)) {
      nabu_geht_bei_ = 0;
      anim(cb_nabu, 248, 330, 1800, 0, 1, lv_anim_path_ease_in);
      nabu_weg_bei_ = jetzt + 1900;
    }
    if (vorbei(jetzt, nabu_weg_bei_)) {
      nabu_weg_bei_ = 0;
      zeigen(nabu_mini_, false);
    }
    if (vorbei(jetzt, besuch_los_)) {
      besuch_los_ = 0;
      zappeln_bis_ = jetzt + 1600;
      anim(cb_besuch, 0, 340, 1600, 0, 1, lv_anim_path_ease_in);
    }
    if (vorbei(jetzt, besuch_zurueck_)) {
      besuch_zurueck_ = 0;
      zappeln_bis_ = jetzt + 1600;
      anim(cb_besuch, -340, 0, 1600, 0, 1, lv_anim_path_ease_out);
      zeitweise(FROH, 3000);
    }
    if (requisit_ == 6 && tuete_ != nullptr) {  // Rauch steigt, wird größer, fängt unten neu an
      for (int k = 0; k < 3; k++) {
        int t = (int) ((jetzt / 40 + k * 25) % 75);
        lv_obj_set_pos(rauch_[k], plan_->mund_x + 32 + t / 6 + (int) (4 * sinf(t / 8.0f)) - t / 6, plan_->mund_y - 10 - t);
        lv_obj_set_size(rauch_[k], 8 + t / 3, 8 + t / 3);
        lv_obj_set_style_bg_opa(rauch_[k], (lv_opa_t) (200 - t * 2), 0);
      }
    }
    if (requisit_bis_ && vorbei(jetzt, requisit_bis_)) {
      requisit_bis_ = 0;
      requisit_ = 0;
      requisit_layout();
    }
    spiel_tick(jetzt);
    if (frage_aktiv_) {
      if (vorbei(jetzt, frage_bis_)) {
        frage_beenden("");
      } else if (!frage_gezeigt_ && !gedrueckt_) {
        frage_gezeigt_ = true;
        schilder_.clear();
        zeitweise(UEBERRASCHT, 1500);
        anim(cb_blick_x, blick_x_, 0, 200);
        anim(cb_blick_y, blick_y_, 2, 200);
        lv_label_set_text(schild_text_, frage_text_.c_str());
        anim(cb_schild, schild_y_, SCHILD_DA, 380, 0, 1, lv_anim_path_overshoot);
        anim(cb_arm, arm_pos_, 100, 400);
        schild_bis_ = frage_bis_;  // Schild bleibt bis zur Antwort
        for (int i = 0; i < 2; i++) lv_label_set_text(frage_lbl_[i], frage_opt_[i].c_str());
        lv_obj_move_foreground(frage_box_);
        zeigen(frage_box_, true);
        if (led) led(0x4AA8FF, 0.4f);
        led_aus_bei_ = jetzt + 1500;
      }
    }
    if (brief_offen_bis_ && vorbei(jetzt, brief_offen_bis_)) {
      brief_offen_bis_ = 0;
      klappe(false);
      brief_layout();
    }
    if (vorbei(jetzt, led_aus_bei_)) {
      led_aus_bei_ = 0;
      if (led) led(0, 0);
    }
    if (vorbei(jetzt, nachher_bei_)) {
      nachher_bei_ = 0;
      zeige_schild(nachher_text_, 3);
    }
    if (vorbei(jetzt, ausdruck_bis_)) {
      ausdruck_bis_ = 0;
      ausdruck_setzen(basis_);
    }
    if (vorbei(jetzt, schild_bis_)) {
      schild_bis_ = 0;
      if (!schilder_.empty()) {  // nächstes Schild der Folge direkt hinterher
        zeige_schild(schilder_.front(), schild_dauer_);
        schilder_.pop_front();
      } else if (art_ == OKTI && zufall(6) == 0) {  // tollpatschig: Schild rutscht ihm nach unten weg
        anim(cb_schild, schild_y_, 250, 450, 0, 1, lv_anim_path_ease_in);
        anim(cb_arm, arm_pos_, 0, 300);
        zeitweise(UEBERRASCHT, 1300);
        zappeln_bis_ = jetzt + 800;
      } else {
        anim(cb_schild, schild_y_, SCHILD_WEG, 300, 0, 1, lv_anim_path_ease_in);
        anim(cb_arm, arm_pos_, 0, 400);
      }
    }
    // Wartende Nachricht: alle paar Sekunden winken, hüpfen, „!“; LED pulsiert
    if (!post_.empty() && !gedrueckt_ && vorbei(jetzt, naechster_ruf_)) {
      naechster_ruf_ = jetzt + 7000;
      if (!schild_bis_) {
        zeitweise(UEBERRASCHT, 1500);
        winken(2500);
        anim(cb_hop, 0, 10, 150, 150, 2, lv_anim_path_ease_out);
        anim(cb_blick_x, blick_x_, 0, 200);  // zum Betrachter
        anim(cb_blick_y, blick_y_, 2, 200);
        naechster_blick_ = jetzt + 4000;
        bool post_text = (rufe_++ % 2) == 0;
        bool liebe = ist_liebe(post_.front());
        std::string t = post_text ? (liebe ? std::string(art_ == FUCHS ? "Psst. Ein Liebesbrief!" : "Liebesbrief!")
                                     : post_.size() > 1 ? esphome::str_sprintf("%d x Post für dich!", (int) post_.size())
                                                        : std::string("Post für dich!"))
                                  : std::string("Tipp mich an!");
        zeige_schild(t, 3);
      }
      if (led) led(0xFFB020, 0.5f);
      led_aus_bei_ = jetzt + 900;
    }
    if (gedrueckt_ || streichel_aktiv_) return;
    langeweile(jetzt);
    if (vorbei(jetzt, naechstes_blinzeln_)) {
      if (augen_rund() && augen_h_ >= 10) anim(cb_blinzel, 0, 100, 70, 90, zufall(5) == 0 ? 2 : 1);
      naechstes_blinzeln_ = jetzt + (1800 + zufall(4500)) * (130 - anspannung_ * 8 / 10) / 100;  // nervös blinzelt öfter
    }
    if (vorbei(jetzt, naechster_blick_)) {
      if (ausdruck_ != SCHLAF) {
        int tx = 0, ty = 0;
        if (schmollen_) {  // schaut beleidigt weg, linst nur selten herüber
          tx = zufall(6) ? -12 : -4;
          ty = 3;
        } else if (zufall(100) >= 45) {
          tx = (int) zufall(25) - 12;
          ty = (int) zufall(13) - 6;
        }
        anim(cb_blick_x, blick_x_, tx, 260);
        anim(cb_blick_y, blick_y_, ty, 260);
      }
      naechster_blick_ = jetzt + (1500 + zufall(5000)) * (130 - anspannung_ * 9 / 10) / 100;  // nervös schaut hektisch
    }
    if (vorbei(jetzt, naechste_idee_)) {
      if (!ausdruck_bis_ && !schild_bis_ && ausdruck_ != SCHLAF) idee();
      naechste_idee_ = jetzt + 12000 + zufall(25000);
    }
  }

 protected:
  using Cb = void (*)(void *, int32_t);

  static lv_obj_t *form(lv_obj_t *eltern, int x, int y, int w, int h, uint32_t farbe,
                        int radius = LV_RADIUS_CIRCLE) {
    lv_obj_t *o = lv_obj_create(eltern);
    lv_obj_remove_style_all(o);
    lv_obj_clear_flag(o, (lv_obj_flag_t) (LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE));
    lv_obj_set_pos(o, x, y);
    lv_obj_set_size(o, w, h);
    lv_obj_set_style_bg_color(o, lv_color_hex(farbe), 0);
    lv_obj_set_style_bg_opa(o, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(o, radius, 0);
    return o;
  }
  static void rand(lv_obj_t *o, int breite, uint32_t farbe) {
    lv_obj_set_style_border_width(o, breite, 0);
    lv_obj_set_style_border_color(o, lv_color_hex(farbe), 0);
  }
  static lv_obj_t *bogen(lv_obj_t *eltern, int groesse, int breite, uint32_t farbe) {
    lv_obj_t *a = lv_arc_create(eltern);
    lv_obj_remove_style_all(a);
    lv_obj_clear_flag(a, (lv_obj_flag_t) (LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE));
    lv_obj_set_size(a, groesse, groesse);
    lv_obj_set_style_arc_width(a, breite, LV_PART_MAIN);
    lv_obj_set_style_arc_color(a, lv_color_hex(farbe), LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(a, true, LV_PART_MAIN);
    lv_obj_set_style_arc_opa(a, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_arc_opa(a, LV_OPA_TRANSP, LV_PART_INDICATOR);
    return a;
  }
  // Linie (Arm, Augenbraue); Punkte gehören dem Aufrufer und müssen leben bleiben
  static lv_obj_t *linie(lv_obj_t *eltern, int breite, uint32_t farbe) {
    lv_obj_t *l = lv_line_create(eltern);
    lv_obj_remove_style_all(l);
    lv_obj_clear_flag(l, (lv_obj_flag_t) (LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE));
    lv_obj_set_pos(l, 0, 0);
    lv_obj_set_style_line_width(l, breite, 0);
    lv_obj_set_style_line_color(l, lv_color_hex(farbe), 0);
    lv_obj_set_style_line_rounded(l, true, 0);
    lv_obj_set_style_line_opa(l, LV_OPA_COVER, 0);
    return l;
  }
  // Linie für Glieder (Roboterarm, Fuchsschwanz) auf großer Fläche, Punkte + (ARM_DX, ARM_DY)
  static lv_obj_t *glied(lv_obj_t *eltern, int breite, uint32_t farbe) {
    lv_obj_t *l = linie(eltern, breite, farbe);
    lv_obj_set_pos(l, -ARM_DX, -ARM_DY);
    lv_obj_set_size(l, 360, 300);
    return l;
  }
  // Gefülltes Dreieck aus 5 dicken Linien von der Spitze zur Grundseite (keine Drehung, kein eigenes Zeichnen)
  static void dreieck_linien(lv_obj_t *eltern, int ax, int ay, int bx1, int by1, int bx2, int by2, uint32_t farbe,
                             int breite, lv_point_precise_t (*pkt)[2]) {
    for (int k = 0; k < 5; k++) {
      pkt[k][0].x = ax + ARM_DX;
      pkt[k][0].y = ay + ARM_DY;
      pkt[k][1].x = bx1 + (bx2 - bx1) * k / 4 + ARM_DX;
      pkt[k][1].y = by1 + (by2 - by1) * k / 4 + ARM_DY;
      lv_obj_t *l = glied(eltern, breite, farbe);
      lv_line_set_points(l, pkt[k], 2);
    }
  }
  static void zeigen(lv_obj_t *o, bool an) {
    if (o == nullptr) return;
    if (an)
      lv_obj_clear_flag(o, LV_OBJ_FLAG_HIDDEN);
    else
      lv_obj_add_flag(o, LV_OBJ_FLAG_HIDDEN);
  }
  static void transparent(lv_obj_t *o) {
    lv_obj_set_style_bg_opa(o, LV_OPA_TRANSP, 0);
    lv_obj_add_flag(o, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
  }

  // Baut die Figur nach plan_ (neu), alles hängt an koerper_
  void figur_bauen();

  void anim(Cb cb, int32_t von, int32_t bis, uint32_t ms, uint32_t zurueck_ms = 0, uint32_t wdh = 1,
            lv_anim_path_cb_t pfad = lv_anim_path_ease_in_out) {
    lv_anim_delete(this, cb);
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, this);
    lv_anim_set_exec_cb(&a, cb);
    lv_anim_set_values(&a, von, bis);
    lv_anim_set_duration(&a, ms);
    if (zurueck_ms) lv_anim_set_playback_duration(&a, zurueck_ms);
    lv_anim_set_repeat_count(&a, wdh);
    lv_anim_set_path_cb(&a, pfad);
    lv_anim_start(&a);
  }

  static void cb_rand(lv_event_t *e) { lv_event_set_ext_draw_size(e, 110); }
  static void cb_nabu(void *v, int32_t x) {  // Mini-Nabu läuft (leichtes Wippen beim Gehen)
    auto *f = (Figur *) v;
    lv_obj_set_pos(f->nabu_mini_, x, 150 - (int) (3 * fabsf(sinf(x / 6.0f))));
  }
  static void cb_besuch(void *v, int32_t x) {
    auto *f = (Figur *) v;
    f->besuch_x_ = x;
    f->koerper_layout();
    for (int i = 0; i < 2; i++)  // Fuchspfoten tippeln beim Laufen
      if (f->pfote_[i]) lv_obj_set_y(f->pfote_[i], 156 - (x == 0 ? 0 : (int) (5 * fabsf(sinf(x / 14.0f + i * 1.57f)))));
  }
  static void cb_blinzel(void *v, int32_t x) { auto *f = (Figur *) v; f->blinzel_ = x; f->augen_layout(); }
  static void cb_blick_x(void *v, int32_t x) { auto *f = (Figur *) v; f->blick_x_ = x; f->gesicht_layout(); }
  static void cb_blick_y(void *v, int32_t x) { auto *f = (Figur *) v; f->blick_y_ = x; f->gesicht_layout(); }
  static void cb_hop(void *v, int32_t x) { auto *f = (Figur *) v; f->hop_ = x; f->koerper_layout(); }
  static void cb_atem(void *v, int32_t x) { auto *f = (Figur *) v; f->atem_ = x; f->koerper_layout(); }
  static void cb_arm(void *v, int32_t x) { auto *f = (Figur *) v; f->arm_pos_ = x; f->arme_layout(); }
  static void cb_schild_dreh(void *v, int32_t x) { lv_obj_set_style_transform_rotation(((Figur *) v)->schild_, x, 0); }
  static void cb_schild(void *v, int32_t x) { auto *f = (Figur *) v; f->schild_y_ = x; f->schild_layout(); }
  static void cb_wackel(void *v, int32_t x) {  // 0..1000 → drei Schwingungen, endet bei 0
    auto *f = (Figur *) v;
    f->wackel_ = (int) lroundf(6.0f * sinf(x / 1000.0f * 6.2832f * 3));
    f->koerper_layout();
  }
  static void cb_schuetteln(void *v, int32_t x) {  // 0..1000 → zweimal links/rechts, endet mittig
    auto *f = (Figur *) v;
    f->blick_x_ = (int) lroundf(11.0f * sinf(x / 1000.0f * 6.2832f * 2));
    f->gesicht_layout();
  }
  static void cb_spirale(void *v, int32_t x) {
    auto *f = (Figur *) v;
    for (int i = 0; i < 2; i++) lv_arc_set_rotation(f->spirale_[i], i ? 360 - x : x);
  }
  static void cb_zzz(void *v, int32_t x) {
    auto *f = (Figur *) v;
    lv_obj_set_y(f->zzz_, f->plan_->zzz_y - x);
  }

  static void cb_touch(lv_event_t *e) {
    auto *f = (Figur *) lv_event_get_user_data(e);
    if (!f->sichtbar_) return;
    lv_event_code_t code = lv_event_get_code(e);
    if (code != LV_EVENT_PRESSED && code != LV_EVENT_PRESSING && code != LV_EVENT_RELEASED &&
        code != LV_EVENT_PRESS_LOST)
      return;
    lv_indev_t *indev = lv_indev_active();
    if (indev == nullptr) return;
    lv_point_t p;
    lv_indev_get_point(indev, &p);
    uint32_t jetzt = esphome::millis();
    if (code == LV_EVENT_PRESSED) {
      if (f->faust_bis_ && abs((int) p.x - 160) < 50 && abs((int) p.y - FAUST_Y) < 45) {  // Faustgruß erwidert
        f->abgeholt_bis_ = jetzt + 800;
        f->faust_treffer(jetzt);
        return;
      }
      if (f->spiel_ != KEIN_SPIEL) {
        f->abgeholt_bis_ = jetzt + 800;
        f->spiel_tipp(p, jetzt);
        return;
      }
      if (f->frage_aktiv_ && f->frage_gezeigt_ && p.y >= 180) {  // Antwortfeld getippt
        std::string a = f->frage_opt_[p.x < 160 ? 0 : 1];
        f->abgeholt_bis_ = jetzt + 800;
        f->frage_beenden(a);
        f->zeitweise(FROH, 1800);
        f->nachher_text_ = "Danke!";
        f->nachher_bei_ = jetzt + 600;
      } else if (f->timer_klingelt_) {  // Klingeln aus (auch bei Nabu)
        f->timer_klingelt_ = false;
        f->abgeholt_bis_ = jetzt + 800;
        f->zeitweise(MUEDE, 1500);
        f->zeige_schild("Okay, okay.", 2);
        if (f->timer_aus) f->timer_aus();
      } else if (!f->post_.empty()) {  // Nachricht abholen statt anstupsen
        f->abgeholt_bis_ = jetzt + 800;
        f->post_abspielen();
      }
      f->gedrueckt_ = true;
      f->start_ = p;
      f->letzte_beruehrung_ = jetzt;
      if (!f->schmollen_) f->lw_stufe_ = 0;
      if (!f->schmollen_ && (f->basis_ == MUEDE || f->basis_ == SCHLAF)) {
        f->basis_ = NORMAL;
        if (!f->ausdruck_bis_) f->ausdruck_setzen(NORMAL);
      }
      f->finger_schauen(p);
      f->greifen(p);
    } else if (code == LV_EVENT_PRESSING) {
      if (!f->streichel_aktiv_ && abs((int) (p.x - f->start_.x)) + abs((int) (p.y - f->start_.y)) > 35) {
        f->streichel_aktiv_ = true;
        f->zahl_[STREICHELN]++;
        f->greif_arm_ = -1;
        if (f->gestreichelt)
          f->gestreichelt(jetzt - f->letztes_schild_zeit_ < 15000 ? f->letztes_schild_ : std::string());
        f->ausdruck_bis_ = 0;
        f->ausdruck_setzen(FROH);
        lv_obj_set_style_bg_opa(f->wange_[0], LV_OPA_COVER, 0);
        lv_obj_set_style_bg_opa(f->wange_[1], LV_OPA_COVER, 0);
        if (f->led) f->led(0xFF8A2A, 0.35f);
        f->led_aus_bei_ = 0;
      }
      if (f->streichel_aktiv_) {
        f->zappeln_bis_ = jetzt + 400;
      } else {
        f->finger_schauen(p);
        f->greifen(p);
      }
    } else {
      f->gedrueckt_ = false;
      f->greif_arm_ = -1;
      if (f->streichel_aktiv_) {
        f->streichel_aktiv_ = false;
        f->streichel_bis_ = jetzt + 700;
        if (f->schmollen_ && ++f->versoehnt_ < 2) {  // erstes Streicheln: noch nicht ganz
          f->zeitweise(GENERVT, 1500);
          f->nachher_text_ = "Noch nicht ganz.";
          f->nachher_bei_ = jetzt + 400;
          return;
        }
        if (f->schmollen_) {  // zweites Streicheln: versöhnt
          f->schmollen_ = false;
          f->basis_ = NORMAL;
          f->lw_stufe_ = 0;
          f->roet_bis_ = jetzt + 4000;
          f->nachher_text_ = "Na gut.";
          f->nachher_bei_ = jetzt + 400;
        }
        f->zeitweise(FROH, 2500);
        f->zappeln_bis_ = jetzt + 1200;
        f->led_aus_bei_ = jetzt + 2500;
      }
    }
  }

  void finger_schauen(lv_point_t p) {
    int tx = std::max(-12, std::min(12, (int) (p.x - 160) / 10));
    int ty = std::max(-8, std::min(8, (int) (p.y - 130) / 12));
    anim(cb_blick_x, blick_x_, tx, 120);
    anim(cb_blick_y, blick_y_, ty, 120);
    naechster_blick_ = esphome::millis() + 3000;
  }
  // Okti: der Arm, dessen Spitze dem Finger am nächsten ist, greift danach
  void greifen(lv_point_t p) {
    greif_x_ = (int) p.x - plan_->kx - wackel_;
    greif_y_ = (int) p.y - (plan_->ky - hop_ - atem_);
    if (art_ != OKTI) {  // Roboter/Fuchs: der näher liegende Arm bzw. die Pfote
      greif_arm_ = greif_x_ < 90 ? 0 : 1;
      return;
    }
    float best = 1e9f;
    for (int i = 0; i < ARME; i++) {
      float w = ARM_WINKEL[i] * 0.017453f;
      float sx = ARM_X[i] + ARM_LAENGE[i] * cosf(w), sy = ARM_Y + ARM_LAENGE[i] * sinf(w);
      float d = (sx - greif_x_) * (sx - greif_x_) + (sy - greif_y_) * (sy - greif_y_);
      if (d < best) {
        best = d;
        greif_arm_ = i;
      }
    }
  }
  void winken(uint32_t ms) { winken_bis_ = esphome::millis() + ms; }

  // Text auf Schilder (je höchstens ~24 Zeichen, an Wortgrenzen) verteilen und nacheinander zeigen
  void schilder_planen(const std::string &text, int sekunden);

  void zeitweise(Ausdruck a, uint32_t ms) {
    ausdruck_setzen(a);
    ausdruck_bis_ = esphome::millis() + ms;
  }
  void led_blitz(uint32_t farbe, uint32_t ms) {
    if (!led) return;
    led(farbe, 0.6f);
    led_aus_bei_ = esphome::millis() + ms;
  }

  void idee() {
    uint32_t r = zufall(100);
    uint32_t jetzt = esphome::millis();
    if (energie_ < 30 && zufall(3) == 0) {  // müde: gähnt oft, Augen halb zu
      zeitweise(zufall(2) ? GAEHNEN : MUEDE, 2500);
      return;
    }
    if (anspannung_ > 70 && zufall(3) == 0) {  // nervös: zuckt zusammen, schaut sich hektisch um
      zeitweise(UEBERRASCHT, 700);
      anim(cb_hop, 0, 6, 90, 120, 1, lv_anim_path_ease_out);
      anim(cb_blick_x, blick_x_, zufall(2) ? 12 : -12, 120);
      naechster_blick_ = jetzt + 600;
      if (zufall(4) == 0) zeige_schild(art_ == ROBOTER ? "Puls: hoch." : art_ == FUCHS ? "Alles ruhig. Oder?" : "Was war das?!", 3);
      return;
    }
    if (r < 4) {
      faustgruss();
    } else if (r >= 6 && r < 9 && art_ == FUCHS) {  // Fuchs zeigt Mut (4–5 bleiben Special Move): richtet sich auf, schaut entschlossen nach vorn
      ausdruck_bis_ = 0;
      zeitweise(NORMAL, 3500);
      anim(cb_hop, 0, 8, 250, 600, 1, lv_anim_path_ease_out);
      anim(cb_blick_x, blick_x_, 0, 200);
      anim(cb_blick_y, blick_y_, -3, 200);
      naechster_blick_ = jetzt + 4000;
      zeigen_spaeter(SPRUECHE_FUCHS[zufall(sizeof(SPRUECHE_FUCHS) / sizeof(SPRUECHE_FUCHS[0]))], 500);
    } else if (r < 8 && art_ != OKTI) {  // Special Move (Oktis Tinte bleibt eine Schreckreaktion)
      spezial();
    } else if (r < 6) {  // Okti: verknotet sich ab und zu
      knoten();
    } else if (r < 8) {  // Okti tollpatschig: stolpert über die eigenen Arme
      zeitweise(SCHWINDLIG, 1400);
      anim(cb_wackel, 0, 1000, 900);
      anim(cb_hop, 0, 6, 100, 140, 1, lv_anim_path_ease_out);
      zappeln_bis_ = jetzt + 900;
      nachher_text_ = zufall(2) ? "Hoppla." : "Wer hat den Arm da hingelegt?";
      nachher_bei_ = jetzt + 1500;
    } else if (r < 22) {
      anim(cb_hop, 0, 10, 150, 150, 2, lv_anim_path_ease_out);
    } else if (r < 36) {
      zeitweise(GAEHNEN, 2200);
    } else if (r < 48) {
      anim(cb_wackel, 0, 1000, 1500);
    } else if (r < 60 && (art_ == FUCHS || zufall(2))) {  // spazieren: raus nach rechts, von links wieder (Fuchs öfter)
      besuch_los_ = jetzt + 100;
      besuch_zurueck_ = jetzt + 7000;
    } else if (r < 60) {
      winken(2500);
    } else if (r < 72) {
      anim(cb_blick_x, blick_x_, zufall(2) ? 12 : -12, 300);
      naechster_blick_ = jetzt + 1800;
    } else if (r < 82) {
      const char *s = spruch();
      std::string z = zahlen_spruch();
      if (!z.empty() && zufall(art_ == ROBOTER ? 2 : 3) == 0)
        zeige_schild(z, 4);
      else if (zufall(art_ == OKTI ? 3 : 5) == 0)
        schild_patzer(s);
      else
        zeige_schild(s, 4);
    } else {
      anim(cb_blinzel, 0, 100, 70, 90, 2);
    }
  }

  bool augen_rund() const { return ausdruck_ != FROH && ausdruck_ != SCHWINDLIG; }

  // Okti-Farbe stufenlos: 0 = gelb (froh), 100 = rot (grummelig); Unterseite jeweils andersherum
  void farbe(int st);

  void ausdruck_setzen(Ausdruck a);

  void spiel_beenden(const std::string &name, int wert);
  int ttt_gewinner() const {  // 1 = du (X), 2 = Yellow (O), 3 = voll, 0 = läuft
    static const int L[8][3] = {{0,1,2},{3,4,5},{6,7,8},{0,3,6},{1,4,7},{2,5,8},{0,4,8},{2,4,6}};
    for (auto &l : L)
      if (ttt_[l[0]] && ttt_[l[0]] == ttt_[l[1]] && ttt_[l[1]] == ttt_[l[2]]) return ttt_[l[0]];
    for (int f : ttt_) if (!f) return 0;
    return 3;
  }
  void ttt_setzen(int i, int wer);
  int ttt_zug() {  // gewinnen, blocken (nicht immer), Mitte, sonst zufällig
    for (int wer : {2, 1})
      for (int i = 0; i < 9; i++) {
        if (ttt_[i]) continue;
        ttt_[i] = wer;
        bool gut = ttt_gewinner() == wer;
        ttt_[i] = 0;
        if (gut && (wer == 2 || zufall(100) < 85)) return i;
      }
    if (!ttt_[4]) return 4;
    int frei[9], n = 0;
    for (int i = 0; i < 9; i++) if (!ttt_[i]) frei[n++] = i;
    return n ? frei[zufall(n)] : -1;
  }
  void ttt_auswerten(uint32_t jetzt);
  void simon_licht(int i, bool an);
  void simon_zeigen_ab(uint32_t t);
  void simon_ende(uint32_t jetzt);
  void spiel_tick(uint32_t jetzt);
  void spiel_tipp(lv_point_t p, uint32_t jetzt);

  void frage_beenden(const std::string &a);

  // Effekte: eigene Zeiten plus automatisch – Streicheln → Erröten, Timer letzte 10 s / Spiel läuft → Schweiß,
  // traurig → Träne
  void effekt_tick(uint32_t jetzt) {
    if (schweiss_ == nullptr) return;
    auto an = [jetzt](uint32_t bis) { return bis && !vorbei(jetzt, bis); };
    bool timer_knapp = timer_ende_ && !timer_klingelt_ && (int32_t) (timer_ende_ - jetzt) < 10000;
    bool schweiss = an(schweiss_bis_) || timer_knapp || spiel_ != KEIN_SPIEL || anspannung_ >= 85;
    bool traene = an(traene_bis_) || ausdruck_ == TRAURIG;
    bool roet = an(roet_bis_) || streichelt();
    bool funkel = an(funkel_bis_);
    const Bauplan &p = *plan_;
    zeigen(schweiss_, schweiss);
    if (schweiss) {  // rinnt langsam die Schläfe hinunter, dann von vorn
      int y = (int) (jetzt / 60 % 18);
      lv_obj_set_pos(schweiss_, p.auge_x[1] + 26, p.auge_y - 38 + y);
    }
    zeigen(traene_, traene);
    if (traene) {  // kullert vom Auge bis zur Wange
      int y = (int) (jetzt / 45 % 30);
      lv_obj_set_pos(traene_, p.auge_x[0] - 4, p.auge_y + 12 + y);
    }
    for (auto &w : roet_)
      for (auto *s : w) zeigen(s, roet);
    if (roet != roet_an_) {  // Wangen kräftig rosa; aus: ausdruck_setzen stellt die normale Deckkraft wieder her
      roet_an_ = roet;
      for (int i = 0; i < 2; i++) {
        lv_obj_set_style_bg_color(wange_[i], lv_color_hex(roet ? 0xFF5A7A : WANGE), 0);
        int g = roet ? 8 : 0;  // beim Erröten größer
        lv_obj_set_size(wange_[i], plan_->wange_b + g, plan_->wange_h + g / 2);
        lv_obj_set_pos(wange_[i], plan_->wange_x[i] - g / 2, plan_->wange_y - g / 4);
      }
      if (roet)
        for (auto *w : wange_) lv_obj_set_style_bg_opa(w, LV_OPA_COVER, 0);
      else
        ausdruck_setzen(ausdruck_);
    }
    for (int i = 0; i < 4; i++) {  // Sternchen blinken versetzt
      zeigen(funkel_[i], funkel && ((jetzt / 180 + i) % 3) != 0);
    }
  }

  void timer_tick(uint32_t jetzt) {
    if (timer_ende_ && !timer_klingelt_) {
      int rest = (int) (((int32_t) (timer_ende_ - jetzt) + 999) / 1000);
      if (rest <= 0) {
        timer(-1, timer_name_);  // eigener Countdown fertig; HA meldet das Klingeln auch
      } else if (rest <= 5 && rest != timer_zahl_) {  // letzte 5 s: großes Zählen
        timer_zahl_ = rest;
        zeitweise(UEBERRASCHT, 1100);
        anim(cb_hop, 0, 8, 120, 120, 1, lv_anim_path_ease_out);
        zeige_schild(esphome::str_sprintf("%d", rest), 1);
        if (ziffer_font_) lv_obj_set_style_text_font(schild_text_, ziffer_font_, 0);  // große Countdown-Ziffer
      } else if (rest > 60 && vorbei(jetzt, naechste_timer_info_) && !schild_bis_ && !gedrueckt_) {
        naechste_timer_info_ = jetzt + 90000;
        anim(cb_blick_x, blick_x_, -12, 250);
        naechster_blick_ = jetzt + 2500;
        zeige_schild(art_ == ROBOTER ? esphome::str_sprintf("Noch %d s. Exakt.", rest) : "Noch " + dauer_text(rest), 3);
      }
    }
    if (timer_klingelt_) {
      if (vorbei(jetzt, klingel_bis_)) {
        timer_klingelt_ = false;
        return;
      }
      if (vorbei(jetzt, naechstes_klingeln_)) {
        naechstes_klingeln_ = jetzt + 1300;
        zeitweise(UEBERRASCHT, 1400);
        anim(cb_hop, 0, 18, 160, 200, 1, lv_anim_path_ease_out);
        zappeln_bis_ = jetzt + 1300;
        if (led) led((klingel_takt_++ % 2) ? 0xFF4A1A : 0xFFB020, 0.8f);
        led_aus_bei_ = jetzt + 600;
        if (!schild_bis_) zeige_schild(timer_name_.empty() ? std::string("Timer!") : timer_name_ + "!", 2);
      }
    }
  }

  // Okti-Arme: Phase weiterdrehen und Punkte neu setzen (alle 100 ms, beim Zappeln/Greifen alle 50 ms)
  void tentakel_tick(uint32_t jetzt);

  // Roboterarme: Schulter → Ellbogen → Hand; hängen, halten das Schild, winken (rechts), greifen zum Finger
  void roboter_arme();

  // Fuchsschwanz: buschig, nach oben gebogen, wedelt (stärker, wenn er sich freut oder zappelt); weiße Spitze
  void fuchs_schwanz();

  void tentakel_zeichnen();

  void augen_layout();
  void gesicht_layout() { lv_obj_set_pos(gesicht_, blick_x_, blick_y_); }
  void koerper_layout();
  void arme_layout();
  void schild_layout() { lv_obj_set_y(schild_, schild_y_); }
  void hut();
  void deko_anwenden();

  // Hintergrund-Deko (Bildschirm-Koordinaten), liegt hinter der Figur
  void deko_bauen();

  // --- Wetter-Ansicht (hinter der Figur): Sonne oben rechts unter der Uhr, zwei Wolken, Regen/Schnee, Nebel, Blitz ---
  void wetter_bauen();

  void wetter_anwenden();

  void wetter_tick(uint32_t jetzt);

  void fledermaeuse_setzen(int schlag) {  // Flügel: V (oben) oder flach
    for (int i = 0; i < 3; i++) {
      int x = fleder_pos_[i][0], y = fleder_pos_[i][1], h = ((schlag + i) % 2) ? -6 : 3;
      fluegel_pkt_[i][0].x = x - 12;
      fluegel_pkt_[i][0].y = y + h;
      fluegel_pkt_[i][1].x = x - 5;
      fluegel_pkt_[i][1].y = y - 1;
      fluegel_pkt_[i][2].x = x;
      fluegel_pkt_[i][2].y = y + 2;
      fluegel_pkt_[i][3].x = x + 5;
      fluegel_pkt_[i][3].y = y - 1;
      fluegel_pkt_[i][4].x = x + 12;
      fluegel_pkt_[i][4].y = y + h;
      lv_line_set_points(fluegel_[i], fluegel_pkt_[i], 5);
    }
  }

  // Deko bewegen: Fledermäuse flattern, Lichter funkeln, Schnee fällt
  void deko_tick(uint32_t jetzt);

  Art art_{OKTI};
  const Bauplan *plan_{&PLAN_OKTI};
  lv_obj_t *root_{}, *uhr_{}, *schatten_{}, *koerper_{}, *gesicht_{}, *kuerbis_{}, *stiel_{}, *antenne_{}, *kopf_{};
  lv_obj_t *arm_[2]{}, *wange_[2]{}, *auge_[2]{}, *glanz_[2]{}, *froh_[2]{}, *spirale_[2]{}, *braue_[2]{};
  lv_obj_t *mund_bogen_{}, *mund_o_{}, *mund_strich_{}, *zzz_{}, *schild_{}, *schild_text_{};
  lv_obj_t *tentakel_[ARME]{}, *unterseite_[ARME]{};
  lv_point_precise_t arm_pkt_[ARME][GLIEDER + 1]{}, unten_pkt_[ARME][GLIEDER + 1]{}, braue_pkt_[2][2]{};

  bool sichtbar_{false}, gedrueckt_{false}, streichel_aktiv_{false}, halloween_{false};
  int stimmung_{0}, farb_st_{-1}, anspannung_{20}, energie_{70};
  bool weihnacht_{false};
  int monat_{0}, tag_{0}, deko_modus_{0}, deko_takt_{0};
  uint32_t letzte_deko_{0}, letzte_schnee_{0}, letzte_laub_{0};
  static const int TEILCHEN = 18, SONNE_X = 286, SONNE_Y = 58;
  lv_obj_t *wetter_{}, *sonne_{}, *strahl_[8]{}, *nebel_[3]{}, *wolke_[2]{}, *wolke_teil_[2][3]{}, *tropfen_[TEILCHEN]{},
      *blitz_{};
  lv_point_precise_t strahl_pkt_[8][2]{}, blitz_pkt_[5]{};
  float wolke_x_[2]{}, tropfen_x_[TEILCHEN]{}, tropfen_y_[TEILCHEN]{};
  uint8_t tropfen_art_[TEILCHEN]{};
  bool wetter_wind_{false}, wetter_blitz_{false};
  uint32_t letztes_wetter_{0}, blitz_an_{0}, naechster_blitz_{0};
  std::string wetter_ha_, wetter_test_, wetter_alt_;
  int wetter_laune_{0};
  lv_obj_t *schweiss_{}, *traene_{}, *roet_[2][3]{}, *funkel_[4]{};
  uint32_t schweiss_bis_{0}, traene_bis_{0}, roet_bis_{0}, funkel_bis_{0};
  bool roet_an_{false};
  uint32_t patzer_bei_{0};
  int lw_stufe_{0}, versoehnt_{0};
  bool schmollen_{false};
  lv_obj_t *tinte_[6]{}, *spritzer_[8]{};
  uint32_t tinte_start_{0};
  bool tinte_weg_{false};
  static const int FAUST_Y = 196;
  lv_obj_t *faust_{}, *faust_arm_{}, *knoechel_[3]{}, *funken_[8]{};
  uint32_t faust_start_{0}, faust_bis_{0}, funken_start_{0};
  int zahl_[4]{};
  bool verreist_{false};
  bool schlaeft_{false};
  lv_obj_t *netz_symbol_{}, *netz_text_{};
  int netz_stufe_{0};
  std::vector<std::string> fern_;

 public:
  lv_obj_t *hilfe_knopf_{}, *hilfe_tafel_{}, *schmuck_{}, *schmuck_glanz_{}, *ohrstecker_[2]{};
  bool schmuck_an_{false};
  uint32_t hilfe_bis_{0}, spiele_bis_{0};
  lv_obj_t *spiele_knopf_{}, *spiele_tafel_{};
  uint32_t schreck_bei_{0}, herzen_start_{0}, jongl_start_{0}, jagen_start_{0}, prop_start_{0};
  bool jongl_fallen_{false};
  lv_obj_t *ball_[3]{}, *prop_[2]{};
  lv_point_precise_t prop_pkt_[2][2]{};
  lv_obj_t *herz_g_[6]{}, *brief_siegel_{};
  uint32_t knoten_bis_{0}, knoten_los_{0}, faecher_schwanz_bis_{0}, klappe_start_{0};
  lv_obj_t *zahnrad_[2]{};
  lv_obj_t *bett_{}, *standby_{};
  lv_obj_t *matrix_[14]{};
  float matrix_y_[14]{};
  uint32_t matrix_bis_{0}, letzte_matrix_{0};
  lv_obj_t *schwanz_rand_{}, *schwanz_bauch_{}, *saugnapf_[ARME][3]{};
  lv_obj_t *huegel_{}, *visier_{}, *scan_[3]{};
  uint32_t maus_start_{0}, glitch_start_{0};
  int maus_y_{0};
  bool maus_raus_{false};
  std::string karte_;
  lv_obj_t *koffer_{}, *zettel_text_{};
  lv_obj_t *status_{}, *status_sym_[4]{}, *status_zahl_[4]{};
  const lv_font_t *status_font_{}, *ziffer_font_{}, *schild_font_{};
  int status_stand_{-1};
  int patzer_art_{0};
  lv_obj_t *deko_h_{}, *deko_w_{}, *muetze_{}, *fluegel_[3]{}, *kugel_[6]{}, *birne_[12]{}, *flocke_[14]{};
  lv_point_precise_t fluegel_pkt_[3][5]{}, kette_pkt_[9]{};
  int fleder_pos_[3][2]{};
  float flocke_x_[14]{}, flocke_y_[14]{};
  Ausdruck ausdruck_{NORMAL}, basis_{NORMAL};
  int augen_w_{24}, augen_h_{40};
  int blinzel_{0}, blick_x_{0}, blick_y_{0}, hop_{0}, atem_{0}, wackel_{0}, arm_pos_{0}, schild_y_{SCHILD_WEG};
  int greif_arm_{-1}, greif_x_{0}, greif_y_{0};
  float phase_{0};
  uint32_t ausdruck_bis_{0}, schild_bis_{0}, led_aus_bei_{0}, nachher_bei_{0}, streichel_bis_{0};
  uint32_t naechstes_blinzeln_{0}, naechster_blick_{0}, naechste_idee_{0}, letzte_beruehrung_{0};
  int nabu_blick_x_{-12}, nabu_blick_y_{-4};
  uint32_t stups_start_{0}, nabu_ruhe_{0}, winken_bis_{0}, zappeln_bis_{0}, letzte_tentakel_{0};
  int stupser_{0};
  lv_point_t start_{};
  std::string nachher_text_;
  std::deque<std::string> post_, schilder_;
  int schild_dauer_{4};
  int rufe_{0};
  lv_obj_t *brief_{};
  lv_point_precise_t brief_klappe_[3]{};
  lv_obj_t *brief_klappe_obj_{};
  bool arm0_vorn_{false};
  uint32_t brief_offen_bis_{0};
  uint32_t naechster_ruf_{0}, abgeholt_bis_{0};
  uint32_t timer_ende_{0}, naechste_timer_info_{0}, klingel_bis_{0}, naechstes_klingeln_{0};
  bool timer_klingelt_{false};
  int timer_zahl_{-1}, klingel_takt_{0};
  std::string timer_name_;
  std::string frage_text_, frage_opt_[2];
  bool frage_aktiv_{false}, frage_gezeigt_{false};
  uint32_t frage_bis_{0};
  lv_obj_t *frage_box_{}, *frage_lbl_[2]{};
  Spiel spiel_{KEIN_SPIEL};
  uint32_t reaktion_bereit_{0}, reaktion_los_{0}, ttt_yellow_bei_{0}, ttt_ende_bei_{0};
  int ttt_[9]{}, ttt_ergebnis_{0};
  uint32_t letzter_zug_{0};
  lv_obj_t *spiel_box_{}, *ttt_lbl_[9]{};
  lv_obj_t *simon_box_{}, *simon_feld_[4]{};
  lv_obj_t *brille_{}, *schal_{}, *schirm_{};
  lv_point_precise_t schirm_stiel_[2]{}, kanne_tuelle_[2]{};
  lv_obj_t *korb_{}, *tonne_{}, *kissen_{}, *nabu_mini_{};
  lv_obj_t *robo_arm_[2]{}, *robo_hand_[2]{}, *herz_{}, *schwanz_{}, *schwanz_spitze_{}, *pfote_[2]{}, *spitze_[5]{};
  lv_point_precise_t spitze_pkt_[5][2]{};
  lv_point_precise_t robo_pkt_[2][3]{}, schwanz_pkt_[5]{}, ohr_pkt_[2][2][5][2]{};
  // Ohrspitzen [Ohr][außen/innen][x,y]
  static constexpr int OHR_SPITZE[2][2][2] = {{{34, -18}, {36, -4}}, {{146, -18}, {144, -4}}};
  uint32_t nabu_gruss_bei_{0}, nabu_geht_bei_{0}, nabu_weg_bei_{0}, welk_bis_{0};
  int besuch_x_{0};
  uint32_t besuch_los_{0}, besuch_zurueck_{0};
  lv_obj_t *faecher_{}, *kanne_{}, *tuete_{}, *rauch_[3]{}, *hut_glyph_{}, *deko_g_[8]{};
  const lv_font_t *deko_font_{};
  const DekoSatz *deko_satz_{};
  int anlass_zahl_{0};
  bool rosa_{false};
  std::string deko_key_;
  std::string anlass_, deko_test_, letztes_schild_;
  uint32_t letztes_schild_zeit_{0};
  const char *accessoire_{""};
  uint32_t accessoire_farbe_{0};
  int requisit_{0};
  uint32_t requisit_bis_{0};
  int simon_folge_[32]{}, simon_len_{1}, simon_idx_{0}, simon_pos_{0}, simon_an_{-1}, simon_phase_{0};
  uint32_t simon_naechst_{0}, simon_aus_bei_{0};
};

inline Figur figur;

}  // namespace yellow

#include "yellow_spiele.h"
#include "yellow_spezial.h"
#include "yellow_deko.h"
#include "yellow_koerper.h"
#include "yellow_post.h"
