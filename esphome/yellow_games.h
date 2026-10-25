#pragma once
// Yellow: games (reaction, tic-tac-toe, Simon) and questions with answer fields (methods of Companion, see yellow.h)
#include "yellow.h"

namespace yellow {

inline bool Companion::game(const std::string &name) {
  if (!visible_ || game_ != NO_GAME) return false;
  uint32_t now_ms = esphome::millis();
  if (name == "reaction") {
    game_ = REACTION;
    reaction_go_ = 0;
    reaction_ready_ = now_ms + 3500 + rnd(3500);
    for_a_while(NORMAL, 100);
    show_sign(txt::REACTION_RULES, 3);
    return true;
  }
  if (name == "simon") {
    game_ = SIMON;
    simon_len_ = 1;
    for (auto &f : simon_seq_) f = (int) rnd(4);
    simon_show_from(now_ms + 1200);
    for (int i = 0; i < 4; i++) simon_light(i, false);
    lv_obj_move_foreground(simon_box_);
    show(simon_box_, true);
    show_sign(txt::COPY_ME, 2);
    return true;
  }
  if (name == "ttt") {
    game_ = TTT;
    for (auto &f : ttt_) f = 0;
    ttt_yellow_at_ = 0;
    ttt_end_at_ = 0;
    last_move_ = now_ms;
    for (int i = 0; i < 9; i++) lv_label_set_text(ttt_lbl_[i], "");
    lv_obj_move_foreground(game_box_);
    show(game_box_, true);
    show_sign(txt::YOU_START, 2);
    return true;
  }
  return false;
}

inline void Companion::question(const std::string &text, const std::string &options) {
  question_text_ = text;
  size_t t = options.find('|');
  question_opt_[0] = t == std::string::npos ? options : options.substr(0, t);
  question_opt_[1] = t == std::string::npos ? std::string(txt::NO) : options.substr(t + 1);
  if (question_opt_[0].empty()) question_opt_[0] = txt::YES;
  question_active_ = true;
  question_until_ = esphome::millis() + 600000;
  question_shown_ = false;
}

inline void Companion::cancel_question() {
  if (!question_active_) return;
  question_active_ = false;
  question_shown_ = false;
  show(question_box_, false);
  sign_until_ = esphome::millis() + 10;
}

inline void Companion::end_game(const std::string &name, int value) {
  game_ = NO_GAME;
  counts_[GAMES]++;
  if (game_over) game_over(name, value);
}

inline void Companion::ttt_set(int i, int who) {
  ttt_[i] = who;
  lv_label_set_text(ttt_lbl_[i], who == 1 ? "X" : who == 2 ? "O" : "");
  lv_obj_set_style_text_color(ttt_lbl_[i], lv_color_hex(who == 1 ? 0x2E7DD8 : RED), 0);
}

inline void Companion::ttt_evaluate(uint32_t now_ms) {
  int g = ttt_winner();
  if (!g) return;
  if (g == 1 && rnd(100) < 40) {  // lost? cheat sometimes: an X becomes an O
    for (int i = 0; i < 9; i++)
      if (ttt_[i] == 1) {
        ttt_set(i, 2);
        show_sign(txt::DIDNT_SEE, 3);
        for_a_while(HAPPY, 2500);
        g = ttt_winner();
        break;
      }
    if (!g) return;  // on we go
  }
  ttt_end_at_ = now_ms + 3500;
  ttt_result_ = g == 1 ? 1 : g == 2 ? -1 : 0;
  if (g == 1) {
    for_a_while(ANNOYED, 3500);
    show_sign(txt::FINE_YOU, 3);
  } else if (g == 2) {
    for_a_while(HAPPY, 3500);
    anim(cb_hop, 0, 14, 160, 180, 2, lv_anim_path_ease_out);
    show_sign(txt::WON, 3);
  } else {
    show_sign(txt::DRAW, 3);
  }
}

inline void Companion::simon_light(int i, bool on) {
  lv_obj_set_style_bg_color(simon_field_[i], lv_color_hex(on ? SIMON_BRIGHT[i] : SIMON_DARK[i]), 0);
  if (on && led) led(SIMON_BRIGHT[i], 0.6f);
  if (!on && led) led(0, 0);
}

inline void Companion::simon_show_from(uint32_t t) {
  simon_phase_ = 1;  // show
  simon_idx_ = 0;
  simon_on_ = -1;
  simon_next_ = t;
}

inline void Companion::simon_end(uint32_t now_ms) {
  int rounds = simon_len_ - 1;
  for_a_while(rounds >= 5 ? HAPPY : ANNOYED, 3000);
  show_sign(esphome::str_sprintf(txt::ROUNDS_FMT, rounds, rounds == 1 ? "" : txt::ROUNDS_PLURAL), 3);
  simon_phase_ = 3;  // finish
  simon_next_ = now_ms + 2500;
}

inline void Companion::game_tick(uint32_t now_ms) {
  if (game_ == SIMON) {
    if (simon_off_at_ && passed(now_ms, simon_off_at_)) {  // short flash after a tap
      simon_off_at_ = 0;
      for (int i = 0; i < 4; i++) simon_light(i, false);
    }
    if (simon_phase_ == 1 && passed(now_ms, simon_next_)) {
      if (simon_on_ >= 0) {
        simon_light(simon_on_, false);
        simon_on_ = -1;
        simon_idx_++;
        simon_next_ = now_ms + 180;
      } else if (simon_idx_ < simon_len_) {
        simon_on_ = simon_seq_[simon_idx_];
        simon_light(simon_on_, true);
        simon_next_ = now_ms + std::max(220, 480 - simon_len_ * 15);
      } else {
        simon_phase_ = 2;  // enter
        simon_pos_ = 0;
        simon_next_ = now_ms + 8000;
      }
    } else if (simon_phase_ == 2 && passed(now_ms, simon_next_)) {
      simon_end(now_ms);
    } else if (simon_phase_ == 3 && passed(now_ms, simon_next_)) {
      show(simon_box_, false);
      end_game("simon", simon_len_ - 1);
    }
    return;
  }
  if (game_ == REACTION) {
    if (!reaction_go_ && passed(now_ms, reaction_ready_)) {
      reaction_go_ = now_ms;
      for_a_while(SURPRISED, 2000);
      anim(cb_hop, 0, 30, 140, 200, 1, lv_anim_path_ease_out);
      wiggle_until_ = now_ms + 800;
      show_sign(txt::NOW, 2);
      if (led) led(0x5AE07A, 0.8f);
      led_off_at_ = now_ms + 800;
    } else if (reaction_go_ && now_ms - reaction_go_ > 5000) {
      show_sign(txt::FELL_ASLEEP, 3);
      end_game("reaction", -2);
    }
  } else if (game_ == TTT) {
    if (!ttt_end_at_ && now_ms - last_move_ > 120000) {  // no move for 2 min: give up
      show(game_box_, false);
      show_sign(txt::HAD_ENOUGH, 3);
      end_game("ttt", -2);
      return;
    }
    if (ttt_yellow_at_ && passed(now_ms, ttt_yellow_at_)) {
      ttt_yellow_at_ = 0;
      int z = ttt_move();
      if (z >= 0) ttt_set(z, 2);
      ttt_evaluate(now_ms);
    }
    if (ttt_end_at_ && passed(now_ms, ttt_end_at_)) {
      ttt_end_at_ = 0;
      show(game_box_, false);
      end_game("ttt", ttt_result_);
    }
  }
}

inline void Companion::game_tap(lv_point_t p, uint32_t now_ms) {
  if (game_ == SIMON) {
    if (simon_phase_ != 2) return;
    int i = (p.x >= 160 ? 1 : 0) + (p.y >= 120 ? 2 : 0);
    for (int k = 0; k < 4; k++) simon_light(k, k == i);
    simon_off_at_ = now_ms + 220;
    if (i != simon_seq_[simon_pos_]) {
      simon_end(now_ms);
      return;
    }
    simon_next_ = now_ms + 8000;
    if (++simon_pos_ == simon_len_) {
      if (simon_len_ < 32) simon_len_++;
      simon_show_from(now_ms + 900);
    }
    return;
  }
  if (game_ == REACTION) {
    if (!reaction_go_) {
      for_a_while(ANNOYED, 2500);
      show_sign(txt::TOO_EARLY, 3);
      end_game("reaction", -2);
    } else {
      int ms = (int) (now_ms - reaction_go_);
      for_a_while(ms < 300 ? HAPPY : NORMAL, 3000);
      show_sign(esphome::str_sprintf("%d ms", ms), 4);
      end_game("reaction", ms);
    }
  } else if (game_ == TTT) {
    if (ttt_yellow_at_ || ttt_end_at_) return;
    if (p.x < 70 || p.y < 30) return;
    int c = ((int) p.x - 70) / 62, r = ((int) p.y - 30) / 62;
    if (c > 2 || r > 2) return;
    int i = r * 3 + c;
    if (ttt_[i]) return;
    ttt_set(i, 1);
    last_move_ = now_ms;
    ttt_evaluate(now_ms);
    if (!ttt_end_at_ && !ttt_winner()) ttt_yellow_at_ = now_ms + 700;
  }
}

inline void Companion::end_question(const std::string &a) {
  question_active_ = false;
  question_shown_ = false;
  show(question_box_, false);
  sign_until_ = esphome::millis() + 10;  // sign gone (via tick)
  if (answer) answer(a);
}

}  // namespace yellow
