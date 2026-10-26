#pragma once
// Yellow: small mute character for the CYD (LVGL 9, landscape 320×240).
// Builds itself as a child of an LVGL object (which also gets the touch events) and moves
// only with lv_anim and line points, without bitmaps. Three characters: Okti (reversible octopus,
// yellow/happy or red/grumpy, 8 arms), Robot and Fox.
// Building blocks: shapes, arcs, lines. Anything that touches LVGL only after build() (see ready()).
// The sign (object + label with its own font) comes from the YAML because the fonts are defined there.

#include "yellow_data.h"

namespace yellow {

class Companion {
 public:
  // LED on the back: color 0xRRGGBB, brightness 0..1 (0 = off)
  std::function<void(uint32_t, float)> led;
  // a message was shown after a tap (text), for the event "read" in HA
  std::function<void(const std::string &)> read;
  // Yellow was tapped while a timer rings (HA then stops the voice assistant's timer)
  std::function<void()> timer_off;
  // Yellow was woken by a tap (the YAML then resets the switch "Yellow sleeps")
  std::function<void()> woke_up;
  // Yellow was petted; text of the sign if one was visible in the last 15 s (= favourite saying)
  std::function<void(const std::string &)> petted;
  // a tap; returns true if the YAML showed the first-tap-of-the-day greeting instead (then no other reaction)
  std::function<bool()> first_tap;

  // Jewelry (bling, combines with hat/crown): golden chain with a sparkling pendant, fox with earrings
  void jewelry(bool on) {
    jewelry_on_ = on;
    if (jewelry_ == nullptr) return;
    show(jewelry_, on);
    for (auto *o : earrings_) show(o, on);
  }
  // Accessory when no occasion hat is active: hat, crown, star, flower, none
  void accessory(const std::string &name) {
    accessory_ = name == "hat" ? "\U000F0BA4" : name == "crown" ? "\U000F01A5" : name == "star" ? "\U000F04CE"
                : name == "flower" ? "\U000F024A" : "";
    accessory_color_ = name == "hat" ? 0x2A2A36 : name == "crown" ? 0xFFD23F : name == "star" ? 0xFFE08A : 0xFF8FA3;
    hat();
  }
  // answer to a question (text of the chosen option, empty = no answer in time)
  std::function<void(const std::string &)> answer;
  // game over: game (reaction, ttt, simon), value (ms, or 1 won / 0 draw / -1 lost / -2 aborted)
  std::function<void(const std::string &, int)> game_over;

  bool game(const std::string &name);

  // Question with two answer fields at the bottom (options "Yes|No"). Waits until Yellow is visible; gone after 10 min without an answer.
  void question(const std::string &text, const std::string &options);

  // Timer from the voice assistant: sec > 0 running (seconds left), 0 = over/cancelled, < 0 = ringing now
  void timer(int sec, const std::string &name) {
    uint32_t now_ms = esphome::millis();
    timer_name_ = name;
    if (sec > 0) {
      bool fresh = timer_end_ == 0;
      timer_end_ = now_ms + (uint32_t) sec * 1000;
      timer_ringing_ = false;
      timer_count_ = -1;
      next_timer_info_ = now_ms + 90000;
      if (fresh && visible_) {
        anim(cb_look_y, 0, 7, 180, 180, 2);  // nod
        show_sign((name.empty() ? std::string(txt::TIMER_PREFIX) : name + ": ") + duration_text(sec), 3);
      }
    } else if (sec == 0) {
      bool rang = timer_ringing_;
      timer_end_ = 0;
      timer_ringing_ = false;
      if (rang && visible_) for_a_while(NORMAL, 100);
    } else {
      timer_end_ = 0;
      if (!timer_ringing_) {
        timer_ringing_ = true;
        ring_until_ = now_ms + 120000;
        next_ring_ = now_ms;
      }
    }
  }

  void build(lv_obj_t *surface, lv_obj_t *sign, lv_obj_t *sign_text) {
    sign_ = sign;
    sign_text_ = sign_text;
    root_ = shape(surface, 0, 0, 320, 240, BACKGROUND, 0);
    lv_obj_move_background(root_);  // assist panel and sign lie on top
    lv_obj_add_flag(root_, LV_OBJ_FLAG_HIDDEN);

    clock_ = lv_label_create(root_);
    lv_obj_set_pos(clock_, 258, 8);
    lv_obj_set_style_text_color(clock_, lv_color_hex(0x5A6274), 0);
    lv_label_set_text(clock_, "");

    build_decor();
    shadow_ = shape(root_, 95, 222, 130, 12, SHADOW);
    lv_obj_add_event_cb(surface, cb_touch, LV_EVENT_ALL, this);
    build_figure();
    build_help();
    // mini Nabu for visits: grey head with a dark visor, two glowing eyes, antenna
    nabu_mini_ = shape(root_, 330, 150, 52, 64, 0, 0);
    lv_obj_set_style_bg_opa(nabu_mini_, LV_OPA_TRANSP, 0);
    shape(nabu_mini_, 24, 0, 4, 12, METAL, 2);
    shape(nabu_mini_, 20, -4, 12, 12, 0xFF5A7A);
    lv_obj_t *nk = shape(nabu_mini_, 0, 10, 52, 40, 0xC8CCD8, 12);
    lv_obj_t *nv = shape(nk, 6, 7, 40, 24, VISOR, 8);
    shape(nv, 8, 7, 8, 10, GLOW);
    shape(nv, 24, 7, 8, 10, GLOW);
    shape(nabu_mini_, 10, 50, 10, 12, METAL, 3);
    shape(nabu_mini_, 32, 50, 10, 12, METAL, 3);
    show(nabu_mini_, false);

    // answer fields for questions (bottom left/right)
    question_box_ = shape(root_, 0, 186, 320, 54, 0, 0);
    lv_obj_set_style_bg_opa(question_box_, LV_OPA_TRANSP, 0);
    static const uint32_t COLOR[2] = {0x2E9E5A, 0xC8463C};
    for (int i = 0; i < 2; i++) {
      lv_obj_t *b = shape(question_box_, i ? 166 : 10, 2, 144, 46, COLOR[i], 14);
      border(b, 2, 0xF4E4C1);
      question_lbl_[i] = lv_label_create(b);
      lv_obj_set_style_text_color(question_lbl_[i], lv_color_hex(0xFFFFFF), 0);
      lv_obj_center(question_lbl_[i]);
    }
    show(question_box_, false);

    // Tic-tac-toe: dimmed background, 3×3 fields; font as on the sign
    game_box_ = shape(root_, 0, 0, 320, 240, 0x0B0D13, 0);
    lv_obj_set_style_bg_opa(game_box_, 200, 0);
    const lv_font_t *big = lv_obj_get_style_text_font(sign_text_, LV_PART_MAIN);
    for (int i = 0; i < 9; i++) {
      lv_obj_t *z = shape(game_box_, 70 + (i % 3) * 62, 30 + (i / 3) * 62, 58, 58, 0xF4E4C1, 10);
      ttt_lbl_[i] = lv_label_create(z);
      lv_obj_set_style_text_font(ttt_lbl_[i], big, 0);
      lv_obj_center(ttt_lbl_[i]);
    }
    show(game_box_, false);

    // Simon Says: four big colour fields
    simon_box_ = shape(root_, 0, 0, 320, 240, 0x0B0D13, 0);
    lv_obj_set_style_bg_opa(simon_box_, 220, 0);
    for (int i = 0; i < 4; i++) simon_field_[i] = shape(simon_box_, (i % 2) ? 166 : 12, (i / 2) ? 126 : 14, 142, 100, SIMON_DARK[i], 18);
    show(simon_box_, false);
  }

  // change the character (works any time, also before build())
  void kind(Kind a) {
    if (a == kind_ && body_ != nullptr) return;
    if (root_ == nullptr) {
      kind_ = a;
      return;
    }
    if (mini_ != nullptr) {  // the night mini shows the old character: rebuilt by the next view run
      lv_obj_delete(mini_);
      mini_ = mini_body_ = mini_z_ = mini_dot_ = nullptr;
      mini_on_ = false;
    }
    bool was_on = visible_;
    visible(false);  // still with the old character: color() would otherwise touch Okti arms that do not exist (crash 2026-10-25)
    kind_ = a;
    build_figure();
    visible(was_on);
  }

  bool is_visible() const { return visible_; }
  bool is_away() const { return away_; }
  // Nobody may touch LVGL objects before build() (on_boot). Selects fire on_value already
  // during setup, before LVGL: that hung the board (task WDT, boot loop, 2026-10-24).
  bool ready() const { return root_ != nullptr; }
  // a swipe is running or just ended: ignore taps in the YAML then
  bool petting() const {
    return petting_active_ || (petting_until_ && !passed(esphome::millis(), petting_until_));
  }

  void visible(bool s) {
    if (!ready() || s == visible_) return;
    visible_ = s;
    lv_anim_delete(this, nullptr);
    blink_ = hop_ = breath_ = wobble_ = look_x_ = look_y_ = arm_pos_ = visit_x_ = 0;
    visit_leave_ = visit_return_ = 0;
    sign_y_ = SIGN_HIDDEN;
    sign_until_ = expression_until_ = later_at_ = wave_until_ = wiggle_until_ = 0;
    signs_.clear();
    next_call_ = esphome::millis() + 2000;  // waiting post announces itself after appearing
    if (game_ != NO_GAME) {
      show(game_box_, false);
      show(simon_box_, false);
      end_game(game_ == REACTION ? "reaction" : game_ == TTT ? "ttt" : "simon", -2);
    }
    question_shown_ = false;
    show(question_box_, false);
    petting_active_ = pressed_ = false;
    grab_arm_ = -1;
    if (s) {
      uint32_t now_ms = esphome::millis();
      lv_obj_clear_flag(root_, LV_OBJ_FLAG_HIDDEN);
      lv_obj_clear_flag(sign_, LV_OBJ_FLAG_HIDDEN);
      start_breathing();
      next_blink_ = now_ms + 1500;
      next_look_ = now_ms + 2500;
      next_idea_ = now_ms + 15000;
      last_touch_ = now_ms;
      base_ = NORMAL;
      bored_level_ = 0;
      sulking_ = false;
      if (sleeping_) {
        sleep_scene();
      } else {  // comes in: fox walks, Okti crawls (arms wiggle), robot rolls
        anim(cb_visit, -340, 0, kind_ == OKTI ? 2600 : 2000, 0, 1, lv_anim_path_ease_out);
        if (kind_ == OKTI) wiggle_until_ = now_ms + 2600;
      }
    } else {
      lv_obj_add_flag(root_, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(sign_, LV_OBJ_FLAG_HIDDEN);
      if (led_off_at_ && led) led(0, 0);
      led_off_at_ = 0;
    }
    apply_expression(base_);
    body_layout();
    face_layout();
    arms_layout();
    sign_layout();
    draw_tentacles();
  }

  void clock(int hour_, int minute) {
    if (clock_) lv_label_set_text(clock_, esphome::str_sprintf("%02d:%02d", hour_, minute).c_str());
  }
  // season from the date (Auto) or fixed via the select "Decoration": 0 Auto, 1 Off, 2 Halloween, 3 Christmas
  void season(int month, int day);
  void decor(int mode);
  // today's occasion from HA ("easter", "birthday|Name", "autumn" …)
  void occasion(const std::string &a);
  void decor_test(const std::string &key) {  // for trying out, empty = back to normal
    decor_test_ = key;
    apply_decor();
  }
  void decor_font(const lv_font_t *f) { decor_font_ = f; }
  // condition like HA weather (sunny, rainy, pouring, lightning-rainy, snowy, fog, windy …); test wins, empty = HA
  void weather(const std::string &condition) { weather_ha_ = condition; apply_weather(); }
  void weather_test(const std::string &condition) { weather_test_ = condition; apply_weather(); }

  // single tap
  void poke() {
    if (!visible_) return;
    uint32_t now_ms = esphome::millis();
    if (collected_until_ && !passed(now_ms, collected_until_)) return;  // the tap collected a message
    if (away_) return;
    if (sleeping_) {  // a tap wakes him
      sleep(false);
      if (woke_up) woke_up();
      if (first_tap && first_tap()) later_at_ = 0;  // the greeting replaces "Morning already?"
      return;
    }
    counts_[TAPS]++;
    if (sulking_) {  // a tap is not enough, petting is
      static const char *const huff[] = {txt::HUFF, txt::OFFENDED, txt::NOW_ALL_OF_A_SUDDEN, txt::PET_TWICE};
      for_a_while(ANNOYED, 1500);
      anim(cb_look_x, look_x_, -12, 200);
      show_sign(huff[rnd(4)], 2);
      return;
    }
    if (first_tap && first_tap()) {
      for_a_while(HAPPY, 2500);
      wave(2500);
      return;
    }
    if (now_ms - poke_start_ > 3000) {
      poke_start_ = now_ms;
      pokes_ = 0;
    }
    ESP_LOGD("yellow", "Tap %d in %u ms", pokes_ + 1, (unsigned) (now_ms - poke_start_));
    if (++pokes_ >= 4 && kind_ == OKTI && rnd(2)) {  // Okti: every other time an ink cloud instead of dizzy
      pokes_ = 0;
      ink();
      later_text_ = txt::PLEASE_DONT;
      later_at_ = now_ms + 2600;
    } else if (pokes_ >= 4 && kind_ == ROBOT && rnd(2)) {  // Robot: every other time a visor glitch
      pokes_ = 0;
      glitch();
    } else if (pokes_ >= 4) {
      pokes_ = 0;
      for_a_while(DIZZY, 3200);
      anim(cb_wobble, 0, 1000, 1600);
      wiggle_until_ = now_ms + 3000;
      led_flash(0x8A4DFF, 3000);
      later_text_ = txt::PLEASE_DONT;
      later_at_ = now_ms + 3300;
    } else {
      for_a_while(HAPPY, 1300);
      anim(cb_hop, 0, 14, 160, 180, 1, lv_anim_path_ease_out);
      wiggle_until_ = now_ms + 900;
      led_flash(plan_->light, 400);
    }
  }

  void show_sign(const std::string &text, int seconds);

  // Imperfect on purpose: sign upside down (then turns it) or with swapped letters (corrects it, annoyed)
  void sign_blunder(const std::string &text);
  void blunder_tick(uint32_t now_ms);

  // Show a message. wait = attention first (waving, "!", LED pulses), the message only after a tap.
  // Long texts are spread over several signs. Waiting messages survive the clock view
  // and the dark; Yellow announces them as soon as he is visible again. At most 5 in the queue.
  void message(const std::string &text, bool wait, int seconds = 4);
  int waiting() const { return (int) post_.size(); }
  // remove a question silently (without an answer event)
  void cancel_question();
  // post via HA: show the next message (like a tap) or drop all
  void play_post();
  void clear_post();
  // Mood 0 (yellow, happy) … 100 (red, grumpy), slider in HA. Annoyed/sad pull it to 100 for a while.
  void mood(int v) {
    mood_ = std::max(0, std::min(100, v));
    if (ready()) apply_expression(expression_);
  }
  // Tension 0 (relaxed) … 100 (very nervous): arm/tail pace, blinking, look, breathing, sweat, flinching
  void tension(int v) {
    tension_ = std::max(0, std::min(100, v));
    if (ready() && visible_) start_breathing();
  }
  // Energy 0 (dead tired) … 100 (wide awake): breathing pace, yawning, faster boredom, half-closed eyes
  void energy(int v) {
    energy_ = std::max(0, std::min(100, v));
    if (!ready()) return;
    if (visible_) start_breathing();
    // very tired: eyes half closed for good; awake again: back (unless the boredom stairs already set "tired")
    if (energy_ < 20 && base_ == NORMAL) base_ = TIRED;
    else if (energy_ >= 20 && base_ == TIRED && bored_level_ < 3) base_ = NORMAL;
    if (!expression_until_ && body_) apply_expression(base_);
  }
  // breathing: tired = slow and deep, nervous = fast and shallow
  void start_breathing() {
    int ms = 1300 + (100 - energy_) * 14 - tension_ * 6;
    int deep = energy_ < 30 ? 4 : tension_ > 70 ? 2 : 3;
    anim(cb_breath, 0, deep, ms, ms, LV_ANIM_REPEAT_INFINITE);
  }
  static std::string duration_text(int sec) {
    if (sec >= 90) return esphome::str_sprintf("%d min", (sec + 30) / 60);
    return esphome::str_sprintf("%d s", sec);
  }
  // Show a prop: sunglasses, umbrella, scarf, none; seconds 0 = stays
  // Yellow is away: character gone, suitcase with stickers and a note (text = last postcard); comes back with a souvenir
  void away(bool on, const std::string &card);

  bool prop(const std::string &name, int seconds) {
    int r = name == "sunglasses" ? 1 : name == "umbrella" ? 2 : name == "scarf" ? 3 : name == "fan" ? 4
          : name == "watering_can" ? 5 : name == "joint" ? 6 : name == "laundry_basket" ? 7 : name == "trash_can" ? 8
          : name == "pillow" ? 9 : name == "none" ? 0 : -1;
    if (r < 0) return false;
    if (r == 4 && kind_ == FOX) {  // the fox needs no fan: fans with his tail
      tail_fan();
      return true;
    }
    prop_id_ = r;
    prop_until_ = (r && seconds > 0) ? esphome::millis() + seconds * 1000 : 0;
    props_layout();
    return true;
  }
  // where paw/hand i (0 left, 1 right) holds a prop (body coordinates); false = hand free
  bool grip(int i, int &x, int &y) const {
    static const int G[10][2][3] = {
        {{0}, {0}}, {{0}, {0}}, {{0}, {1, 186, 100}}, {{0}, {0}}, {{1, -4, 106}, {0}}, {{0}, {1, 180, 108}},
        {{0}, {0}}, {{1, 48, 132}, {1, 132, 132}}, {{0}, {1, 196, 96}}, {{1, 4, 72}, {0}}};
    if (prop_id_ < 0 || prop_id_ > 9 || !G[prop_id_][i][0]) return false;
    x = G[prop_id_][i][1];
    y = G[prop_id_][i][2];
    return true;
  }
  void props_layout() {
    show(glasses_, prop_id_ == 1);
    show(umbrella_, prop_id_ == 2);
    show(scarf_, prop_id_ == 3);
    show(fan_, prop_id_ == 4);
    show(can_, prop_id_ == 5);
    show(joint_, prop_id_ == 6);
    for (auto *r : smoke_) show(r, prop_id_ == 6);
    show(basket_, prop_id_ == 7);
    show(bin_, prop_id_ == 8);
    show(pillow_, prop_id_ == 9);
  }
  // Love letters are post with the prefix LOVE_PREFIX: pink envelope, heart seal, hearts when opened
  static bool is_love(const std::string &t) { return t.rfind(LOVE_PREFIX, 0) == 0; }
  void letter_layout() {
    show(letter_, !post_.empty() || (letter_open_until_ && !passed(esphome::millis(), letter_open_until_)));
    if (letter_ == nullptr) return;
    bool love = !post_.empty() && is_love(post_.front());
    lv_obj_set_style_bg_color(letter_, lv_color_hex(love ? 0xFFC8D8 : 0xF4E4C1), 0);
    if (letter_seal_) {
      lv_obj_set_style_bg_color(letter_seal_, lv_color_hex(love ? 0xFF2A5A : RED), 0);
      lv_obj_set_size(letter_seal_, love ? 11 : 9, love ? 11 : 9);
    }
  }
  // flap closed (V down) or open (tip up); the line sits 14 px higher so "open" fits in
  void flap(bool open) {
    if (letter_flap_obj_ == nullptr) return;
    letter_flap_[0].x = 0;
    letter_flap_[0].y = 14;
    letter_flap_[1].x = 21;
    letter_flap_[1].y = open ? 0 : 29;
    letter_flap_[2].x = 42;
    letter_flap_[2].y = 14;
    lv_line_set_points(letter_flap_obj_, letter_flap_, 3);
  }

  void set_expression(const std::string &name, int seconds) {
    if (!visible_) return;
    static const char *const names[] = {"normal", "happy", "surprised", "dizzy", "tired",
                                        "annoyed", "sad", "asleep", "yawn"};
    for (int i = 0; i <= YAWN; i++) {
      if (name != names[i]) continue;
      if (seconds <= 0) {  // permanent, until the next touch
        base_ = (Expression) i;
        expression_until_ = 0;
        apply_expression(base_);
      } else {
        for_a_while((Expression) i, seconds * 1000);
      }
      if (i == DIZZY) anim(cb_wobble, 0, 1000, 1600);
      return;
    }
  }

  // single gesture (for HA/LLM): hop, wobble, wiggle, nod, shake_head, wave, startle, oracle …
  bool gesture(const std::string &g) {
    if (!visible_) return false;
    uint32_t now_ms = esphome::millis();
    if (g == "hop") anim(cb_hop, 0, 14, 160, 180, 2, lv_anim_path_ease_out);
    else if (g == "wobble") anim(cb_wobble, 0, 1000, 1500);
    else if (g == "wiggle") wiggle_until_ = now_ms + 2000;
    else if (g == "nod") anim(cb_look_y, 0, 7, 180, 180, 2);
    else if (g == "shake_head") anim(cb_shake, 0, 1000, 1200);
    else if (g == "wave") wave(2500);
    else if (g == "ink" && kind_ == OKTI) ink();
    else if (g == "fist_bump") fist_bump();
    else if (g == "special") special();
    else if (g == "mouse_dive" && kind_ == FOX) mouse_dive();
    else if (g == "glitch" && kind_ == ROBOT) glitch();
    else if (g == "knot" && kind_ == OKTI) knot();
    else if (g == "light_scare") light_scare();
    else if (g == "hearts") hearts_start();
    else if (g == "help") help(true);
    else if (g == "juggle" && kind_ == OKTI) juggle();
    else if (g == "chase_tail" && kind_ == FOX) chase_tail();
    else if (g == "propeller" && kind_ == ROBOT) propeller();
    else if (g == "tail_fan" && kind_ == FOX) tail_fan();
    else if (g == "visor" && kind_ == ROBOT) open_visor();
    else if (g == "blunder") sign_blunder(saying());
    else if (g == "wilt") {  // arms hang limp, sad, 6 s
      for_a_while(SAD, 6000);
      wilt_until_ = now_ms + 6000;
    } else if (g == "dance") {  // happy dance: hop, wobble, all arms wiggle, 4 s
      for_a_while(HAPPY, 4000);
      anim(cb_hop, 0, 12, 180, 180, 5, lv_anim_path_ease_out);
      anim(cb_wobble, 0, 1000, 3600);
      wiggle_until_ = now_ms + 4000;
      sparkle_until_ = now_ms + 4000;
      if (led) led(0xFFD23F, 0.5f);
      led_off_at_ = now_ms + 4000;
    } else if (g == "nabu_visits") {  // mini Nabu walks in from the right, stays, Yellow waves, Nabu leaves again
      lv_obj_move_foreground(nabu_mini_);
      show(nabu_mini_, true);
      anim(cb_nabu, 330, 248, 1800, 0, 1, lv_anim_path_ease_out);
      anim(cb_look_x, look_x_, 12, 400);
      next_look_ = now_ms + 9000;
      nabu_greet_at_ = now_ms + 1900;
      nabu_leaves_at_ = now_ms + 9000;
    } else if (g == "visit_nabu") {  // "Off to see Nabu.", walks out to the right, comes back from the left after 10 s
      show_sign(txt::OFF_TO_NABU, 2);
      visit_leave_ = now_ms + 2200;
      visit_return_ = now_ms + 13000;
    } else if (g == "oracle") {
      static const char *const answers[] = {txt::ORACLE_1, txt::ORACLE_2, txt::ORACLE_3, txt::ORACLE_4, txt::ORACLE_5,
                                              txt::ORACLE_6, txt::ORACLE_7, txt::ORACLE_8, txt::ORACLE_9,
                                              txt::ORACLE_10, txt::ORACLE_11, txt::NO_COMMENT};
      for_a_while(SURPRISED, 1500);
      anim(cb_wobble, 0, 1000, 1200);
      wiggle_until_ = now_ms + 1200;
      later_text_ = answers[rnd(sizeof(answers) / sizeof(answers[0]))];
      later_at_ = now_ms + 1400;
    } else if (g == "startle" && kind_ == FOX) {
      for_a_while(SURPRISED, 700);
      anim(cb_hop, 0, 8, 120, 160, 1, lv_anim_path_ease_out);
      later_text_ = txt::NOT_SCARED;
      later_at_ = now_ms + 900;
    } else if (g == "startle") {
      for_a_while(SURPRISED, 2000);
      anim(cb_hop, 0, 26, 180, 240, 1, lv_anim_path_ease_out);
      wiggle_until_ = now_ms + 1200;
    } else
      return false;
    next_look_ = now_ms + 2500;
    return true;
  }
  // Ink cloud (Okti only): dark circles swell up, Okti disappears in them, the cloud fades,
  // Okti reappears, embarrassed (red)
  void ink();
  void ink_tick(uint32_t now_ms);

  // Fist bump: a fist comes forward (grows), sign "Fist?"; tap → "Boom!" with sparks, otherwise disappointed after 8 s
  void fist_bump();
  void fist_hit(uint32_t now_ms);
  void fist_tick(uint32_t now_ms);

  // Daily counts (Pwnagotchi principle): status line at the bottom, sayings from them, for HA as "taps|pets|games|fist"
  enum Count { TAPS, PETS, GAMES, FIST };
  std::string counts_text() const {
    return esphome::str_sprintf("%d|%d|%d|%d", counts_[TAPS], counts_[PETS], counts_[GAMES], counts_[FIST]);
  }
  void new_day() {
    for (int &z : counts_) z = 0;
  }
  std::string counts_saying() const {
    int t = counts_[TAPS], s = counts_[PETS], g = counts_[GAMES];
    switch (rnd(4)) {
      case 0:
        if (t >= 10) return esphome::str_sprintf(txt::TAPPED_TODAY, t);
        break;
      case 1:
        if (t >= 3 && s == 0) return txt::NOT_PETTED_TODAY;
        if (s >= 3) return esphome::str_sprintf(txt::PETTED_COUNT, s);
        break;
      case 2:
        if (g >= 3) return esphome::str_sprintf(txt::GAMES_TODAY, g);
        break;
      default:
        if (t == 0 && s == 0) return txt::ZERO_TOUCHES;
    }
    return "";
  }
  // status line at the bottom: symbol + number per value (taps, pets, games, fist bumps), fist only from 1
  void counts_tick() {
    static const char *const SYM[4] = {"\U000F0741", "\U000F10F1", "\U000F0297", "\U000F0FD7"};
    if (status_ == nullptr) {
      status_ = shape(root_, 0, 221, 320, 19, 0, 0);
      transparent(status_);
      lv_obj_set_flex_flow(status_, LV_FLEX_FLOW_ROW);
      lv_obj_set_flex_align(status_, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
      lv_obj_set_style_pad_column(status_, 4, 0);
      for (int i = 0; i < 4; i++) {
        status_sym_[i] = lv_label_create(status_);
        if (status_font_) lv_obj_set_style_text_font(status_sym_[i], status_font_, 0);
        lv_label_set_text(status_sym_[i], status_font_ ? SYM[i] : "");
        status_count_[i] = lv_label_create(status_);
        lv_obj_set_style_pad_right(status_count_[i], 10, 0);
        for (auto *l : {status_sym_[i], status_count_[i]}) lv_obj_set_style_text_color(l, lv_color_hex(0x5A6680), 0);
      }
      status_stand_ = -1;
    }
    int state_sum = counts_[TAPS] * 1000000 + counts_[PETS] * 10000 + counts_[GAMES] * 100 + counts_[FIST];
    if (state_sum == status_stand_) return;
    status_stand_ = state_sum;
    for (int i = 0; i < 4; i++) {
      lv_label_set_text(status_count_[i], esphome::to_string(counts_[i]).c_str());
      bool on = i < 3 || counts_[FIST] > 0;
      show(status_sym_[i], on);
      show(status_count_[i], on);
    }
  }
  void status_font(const lv_font_t *f) { status_font_ = f; }
  void digit_font(const lv_font_t *f) { digit_font_ = f; }

  // special move of the current character: Okti ink cloud, fox mouse dive, robot visor glitch
  void special();
  // Okti juggles three balls above the head; clumsy: sometimes one drops
  void juggle();
  // fox chases his own tail: runs back and forth, gets dizzy
  void chase_tail();
  // robot: propeller extends, he lifts off briefly and lands again
  void propeller();
  void special3_tick(uint32_t now_ms);
  // Okti: ties his arms in a knot, wiggles, "One moment.", shakes them loose
  void knot();
  // fox: fans himself with his tail (fast, in front of the face)
  void tail_fan();
  // robot: visor flips up, gears turn, "Maintenance.", flips closed again
  void open_visor();
  void special2_tick(uint32_t now_ms);

  // fox: jumps up and dives head first into a mound (ears and tail stick out), comes back up
  void mouse_dive();
  void mouse_tick(uint32_t now_ms);

  // robot: visor flickers, face jumps, scan lines run, briefly black, then "Rebooting… done."
  void glitch();
  void glitch_tick(uint32_t now_ms);

  // Matrix rain (13:37, the robot's nerd event): green zeros and ones fall behind the character
  void matrix(int seconds);
  void matrix_tick(uint32_t now_ms);

  // Go to sleep (switch "Yellow sleeps", also by voice): Okti in bed with a blanket, arms hanging out;
  // fox curled up, tail around the front; robot in standby (face off, standby dot pulses)
  void sleep(bool on);
  bool sleeping() const { return sleeping_; }
  void sleep_scene();
  void sleep_tick(uint32_t now_ms);
  // Tiny sleeping character next to the dim clock in the dark (#19); built once as a child of the clock view
  void night_sleeper(lv_obj_t *clock_view, bool on);
  void night_sleeper_tick(uint32_t now_ms);

  // Fright at sudden light (room was dark, lamp goes on): flinches, squeezes the eyes shut, different per character
  void light_scare();
  void scare_tick(uint32_t now_ms);
  void show_later(const char *text, uint32_t ms) {
    later_text_ = text;
    later_at_ = esphome::millis() + ms;
  }

  // hearts rise for a love letter (MDI heart from the decoration font)
  void hearts_start();
  void hearts_tick(uint32_t now_ms);

  // Help: small "?" bottom right; a tap shows how to use Yellow (a settings button will be added here later)
  void build_help();
  void help(bool on);
  bool help_open() const { return help_until_ != 0; }
  // games button bottom left: choose reaction / tic-tac-toe / Simon
  void games_menu(bool on);
  void games_tap(int y);
  bool games_open() const { return games_until_ != 0; }

  // network status next to the "?": 0 = all good, 1 = Wi-Fi missing (red, "waiting for network: …"),
  // 2 = MQTT disconnected, internet there (orange cloud), 3 = Wi-Fi without internet (orange globe)
  int network_level() const { return network_level_; }
  void network(int level, const std::string &ssid);

  // Boredom stairs since the last touch: 5 min curious, 10 min bored (taps with an arm, "…"),
  // 20 min tired, 40 min dozing, 60 min sulking (red, looks away; a tap is not enough, petting twice makes up)
  void boredom(uint32_t now_ms) {
    // tired Yellows get bored faster, awake ones slower
    uint32_t m = (now_ms - last_touch_) / 60000 * (50 + (100 - energy_)) / 100;
    int level = m >= 60 ? 5 : m >= 40 ? 4 : m >= 20 ? 3 : m >= 10 ? 2 : m >= 5 ? 1 : 0;
    if (level <= bored_level_ || sign_until_ || expression_until_ || !post_.empty() || game_ != NO_GAME) return;
    if (level >= 3 && base_ != NORMAL && base_ != TIRED && base_ != ASLEEP) return;  // set by HA: do not overwrite
    bored_level_ = level;
    switch (level) {
      case 1:
        anim(cb_look_x, look_x_, 0, 200);
        anim(cb_look_y, look_y_, 2, 200);
        next_look_ = now_ms + 4000;
        show_sign(rnd(2) ? txt::HELLO_Q : txt::ANYONE_THERE, 3);
        break;
      case 2:
        wiggle_until_ = now_ms + 1500;
        anim(cb_wobble, 0, 1000, 1200);
        show_sign(rnd(2) ? "…" : txt::BORED, 3);
        break;
      case 3:
        base_ = TIRED;
        apply_expression(base_);
        break;
      case 4:
        base_ = ASLEEP;
        apply_expression(base_);
        break;
      case 5:
        sulking_ = true;
        reconciled_ = 0;
        base_ = ANNOYED;
        apply_expression(base_);
        anim(cb_look_x, look_x_, -12, 400);
        next_look_ = now_ms + 3000;
        show_sign(txt::HUFF, 3);
        break;
    }
  }
  // test: pretend the last touch was this many minutes ago
  void boredom_test(int minutes) {
    last_touch_ = esphome::millis() - (uint32_t) minutes * 60000;
    bored_level_ = 0;
    sulking_ = false;
    base_ = NORMAL;
    expression_until_ = 0;
    if (sign_until_) sign_until_ = esphome::millis() + 10;  // clear a running sign
  }

  // remote sayings list (one line = one saying, "-" or empty = none)
  void remote_sayings(const std::string &t) {
    remote_.clear();
    size_t a = 0;
    while (t != "-" && a < t.size()) {
      size_t e = t.find('\n', a);
      if (e == std::string::npos) e = t.size();
      if (e > a) remote_.push_back(t.substr(a, e - a));
      a = e + 1;
    }
  }

  // random idle saying: every third from remote (if any), otherwise half shared, half from the character
  const char *saying() const {
    if (!remote_.empty() && rnd(3) == 0) return remote_[rnd(remote_.size())].c_str();
    if (rnd(2)) return txt::SAYINGS[rnd(sizeof(txt::SAYINGS) / sizeof(txt::SAYINGS[0]))];
    if (kind_ == ROBOT) return txt::SAYINGS_ROBOT[rnd(sizeof(txt::SAYINGS_ROBOT) / sizeof(txt::SAYINGS_ROBOT[0]))];
    if (kind_ == FOX) return txt::SAYINGS_FOX[rnd(sizeof(txt::SAYINGS_FOX) / sizeof(txt::SAYINGS_FOX[0]))];
    return txt::SAYINGS_OKTI[rnd(sizeof(txt::SAYINGS_OKTI) / sizeof(txt::SAYINGS_OKTI[0]))];
  }

  // effect for a few seconds: sweat, tear, blush, sparkle, none (0 s = 5 s)
  bool effect(const std::string &e, int seconds) {
    uint32_t until = esphome::millis() + (seconds > 0 ? seconds : 5) * 1000;
    if (e == "sweat") sweat_until_ = until;
    else if (e == "tear") tear_until_ = until;
    else if (e == "blush") blush_until_ = until;
    else if (e == "sparkle") sparkle_until_ = until;
    else if (e == "none") sweat_until_ = tear_until_ = blush_until_ = sparkle_until_ = 0;
    else return false;
    return true;
  }
  // look direction for a few seconds: left, right, up, down, center
  bool look(const std::string &r) {
    if (!visible_) return false;
    int tx = 0, ty = 0;
    if (r == "left") tx = -12;
    else if (r == "right") tx = 12;
    else if (r == "up") ty = -7;
    else if (r == "down") ty = 8;
    else if (r != "center") return false;
    anim(cb_look_x, look_x_, tx, 250);
    anim(cb_look_y, look_y_, ty, 250);
    next_look_ = esphome::millis() + 5000;
    return true;
  }

  void doorbell() {
    if (!visible_) return;
    if (kind_ == OKTI) {  // Okti scares himself into his own ink and then holds up the sign
      ink();
      later_text_ = txt::DOOR;
      later_at_ = esphome::millis() + 2500;
      led_flash(0xFFFFFF, 1500);
      return;
    }
    for_a_while(SURPRISED, 3000);
    anim(cb_hop, 0, 30, 200, 260, 1, lv_anim_path_ease_out);
    wiggle_until_ = esphome::millis() + 2000;
    if (kind_ == FOX) {  // brave: no jump, looks towards the door
      expression_until_ = 0;
      for_a_while(NORMAL, 3000);
      anim(cb_look_x, look_x_, 12, 200);
      next_look_ = esphome::millis() + 4000;
      show_sign(rnd(2) ? txt::ILL_CHECK : txt::WHO_THERE, 6);
      return;
    }
    show_sign(kind_ == ROBOT ? txt::DOOR_ROBOT : txt::DOOR, 8);
    led_flash(0xFFFFFF, 1500);
  }

  // where the voice assistant stands (from HA): left, right, up (= another room)
  void nabu_direction(const std::string &r) {
    nabu_look_x_ = r == "right" ? 12 : r == "up" ? 0 : -12;
    nabu_look_y_ = r == "up" ? -7 : -4;
  }
  // the voice assistant's LED ring goes on: Yellow looks over attentively, jealous or curious (at most every 20 s)
  void nabu_ring() {
    if (!visible_ || pressed_ || petting_active_) return;
    uint32_t now_ms = esphome::millis();
    if (nabu_quiet_ && !passed(now_ms, nabu_quiet_)) return;
    nabu_quiet_ = now_ms + 20000;
    anim(cb_look_x, look_x_, nabu_look_x_, 200);  // to where Nabu stands
    anim(cb_look_y, look_y_, nabu_look_y_, 200);
    next_look_ = now_ms + 4500;
    if (rnd(2)) {
      for_a_while(ANNOYED, 4500);
      show_sign(txt::SHOW_OFF, 3);
    } else {
      for_a_while(SURPRISED, 3500);
      anim(cb_hop, 0, 8, 150, 150, 1, lv_anim_path_ease_out);
      if (rnd(2)) show_sign(rnd(2) ? txt::OOH : txt::HOW_DO_YOU, 3);
    }
  }

  // every 50 ms from the YAML
  void tick() {
    if (mini_on_) night_sleeper_tick(esphome::millis());  // runs while Yellow is hidden
    if (!visible_) return;
    uint32_t now_ms = esphome::millis();
    if (sleeping_) {  // sleeping: only breathing, decoration and weather keep running
      sleep_tick(now_ms);
      decor_tick(now_ms);
      weather_tick(now_ms);
      tentacle_tick(now_ms);
      return;
    }
    if (away_) {  // suitcase rocks slightly, nothing else happens
      if (suitcase_) lv_obj_set_y(suitcase_, (now_ms / 700) % 2 ? -1 : 0);
      decor_tick(now_ms);
      weather_tick(now_ms);
      return;
    }
    tentacle_tick(now_ms);
    decor_tick(now_ms);
    weather_tick(now_ms);
    timer_tick(now_ms);
    effect_tick(now_ms);
    blunder_tick(now_ms);
    ink_tick(now_ms);
    fist_tick(now_ms);
    counts_tick();
    mouse_tick(now_ms);
    glitch_tick(now_ms);
    matrix_tick(now_ms);
    special2_tick(now_ms);
    scare_tick(now_ms);
    if (help_until_ && passed(now_ms, help_until_)) help(false);
    if (games_until_ && passed(now_ms, games_until_)) games_menu(false);
    if (jewelry_on_ && jewelry_shine_)  // bling: the shine dot flashes briefly
      lv_obj_set_style_bg_opa(jewelry_shine_, (now_ms / 120) % 18 < 2 ? LV_OPA_COVER : 90, 0);
    special3_tick(now_ms);
    hearts_tick(now_ms);
    if (passed(now_ms, nabu_greet_at_)) {
      nabu_greet_at_ = 0;
      for_a_while(HAPPY, 3000);
      wave(2500);
      show_sign(txt::VISITOR, 3);
    }
    if (passed(now_ms, nabu_leaves_at_)) {
      nabu_leaves_at_ = 0;
      anim(cb_nabu, 248, 330, 1800, 0, 1, lv_anim_path_ease_in);
      nabu_gone_at_ = now_ms + 1900;
    }
    if (passed(now_ms, nabu_gone_at_)) {
      nabu_gone_at_ = 0;
      show(nabu_mini_, false);
    }
    if (passed(now_ms, visit_leave_)) {
      visit_leave_ = 0;
      wiggle_until_ = now_ms + 1600;
      anim(cb_visit, 0, 340, 1600, 0, 1, lv_anim_path_ease_in);
    }
    if (passed(now_ms, visit_return_)) {
      visit_return_ = 0;
      wiggle_until_ = now_ms + 1600;
      anim(cb_visit, -340, 0, 1600, 0, 1, lv_anim_path_ease_out);
      for_a_while(HAPPY, 3000);
    }
    if (prop_id_ == 6 && joint_ != nullptr) {  // smoke rises, grows, starts again at the bottom
      for (int k = 0; k < 3; k++) {
        int t = (int) ((now_ms / 40 + k * 25) % 75);
        lv_obj_set_pos(smoke_[k], plan_->mouth_x + 32 + t / 6 + (int) (4 * sinf(t / 8.0f)) - t / 6, plan_->mouth_y - 10 - t);
        lv_obj_set_size(smoke_[k], 8 + t / 3, 8 + t / 3);
        lv_obj_set_style_bg_opa(smoke_[k], (lv_opa_t) (200 - t * 2), 0);
      }
    }
    if (prop_until_ && passed(now_ms, prop_until_)) {
      prop_until_ = 0;
      prop_id_ = 0;
      props_layout();
    }
    game_tick(now_ms);
    if (question_active_) {
      if (passed(now_ms, question_until_)) {
        end_question("");
      } else if (!question_shown_ && !pressed_) {
        question_shown_ = true;
        signs_.clear();
        for_a_while(SURPRISED, 1500);
        anim(cb_look_x, look_x_, 0, 200);
        anim(cb_look_y, look_y_, 2, 200);
        lv_label_set_text(sign_text_, question_text_.c_str());
        anim(cb_sign, sign_y_, SIGN_SHOWN, 380, 0, 1, lv_anim_path_overshoot);
        anim(cb_arm, arm_pos_, 100, 400);
        sign_until_ = question_until_;  // sign stays until the answer
        for (int i = 0; i < 2; i++) lv_label_set_text(question_lbl_[i], question_opt_[i].c_str());
        lv_obj_move_foreground(question_box_);
        show(question_box_, true);
        if (led) led(0x4AA8FF, 0.4f);
        led_off_at_ = now_ms + 1500;
      }
    }
    if (letter_open_until_ && passed(now_ms, letter_open_until_)) {
      letter_open_until_ = 0;
      flap(false);
      letter_layout();
    }
    if (passed(now_ms, led_off_at_)) {
      led_off_at_ = 0;
      if (led) led(0, 0);
    }
    if (passed(now_ms, later_at_)) {
      later_at_ = 0;
      show_sign(later_text_, 3);
    }
    if (passed(now_ms, expression_until_)) {
      expression_until_ = 0;
      apply_expression(base_);
    }
    if (passed(now_ms, sign_until_)) {
      sign_until_ = 0;
      if (!signs_.empty()) {  // next sign of the sequence right after
        show_sign(signs_.front(), sign_duration_);
        signs_.pop_front();
      } else if (kind_ == OKTI && rnd(6) == 0) {  // clumsy: the sign slips down out of his arms
        anim(cb_sign, sign_y_, 250, 450, 0, 1, lv_anim_path_ease_in);
        anim(cb_arm, arm_pos_, 0, 300);
        for_a_while(SURPRISED, 1300);
        wiggle_until_ = now_ms + 800;
      } else {
        anim(cb_sign, sign_y_, SIGN_HIDDEN, 300, 0, 1, lv_anim_path_ease_in);
        anim(cb_arm, arm_pos_, 0, 400);
      }
    }
    // waiting message: every few seconds wave, hop, "!"; LED pulses
    if (!post_.empty() && !pressed_ && passed(now_ms, next_call_)) {
      next_call_ = now_ms + 7000;
      if (!sign_until_) {
        for_a_while(SURPRISED, 1500);
        wave(2500);
        anim(cb_hop, 0, 10, 150, 150, 2, lv_anim_path_ease_out);
        anim(cb_look_x, look_x_, 0, 200);  // at the viewer
        anim(cb_look_y, look_y_, 2, 200);
        next_look_ = now_ms + 4000;
        bool post_line = (calls_++ % 2) == 0;
        bool love = is_love(post_.front());
        std::string t = post_line ? (love ? std::string(kind_ == FOX ? txt::LOVE_LETTER_FOX : txt::LOVE_LETTER)
                                     : post_.size() > 1 ? esphome::str_sprintf(txt::POST_COUNT, (int) post_.size())
                                                        : std::string(txt::POST))
                                  : std::string(txt::TAP_ME);
        show_sign(t, 3);
      }
      if (led) led(0xFFB020, 0.5f);
      led_off_at_ = now_ms + 900;
    }
    if (pressed_ || petting_active_) return;
    boredom(now_ms);
    if (passed(now_ms, next_blink_)) {
      if (eyes_round() && eyes_h_ >= 10) anim(cb_blink, 0, 100, 70, 90, rnd(5) == 0 ? 2 : 1);
      next_blink_ = now_ms + (1800 + rnd(4500)) * (130 - tension_ * 8 / 10) / 100;  // nervous blinks more often
    }
    if (passed(now_ms, next_look_)) {
      if (expression_ != ASLEEP) {
        int tx = 0, ty = 0;
        if (sulking_) {  // looks away offended, only rarely peeks over
          tx = rnd(6) ? -12 : -4;
          ty = 3;
        } else if (rnd(100) >= 45) {
          tx = (int) rnd(25) - 12;
          ty = (int) rnd(13) - 6;
        }
        anim(cb_look_x, look_x_, tx, 260);
        anim(cb_look_y, look_y_, ty, 260);
      }
      next_look_ = now_ms + (1500 + rnd(5000)) * (130 - tension_ * 9 / 10) / 100;  // nervous looks around hectically
    }
    if (passed(now_ms, next_idea_)) {
      if (!expression_until_ && !sign_until_ && expression_ != ASLEEP) idea();
      next_idea_ = now_ms + 12000 + rnd(25000);
    }
  }

 protected:
  using Cb = void (*)(void *, int32_t);

  static lv_obj_t *shape(lv_obj_t *parent, int x, int y, int w, int h, uint32_t color,
                        int radius = LV_RADIUS_CIRCLE) {
    lv_obj_t *o = lv_obj_create(parent);
    lv_obj_remove_style_all(o);
    lv_obj_clear_flag(o, (lv_obj_flag_t) (LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE));
    lv_obj_set_pos(o, x, y);
    lv_obj_set_size(o, w, h);
    lv_obj_set_style_bg_color(o, lv_color_hex(color), 0);
    lv_obj_set_style_bg_opa(o, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(o, radius, 0);
    return o;
  }
  static void border(lv_obj_t *o, int width_px, uint32_t color) {
    lv_obj_set_style_border_width(o, width_px, 0);
    lv_obj_set_style_border_color(o, lv_color_hex(color), 0);
  }
  static lv_obj_t *arc(lv_obj_t *parent, int size_px, int width_px, uint32_t color) {
    lv_obj_t *a = lv_arc_create(parent);
    lv_obj_remove_style_all(a);
    lv_obj_clear_flag(a, (lv_obj_flag_t) (LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE));
    lv_obj_set_size(a, size_px, size_px);
    lv_obj_set_style_arc_width(a, width_px, LV_PART_MAIN);
    lv_obj_set_style_arc_color(a, lv_color_hex(color), LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(a, true, LV_PART_MAIN);
    lv_obj_set_style_arc_opa(a, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_arc_opa(a, LV_OPA_TRANSP, LV_PART_INDICATOR);
    return a;
  }
  // line (arm, eyebrow); the points belong to the caller and must stay alive
  static lv_obj_t *line(lv_obj_t *parent, int width_px, uint32_t color) {
    lv_obj_t *l = lv_line_create(parent);
    lv_obj_remove_style_all(l);
    lv_obj_clear_flag(l, (lv_obj_flag_t) (LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE));
    lv_obj_set_pos(l, 0, 0);
    lv_obj_set_style_line_width(l, width_px, 0);
    lv_obj_set_style_line_color(l, lv_color_hex(color), 0);
    lv_obj_set_style_line_rounded(l, true, 0);
    lv_obj_set_style_line_opa(l, LV_OPA_COVER, 0);
    return l;
  }
  // line for segments (robot arm, fox tail) on a large area, points + (ARM_DX, ARM_DY)
  static lv_obj_t *segment_line(lv_obj_t *parent, int width_px, uint32_t color) {
    lv_obj_t *l = line(parent, width_px, color);
    lv_obj_set_pos(l, -ARM_DX, -ARM_DY);
    lv_obj_set_size(l, 360, 300);
    return l;
  }
  // filled triangle from 5 thick lines from the tip to the base (no rotation, no custom drawing)
  static void triangle_lines(lv_obj_t *parent, int ax, int ay, int bx1, int by1, int bx2, int by2, uint32_t color,
                             int width_px, lv_point_precise_t (*pts)[2]) {
    for (int k = 0; k < 5; k++) {
      pts[k][0].x = ax + ARM_DX;
      pts[k][0].y = ay + ARM_DY;
      pts[k][1].x = bx1 + (bx2 - bx1) * k / 4 + ARM_DX;
      pts[k][1].y = by1 + (by2 - by1) * k / 4 + ARM_DY;
      lv_obj_t *l = segment_line(parent, width_px, color);
      lv_line_set_points(l, pts[k], 2);
    }
  }
  static void show(lv_obj_t *o, bool on) {
    if (o == nullptr) return;
    if (on)
      lv_obj_clear_flag(o, LV_OBJ_FLAG_HIDDEN);
    else
      lv_obj_add_flag(o, LV_OBJ_FLAG_HIDDEN);
  }
  static void transparent(lv_obj_t *o) {
    lv_obj_set_style_bg_opa(o, LV_OPA_TRANSP, 0);
    lv_obj_add_flag(o, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
  }

  // builds the character from plan_ (new), everything hangs on body_
  void build_figure();

  void anim(Cb cb, int32_t from, int32_t until, uint32_t ms, uint32_t back_ms = 0, uint32_t repeats = 1,
            lv_anim_path_cb_t path = lv_anim_path_ease_in_out) {
    lv_anim_delete(this, cb);
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, this);
    lv_anim_set_exec_cb(&a, cb);
    lv_anim_set_values(&a, from, until);
    lv_anim_set_duration(&a, ms);
    if (back_ms) lv_anim_set_playback_duration(&a, back_ms);
    lv_anim_set_repeat_count(&a, repeats);
    lv_anim_set_path_cb(&a, path);
    lv_anim_start(&a);
  }

  static void cb_border(lv_event_t *e) { lv_event_set_ext_draw_size(e, 110); }
  static void cb_nabu(void *v, int32_t x) {  // mini Nabu walks (slight bobbing while walking)
    auto *f = (Companion *) v;
    lv_obj_set_pos(f->nabu_mini_, x, 150 - (int) (3 * fabsf(sinf(x / 6.0f))));
  }
  static void cb_visit(void *v, int32_t x) {
    auto *f = (Companion *) v;
    f->visit_x_ = x;
    f->body_layout();
    for (int i = 0; i < 2; i++)  // fox paws patter while walking
      if (f->paw_[i]) lv_obj_set_y(f->paw_[i], 156 - (x == 0 ? 0 : (int) (5 * fabsf(sinf(x / 14.0f + i * 1.57f)))));
  }
  static void cb_blink(void *v, int32_t x) { auto *f = (Companion *) v; f->blink_ = x; f->eyes_layout(); }
  static void cb_look_x(void *v, int32_t x) { auto *f = (Companion *) v; f->look_x_ = x; f->face_layout(); }
  static void cb_look_y(void *v, int32_t x) { auto *f = (Companion *) v; f->look_y_ = x; f->face_layout(); }
  static void cb_hop(void *v, int32_t x) { auto *f = (Companion *) v; f->hop_ = x; f->body_layout(); }
  static void cb_breath(void *v, int32_t x) { auto *f = (Companion *) v; f->breath_ = x; f->body_layout(); }
  static void cb_arm(void *v, int32_t x) { auto *f = (Companion *) v; f->arm_pos_ = x; f->arms_layout(); }
  static void cb_sign_rotate(void *v, int32_t x) { lv_obj_set_style_transform_rotation(((Companion *) v)->sign_, x, 0); }
  static void cb_sign(void *v, int32_t x) { auto *f = (Companion *) v; f->sign_y_ = x; f->sign_layout(); }
  static void cb_wobble(void *v, int32_t x) {  // 0..1000 → three swings, ends at 0
    auto *f = (Companion *) v;
    f->wobble_ = (int) lroundf(6.0f * sinf(x / 1000.0f * 6.2832f * 3));
    f->body_layout();
  }
  static void cb_shake(void *v, int32_t x) {  // 0..1000 → twice left/right, ends in the middle
    auto *f = (Companion *) v;
    f->look_x_ = (int) lroundf(11.0f * sinf(x / 1000.0f * 6.2832f * 2));
    f->face_layout();
  }
  static void cb_spiral(void *v, int32_t x) {
    auto *f = (Companion *) v;
    for (int i = 0; i < 2; i++) lv_arc_set_rotation(f->spiral_[i], i ? 360 - x : x);
  }
  static void cb_zzz(void *v, int32_t x) {
    auto *f = (Companion *) v;
    lv_obj_set_y(f->zzz_, f->plan_->zzz_y - x);
  }

  static void cb_touch(lv_event_t *e) {
    auto *f = (Companion *) lv_event_get_user_data(e);
    if (!f->visible_) return;
    lv_event_code_t code = lv_event_get_code(e);
    if (code != LV_EVENT_PRESSED && code != LV_EVENT_PRESSING && code != LV_EVENT_RELEASED &&
        code != LV_EVENT_PRESS_LOST)
      return;
    lv_indev_t *indev = lv_indev_active();
    if (indev == nullptr) return;
    lv_point_t p;
    lv_indev_get_point(indev, &p);
    uint32_t now_ms = esphome::millis();
    if (code == LV_EVENT_PRESSED) {
      if (f->fist_until_ && abs((int) p.x - 160) < 50 && abs((int) p.y - FIST_Y) < 45) {  // fist bump returned
        f->collected_until_ = now_ms + 800;
        f->fist_hit(now_ms);
        return;
      }
      if (f->game_ != NO_GAME) {
        f->collected_until_ = now_ms + 800;
        f->game_tap(p, now_ms);
        return;
      }
      if (f->question_active_ && f->question_shown_ && p.y >= 180) {  // answer field tapped
        std::string a = f->question_opt_[p.x < 160 ? 0 : 1];
        f->collected_until_ = now_ms + 800;
        f->end_question(a);
        f->for_a_while(HAPPY, 1800);
        f->later_text_ = txt::THANKS;
        f->later_at_ = now_ms + 600;
      } else if (f->timer_ringing_) {  // ringing off (also on the voice assistant)
        f->timer_ringing_ = false;
        f->collected_until_ = now_ms + 800;
        f->for_a_while(TIRED, 1500);
        f->show_sign(txt::OKAY_OKAY, 2);
        if (f->timer_off) f->timer_off();
      } else if (!f->post_.empty()) {  // collect the message instead of poking
        f->collected_until_ = now_ms + 800;
        f->play_post();
      }
      f->pressed_ = true;
      f->start_ = p;
      f->last_touch_ = now_ms;
      if (!f->sulking_) f->bored_level_ = 0;
      if (!f->sulking_ && (f->base_ == TIRED || f->base_ == ASLEEP)) {
        f->base_ = NORMAL;
        if (!f->expression_until_) f->apply_expression(NORMAL);
      }
      f->look_at_finger(p);
      f->grab(p);
    } else if (code == LV_EVENT_PRESSING) {
      if (!f->petting_active_ && abs((int) (p.x - f->start_.x)) + abs((int) (p.y - f->start_.y)) > 35) {
        f->petting_active_ = true;
        f->counts_[PETS]++;
        f->grab_arm_ = -1;
        if (f->petted)
          f->petted(now_ms - f->last_sign_time_ < 15000 ? f->last_sign_ : std::string());
        f->expression_until_ = 0;
        f->apply_expression(HAPPY);
        lv_obj_set_style_bg_opa(f->cheek_[0], LV_OPA_COVER, 0);
        lv_obj_set_style_bg_opa(f->cheek_[1], LV_OPA_COVER, 0);
        if (f->led) f->led(0xFF8A2A, 0.35f);
        f->led_off_at_ = 0;
      }
      if (f->petting_active_) {
        f->wiggle_until_ = now_ms + 400;
      } else {
        f->look_at_finger(p);
        f->grab(p);
      }
    } else {
      f->pressed_ = false;
      f->grab_arm_ = -1;
      if (f->petting_active_) {
        f->petting_active_ = false;
        f->petting_until_ = now_ms + 700;
        if (f->sulking_ && ++f->reconciled_ < 2) {  // first petting: not quite yet
          f->for_a_while(ANNOYED, 1500);
          f->later_text_ = txt::NOT_QUITE;
          f->later_at_ = now_ms + 400;
          return;
        }
        if (f->sulking_) {  // second petting: made up
          f->sulking_ = false;
          f->base_ = NORMAL;
          f->bored_level_ = 0;
          f->blush_until_ = now_ms + 4000;
          f->later_text_ = txt::FINE;
          f->later_at_ = now_ms + 400;
        }
        f->for_a_while(HAPPY, 2500);
        f->wiggle_until_ = now_ms + 1200;
        f->led_off_at_ = now_ms + 2500;
      }
    }
  }

  void look_at_finger(lv_point_t p) {
    int tx = std::max(-12, std::min(12, (int) (p.x - 160) / 10));
    int ty = std::max(-8, std::min(8, (int) (p.y - 130) / 12));
    anim(cb_look_x, look_x_, tx, 120);
    anim(cb_look_y, look_y_, ty, 120);
    next_look_ = esphome::millis() + 3000;
  }
  // Okti: the arm whose tip is closest to the finger reaches for it
  void grab(lv_point_t p) {
    grab_x_ = (int) p.x - plan_->kx - wobble_;
    grab_y_ = (int) p.y - (plan_->ky - hop_ - breath_);
    if (kind_ != OKTI) {  // robot/fox: the closer arm or paw
      grab_arm_ = grab_x_ < 90 ? 0 : 1;
      return;
    }
    float best = 1e9f;
    for (int i = 0; i < ARMS; i++) {
      float w = ARM_ANGLE[i] * 0.017453f;
      float sx = ARM_X[i] + ARM_LENGTH[i] * cosf(w), sy = ARM_Y + ARM_LENGTH[i] * sinf(w);
      float d = (sx - grab_x_) * (sx - grab_x_) + (sy - grab_y_) * (sy - grab_y_);
      if (d < best) {
        best = d;
        grab_arm_ = i;
      }
    }
  }
  void wave(uint32_t ms) { wave_until_ = esphome::millis() + ms; }

  // spread text over signs (at most ~24 characters each, at word boundaries) and show them one after another
  void plan_signs(const std::string &text, int seconds);

  void for_a_while(Expression a, uint32_t ms) {
    apply_expression(a);
    expression_until_ = esphome::millis() + ms;
  }
  void led_flash(uint32_t color, uint32_t ms) {
    if (!led) return;
    led(color, 0.6f);
    led_off_at_ = esphome::millis() + ms;
  }

  void idea() {
    uint32_t r = rnd(100);
    uint32_t now_ms = esphome::millis();
    if (energy_ < 30 && rnd(3) == 0) {  // tired: yawns often, eyes half closed
      for_a_while(rnd(2) ? YAWN : TIRED, 2500);
      return;
    }
    if (tension_ > 70 && rnd(3) == 0) {  // nervous: flinches, looks around hectically
      for_a_while(SURPRISED, 700);
      anim(cb_hop, 0, 6, 90, 120, 1, lv_anim_path_ease_out);
      anim(cb_look_x, look_x_, rnd(2) ? 12 : -12, 120);
      next_look_ = now_ms + 600;
      if (rnd(4) == 0) show_sign(kind_ == ROBOT ? txt::PULSE_HIGH : kind_ == FOX ? txt::ALL_QUIET : txt::WHAT_WAS_THAT, 3);
      return;
    }
    if (r < 4) {
      fist_bump();
    } else if (r >= 6 && r < 9 && kind_ == FOX) {  // fox shows courage (4–5 stay the special move): straightens up, looks ahead determined
      expression_until_ = 0;
      for_a_while(NORMAL, 3500);
      anim(cb_hop, 0, 8, 250, 600, 1, lv_anim_path_ease_out);
      anim(cb_look_x, look_x_, 0, 200);
      anim(cb_look_y, look_y_, -3, 200);
      next_look_ = now_ms + 4000;
      show_later(txt::SAYINGS_FOX[rnd(sizeof(txt::SAYINGS_FOX) / sizeof(txt::SAYINGS_FOX[0]))], 500);
    } else if (r < 8 && kind_ != OKTI) {  // special move (Okti's ink stays a fright reaction)
      special();
    } else if (r < 6) {  // Okti: ties himself in a knot now and then
      knot();
    } else if (r < 8) {  // clumsy Okti: trips over his own arms
      for_a_while(DIZZY, 1400);
      anim(cb_wobble, 0, 1000, 900);
      anim(cb_hop, 0, 6, 100, 140, 1, lv_anim_path_ease_out);
      wiggle_until_ = now_ms + 900;
      later_text_ = rnd(2) ? txt::OOPS : txt::WHO_PUT_ARM;
      later_at_ = now_ms + 1500;
    } else if (r < 22) {
      anim(cb_hop, 0, 10, 150, 150, 2, lv_anim_path_ease_out);
    } else if (r < 36) {
      for_a_while(YAWN, 2200);
    } else if (r < 48) {
      anim(cb_wobble, 0, 1000, 1500);
    } else if (r < 60 && (kind_ == FOX || rnd(2))) {  // a walk: out to the right, back from the left (fox more often)
      visit_leave_ = now_ms + 100;
      visit_return_ = now_ms + 7000;
    } else if (r < 60) {
      wave(2500);
    } else if (r < 72) {
      anim(cb_look_x, look_x_, rnd(2) ? 12 : -12, 300);
      next_look_ = now_ms + 1800;
    } else if (r < 82) {
      const char *s = saying();
      std::string z = counts_saying();
      if (!z.empty() && rnd(kind_ == ROBOT ? 2 : 3) == 0)
        show_sign(z, 4);
      else if (rnd(kind_ == OKTI ? 3 : 5) == 0)
        sign_blunder(s);
      else
        show_sign(s, 4);
    } else {
      anim(cb_blink, 0, 100, 70, 90, 2);
    }
  }

  bool eyes_round() const { return expression_ != HAPPY && expression_ != DIZZY; }

  // Okti color, stepless: 0 = yellow (happy), 100 = red (grumpy); the underside the other way round
  void color(int st);

  void apply_expression(Expression a);

  void end_game(const std::string &name, int value);
  int ttt_winner() const {  // 1 = you (X), 2 = Yellow (O), 3 = full, 0 = running
    static const int L[8][3] = {{0,1,2},{3,4,5},{6,7,8},{0,3,6},{1,4,7},{2,5,8},{0,4,8},{2,4,6}};
    for (auto &l : L)
      if (ttt_[l[0]] && ttt_[l[0]] == ttt_[l[1]] && ttt_[l[1]] == ttt_[l[2]]) return ttt_[l[0]];
    for (int f : ttt_) if (!f) return 0;
    return 3;
  }
  void ttt_set(int i, int who);
  int ttt_move() {  // win, block (not always), center, otherwise random
    for (int who : {2, 1})
      for (int i = 0; i < 9; i++) {
        if (ttt_[i]) continue;
        ttt_[i] = who;
        bool good = ttt_winner() == who;
        ttt_[i] = 0;
        if (good && (who == 2 || rnd(100) < 85)) return i;
      }
    if (!ttt_[4]) return 4;
    int free_cells[9], n = 0;
    for (int i = 0; i < 9; i++) if (!ttt_[i]) free_cells[n++] = i;
    return n ? free_cells[rnd(n)] : -1;
  }
  void ttt_evaluate(uint32_t now_ms);
  void simon_light(int i, bool on);
  void simon_show_from(uint32_t t);
  void simon_end(uint32_t now_ms);
  void game_tick(uint32_t now_ms);
  void game_tap(lv_point_t p, uint32_t now_ms);

  void end_question(const std::string &a);

  // effects: own times plus automatic – petting → blush, timer last 10 s / game running → sweat,
  // sad → tear
  void effect_tick(uint32_t now_ms) {
    if (sweat_ == nullptr) return;
    auto on = [now_ms](uint32_t until) { return until && !passed(now_ms, until); };
    bool timer_short = timer_end_ && !timer_ringing_ && (int32_t) (timer_end_ - now_ms) < 10000;
    bool sweat = on(sweat_until_) || timer_short || game_ != NO_GAME || tension_ >= 85;
    bool tear = on(tear_until_) || expression_ == SAD;
    bool blush = on(blush_until_) || petting();
    bool sparkle = on(sparkle_until_);
    const Blueprint &p = *plan_;
    show(sweat_, sweat);
    if (sweat) {  // runs slowly down the temple, then from the start
      int y = (int) (now_ms / 60 % 18);
      lv_obj_set_pos(sweat_, p.eye_x[1] + 26, p.eye_y - 38 + y);
    }
    show(tear_, tear);
    if (tear) {  // rolls from the eye to the cheek
      int y = (int) (now_ms / 45 % 30);
      lv_obj_set_pos(tear_, p.eye_x[0] - 4, p.eye_y + 12 + y);
    }
    for (auto &w : blush_)
      for (auto *s : w) show(s, blush);
    if (blush != blush_on_) {  // cheeks strongly pink; off: apply_expression restores the normal opacity
      blush_on_ = blush;
      for (int i = 0; i < 2; i++) {
        lv_obj_set_style_bg_color(cheek_[i], lv_color_hex(blush ? 0xFF5A7A : CHEEK), 0);
        int g = blush ? 8 : 0;  // bigger when blushing
        lv_obj_set_size(cheek_[i], plan_->cheek_w + g, plan_->cheek_h + g / 2);
        lv_obj_set_pos(cheek_[i], plan_->cheek_x[i] - g / 2, plan_->cheek_y - g / 4);
      }
      if (blush)
        for (auto *w : cheek_) lv_obj_set_style_bg_opa(w, LV_OPA_COVER, 0);
      else
        apply_expression(expression_);
    }
    for (int i = 0; i < 4; i++) {  // little stars blink offset
      show(sparkle_[i], sparkle && ((now_ms / 180 + i) % 3) != 0);
    }
  }

  void timer_tick(uint32_t now_ms) {
    if (timer_end_ && !timer_ringing_) {
      int rest = (int) (((int32_t) (timer_end_ - now_ms) + 999) / 1000);
      if (rest <= 0) {
        timer(-1, timer_name_);  // own countdown done; HA reports the ringing as well
      } else if (rest <= 5 && rest != timer_count_) {  // last 5 s: big counting
        timer_count_ = rest;
        for_a_while(SURPRISED, 1100);
        anim(cb_hop, 0, 8, 120, 120, 1, lv_anim_path_ease_out);
        show_sign(esphome::str_sprintf("%d", rest), 1);
        if (digit_font_) lv_obj_set_style_text_font(sign_text_, digit_font_, 0);  // big countdown digit
      } else if (rest > 60 && passed(now_ms, next_timer_info_) && !sign_until_ && !pressed_) {
        next_timer_info_ = now_ms + 90000;
        anim(cb_look_x, look_x_, -12, 250);
        next_look_ = now_ms + 2500;
        show_sign(kind_ == ROBOT ? esphome::str_sprintf(txt::TIMER_LEFT_ROBOT, rest) : esphome::str_sprintf(txt::TIMER_LEFT_FMT, duration_text(rest).c_str()), 3);
      }
    }
    if (timer_ringing_) {
      if (passed(now_ms, ring_until_)) {
        timer_ringing_ = false;
        return;
      }
      if (passed(now_ms, next_ring_)) {
        next_ring_ = now_ms + 1300;
        for_a_while(SURPRISED, 1400);
        anim(cb_hop, 0, 18, 160, 200, 1, lv_anim_path_ease_out);
        wiggle_until_ = now_ms + 1300;
        if (led) led((ring_beat_++ % 2) ? 0xFF4A1A : 0xFFB020, 0.8f);
        led_off_at_ = now_ms + 600;
        if (!sign_until_) show_sign(timer_name_.empty() ? std::string(txt::TIMER) : timer_name_ + "!", 2);
      }
    }
  }

  // Okti arms: advance the phase and set the points again (every 100 ms, every 50 ms while wiggling/grabbing)
  void tentacle_tick(uint32_t now_ms);

  // robot arms: shoulder → elbow → hand; hang, hold the sign, wave (right), reach for the finger
  void robot_arms();

  // fox tail: bushy, curved up, wags (more when he is happy or wiggles); white tip
  void fox_tail();

  void draw_tentacles();

  void eyes_layout();
  void face_layout() { lv_obj_set_pos(face_, look_x_, look_y_); }
  void body_layout();
  void arms_layout();
  void sign_layout() { lv_obj_set_y(sign_, sign_y_); }
  void hat();
  void apply_decor();

  // background decoration (screen coordinates), lies behind the character
  void build_decor();

  // --- weather view (behind the character): sun top right below the clock, two clouds, rain/snow, fog, lightning ---
  void build_weather();

  void apply_weather();

  void weather_tick(uint32_t now_ms);

  void set_bats(int beat) {  // wings: V (up) or flat
    for (int i = 0; i < 3; i++) {
      int x = bat_pos_[i][0], y = bat_pos_[i][1], h = ((beat + i) % 2) ? -6 : 3;
      wing_pts_[i][0].x = x - 12;
      wing_pts_[i][0].y = y + h;
      wing_pts_[i][1].x = x - 5;
      wing_pts_[i][1].y = y - 1;
      wing_pts_[i][2].x = x;
      wing_pts_[i][2].y = y + 2;
      wing_pts_[i][3].x = x + 5;
      wing_pts_[i][3].y = y - 1;
      wing_pts_[i][4].x = x + 12;
      wing_pts_[i][4].y = y + h;
      lv_line_set_points(wing_[i], wing_pts_[i], 5);
    }
  }

  // move the decoration: bats flap, lights sparkle, snow falls
  void decor_tick(uint32_t now_ms);

  Kind kind_{OKTI};
  const Blueprint *plan_{&PLAN_OKTI};
  lv_obj_t *root_{}, *clock_{}, *shadow_{}, *body_{}, *face_{}, *pumpkin_{}, *stem_{}, *antenna_{}, *head_{};
  lv_obj_t *arm_[2]{}, *cheek_[2]{}, *eye_[2]{}, *shine_[2]{}, *happy_[2]{}, *spiral_[2]{}, *brow_[2]{};
  lv_obj_t *mouth_arc_{}, *mouth_o_{}, *mouth_line_{}, *zzz_{}, *sign_{}, *sign_text_{};
  lv_obj_t *tentacle_[ARMS]{}, *underside_[ARMS]{};
  lv_point_precise_t arm_pkt_[ARMS][SEGMENTS + 1]{}, under_pts_[ARMS][SEGMENTS + 1]{}, brow_pts_[2][2]{};

  bool visible_{false}, pressed_{false}, petting_active_{false}, halloween_{false};
  int mood_{0}, color_st_{-1}, tension_{20}, energy_{70};
  bool christmas_{false};
  int month_{0}, day_{0}, decor_mode_{0}, decor_beat_{0};
  uint32_t last_decor_{0}, last_snow_{0}, last_leaves_{0};
  static const int PARTICLES = 18, SUN_X = 286, SUN_Y = 58;
  lv_obj_t *weather_{}, *sun_{}, *ray_[8]{}, *fog_[3]{}, *cloud_[2]{}, *cloud_part_[2][3]{}, *drop_[PARTICLES]{},
      *flash_{};
  lv_point_precise_t ray_pts_[8][2]{}, flash_pts_[5]{};
  float cloud_x_[2]{}, drop_x_[PARTICLES]{}, drop_y_[PARTICLES]{};
  uint8_t drop_kind_[PARTICLES]{};
  bool weather_wind_{false}, weather_flash_{false};
  uint32_t last_weather_{0}, flash_on_{0}, next_flash_{0};
  std::string weather_ha_, weather_test_, weather_old_;
  int weather_mood_{0};
  lv_obj_t *sweat_{}, *tear_{}, *blush_[2][3]{}, *sparkle_[4]{};
  uint32_t sweat_until_{0}, tear_until_{0}, blush_until_{0}, sparkle_until_{0};
  bool blush_on_{false};
  uint32_t blunder_at_{0};
  int bored_level_{0}, reconciled_{0};
  bool sulking_{false};
  lv_obj_t *ink_[6]{}, *splash_[8]{};
  uint32_t ink_start_{0};
  bool ink_gone_{false};
  static const int FIST_Y = 196;
  lv_obj_t *fist_{}, *fist_arm_{}, *knuckle_[3]{}, *sparks_[8]{};
  uint32_t fist_start_{0}, fist_until_{0}, sparks_start_{0};
  int counts_[4]{};
  bool away_{false};
  lv_obj_t *mini_{}, *mini_body_{}, *mini_z_{}, *mini_dot_{};
  lv_point_precise_t mini_pts_[2][3]{};  // fox ears of the night mini (LVGL keeps pointers to them)
  bool mini_on_{false};
  uint32_t mini_next_{0};
  bool sleeping_{false};
  lv_obj_t *network_symbol_{}, *network_text_{};
  int network_level_{0};
  std::vector<std::string> remote_;

 public:
  lv_obj_t *help_button_{}, *help_panel_{}, *jewelry_{}, *jewelry_shine_{}, *earrings_[2]{};
  bool jewelry_on_{false};
  uint32_t help_until_{0}, games_until_{0};
  lv_obj_t *games_button_{}, *games_panel_{};
  uint32_t scare_at_{0}, hearts_start_{0}, juggle_start_{0}, chase_start_{0}, prop_start_{0};
  bool juggle_drop_{false};
  lv_obj_t *ball_[3]{}, *prop_[2]{};
  lv_point_precise_t prop_pts_[2][2]{};
  lv_obj_t *heart_g_[6]{}, *letter_seal_{};
  uint32_t knot_until_{0}, knot_loose_{0}, tail_fan_until_{0}, flap_start_{0};
  lv_obj_t *gear_[2]{};
  lv_obj_t *bed_{}, *standby_{};
  lv_obj_t *matrix_[14]{};
  float matrix_y_[14]{};
  uint32_t matrix_until_{0}, last_matrix_{0};
  lv_obj_t *tail_edge_{}, *tail_belly_{}, *sucker_[ARMS][3]{};
  lv_obj_t *mound_{}, *visor_{}, *scan_[3]{};
  uint32_t mouse_start_{0}, glitch_start_{0};
  int mouse_y_{0};
  bool mouse_out_{false};
  std::string card_;
  lv_obj_t *suitcase_{}, *note_text_{};
  lv_obj_t *status_{}, *status_sym_[4]{}, *status_count_[4]{};
  const lv_font_t *status_font_{}, *digit_font_{}, *sign_font_{};
  int status_stand_{-1};
  int blunder_kind_{0};
  lv_obj_t *decor_hat_{}, *decor_w_{}, *cap_{}, *wing_[3]{}, *bauble_[6]{}, *bulb_[12]{}, *flake_[14]{};
  lv_point_precise_t wing_pts_[3][5]{}, chain_pts_[9]{};
  int bat_pos_[3][2]{};
  float flake_x_[14]{}, flake_y_[14]{};
  Expression expression_{NORMAL}, base_{NORMAL};
  int eyes_w_{24}, eyes_h_{40};
  int blink_{0}, look_x_{0}, look_y_{0}, hop_{0}, breath_{0}, wobble_{0}, arm_pos_{0}, sign_y_{SIGN_HIDDEN};
  int grab_arm_{-1}, grab_x_{0}, grab_y_{0};
  float phase_{0};
  uint32_t expression_until_{0}, sign_until_{0}, led_off_at_{0}, later_at_{0}, petting_until_{0};
  uint32_t next_blink_{0}, next_look_{0}, next_idea_{0}, last_touch_{0};
  int nabu_look_x_{-12}, nabu_look_y_{-4};
  uint32_t poke_start_{0}, nabu_quiet_{0}, wave_until_{0}, wiggle_until_{0}, last_tentacle_{0};
  int pokes_{0};
  lv_point_t start_{};
  std::string later_text_;
  std::deque<std::string> post_, signs_;
  int sign_duration_{4};
  int calls_{0};
  lv_obj_t *letter_{};
  lv_point_precise_t letter_flap_[3]{};
  lv_obj_t *letter_flap_obj_{};
  bool arm0_front_{false};
  uint32_t letter_open_until_{0};
  uint32_t next_call_{0}, collected_until_{0};
  uint32_t timer_end_{0}, next_timer_info_{0}, ring_until_{0}, next_ring_{0};
  bool timer_ringing_{false};
  int timer_count_{-1}, ring_beat_{0};
  std::string timer_name_;
  std::string question_text_, question_opt_[2];
  bool question_active_{false}, question_shown_{false};
  uint32_t question_until_{0};
  lv_obj_t *question_box_{}, *question_lbl_[2]{};
  Game game_{NO_GAME};
  uint32_t reaction_ready_{0}, reaction_go_{0}, ttt_yellow_at_{0}, ttt_end_at_{0};
  int ttt_[9]{}, ttt_result_{0};
  uint32_t last_move_{0};
  lv_obj_t *game_box_{}, *ttt_lbl_[9]{};
  lv_obj_t *simon_box_{}, *simon_field_[4]{};
  lv_obj_t *glasses_{}, *scarf_{}, *umbrella_{};
  lv_point_precise_t umbrella_stick_[2]{}, can_spout_[2]{};
  lv_obj_t *basket_{}, *bin_{}, *pillow_{}, *nabu_mini_{};
  lv_obj_t *robot_arm_[2]{}, *robot_hand_[2]{}, *heart_{}, *tail_{}, *tail_tip_{}, *paw_[2]{}, *tip_[5]{};
  lv_point_precise_t tip_pts_[5][2]{};
  lv_point_precise_t robot_pts_[2][3]{}, tail_pts_[5]{}, ear_pts_[2][2][5][2]{};
  // ear tips [ear][outer/inner][x,y]
  static constexpr int EAR_TIP[2][2][2] = {{{34, -18}, {36, -4}}, {{146, -18}, {144, -4}}};
  uint32_t nabu_greet_at_{0}, nabu_leaves_at_{0}, nabu_gone_at_{0}, wilt_until_{0};
  int visit_x_{0};
  uint32_t visit_leave_{0}, visit_return_{0};
  lv_obj_t *fan_{}, *can_{}, *joint_{}, *smoke_[3]{}, *hat_glyph_{}, *decor_g_[8]{};
  const lv_font_t *decor_font_{};
  const DecoSet *decor_set_{};
  int occasion_count_{0};
  bool pink_{false};
  std::string decor_key_;
  std::string occasion_, decor_test_, last_sign_;
  uint32_t last_sign_time_{0};
  const char *accessory_{""};
  uint32_t accessory_color_{0};
  int prop_id_{0};
  uint32_t prop_until_{0};
  int simon_seq_[32]{}, simon_len_{1}, simon_idx_{0}, simon_pos_{0}, simon_on_{-1}, simon_phase_{0};
  uint32_t simon_next_{0}, simon_off_at_{0};
};

inline Companion companion;

}  // namespace yellow

#include "yellow_games.h"
#include "yellow_special.h"
#include "yellow_decor.h"
#include "yellow_body.h"
#include "yellow_post.h"
