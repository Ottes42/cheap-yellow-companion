#pragma once
// Yellow: constants, character blueprints and decoration sets (included by yellow.h); texts in lang/*.h
#include <algorithm>
#include <cmath>
#include <deque>
#include <functional>
#include <string>
#include <vector>
#include <esp_heap_caps.h>
#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"

namespace yellow {

enum Kind { OKTI, ROBOT, FOX };
enum Game { NO_GAME, REACTION, TTT, SIMON };
static const uint32_t SIMON_BRIGHT[4] = {0xFF4A4A, 0x5AE07A, 0x4AA8FF, 0xFFD23F};
static const uint32_t SIMON_DARK[4] = {0x2E0C0C, 0x0C2614, 0x0C1A30, 0x2E2608};
enum Expression { NORMAL, HAPPY, SURPRISED, DIZZY, TIRED, ANNOYED, SAD, ASLEEP, YAWN };

static const uint32_t YELLOW = 0xFFD23F, YELLOW_EDGE = 0xD9A300, METAL = 0x8A93A6, VISOR = 0x1E2433,
                      GLOW = 0x6FE7FF, RED = 0xE8413A;
static const uint32_t EYE = 0x2B2118, CHEEK = 0xFF8FA3, BACKGROUND = 0x161A26, SHADOW = 0x0B0D13;
static const int SIGN_HIDDEN = -100, SIGN_SHOWN = 6;
static const int ARMS = 8, SEGMENTS = 5;  // Okti: 8 arms of 5 segments each
// arm lines lie on a screen-sized area, offset by (ARM_DX, ARM_DY) against the container,
// so no point gets negative (LVGL clips lines to their own object area)
static const int ARM_DX = 90, ARM_DY = 60;
static const int LETTER_X = 188, LETTER_Y = 96;
static constexpr const char *LOVE_PREFIX = "LOVE|";  // post with this prefix is a love letter (internal marker); BRIEF: center of the envelope (container coordinates)

// sizes per character. Container (body_) at kx/ky; face relative to its carrier (head or visor).
struct Blueprint {
  int kx, ky;
  int eye_x[2], eye_y;
  float eye_scale;
  int arc;  // size of the arc eyes
  int mouth_arc;
  int mouth_x, mouth_y;
  int cheek_x[2], cheek_y, cheek_w, cheek_h;
  int arm_down[2][2], arm_up[2][2];
  int shadow_y, shadow_w;
  int hat_x, hat_y;
  int zzz_x, zzz_y;
  uint32_t eye_color, mouth_color, light;  // light: LED colour when poked
};
static const Blueprint PLAN_OKTI = {70, 30, {52, 98}, 60, 0.75f, 22, 22, 75, 90, {18, 114}, 76, 18, 10,
                                  {{0, 0}, {0, 0}}, {{0, 0}, {0, 0}}, 224, 210, 57, -16, 140, -12,
                                  EYE, EYE, YELLOW};
static const Blueprint PLAN_ROBOT = {70, 50, {44, 96}, 36, 0.72f, 22, 26, 70, 60, {12, 110}, 50, 18, 10,
                                     {{20, 126}, {136, 126}}, {{30, 8}, {126, 8}}, 222, 110, 57, -24, 160, -16,
                                     GLOW, GLOW, GLOW};

// Okti arms: root below the head (container coordinates), base direction (degrees, y down), length
static const uint32_t FOX_ORANGE = 0xF07A1E, FOX_CREAM = 0xFFF3E2, FOX_BROWN = 0x3A2418;
static const Blueprint PLAN_FOX = {70, 40, {52, 98}, 50, 0.7f, 22, 20, 75, 84, {20, 112}, 64, 16, 9,
                                   {{40, 146}, {116, 146}}, {{30, 6}, {128, 6}}, 222, 160, 57, -12, 150, -10,
                                   EYE, EYE, FOX_ORANGE};

static const int ARM_X[ARMS] = {38, 52, 68, 82, 98, 112, 128, 142};
static const int ARM_Y = 112;
static const float ARM_ANGLE[ARMS] = {176, 154, 128, 104, 76, 52, 26, 4};
static const int ARM_LENGTH[ARMS] = {112, 100, 88, 76, 76, 88, 100, 112};

// occasions (holidays, birthdays, seasons): hat symbol + up to 8 symbols at the edge (MDI glyphs, font_decor)
struct DecoSet {
  const char *key, *hat;
  uint32_t hat_color;
  const char *glyph[8];
  uint32_t color[8];
};
static const DecoSet DECO_SETS[] = {
    {"easter", "\U000F0907", 0xF4F4F4,
     {"\U000F0AB0", "\U000F0AB0", "\U000F09F1", "\U000F0AB0", "\U000F0AB0", "\U000F024A", "\U000F0AB0", "\U000F0907"},
     {0xFF8FA3, 0x6FB8FF, 0xFF6A8A, 0xFFD23F, 0x5AE07A, 0xFFD23F, 0xC89AFF, 0xF4F4F4}},
    {"new_year", "\U000F0BA4", 0x2A2A36,
     {"\U000F0E30", "\U000F1056", "\U000F0E30", "\U000F0E30", "\U000F0356", "\U000F0E30", "\U000F04CE", "\U000F04CE"},
     {0xFF4A4A, 0xFFB020, 0x4AA8FF, 0x5AE07A, 0xFFD23F, 0xC89AFF, 0xFFFFFF, 0xFFD23F}},
    {"valentine", "\U000F02D1", 0xFF4A6A,
     {"\U000F02D1", "\U000F02D1", "\U000F02D1", "\U000F02D1", "\U000F02D1", "\U000F02D1", "\U000F02D1", "\U000F04CE"},
     {0xFF4A6A, 0xFF8FA3, 0xFF4A6A, 0xFF8FA3, 0xFF4A6A, 0xFF8FA3, 0xFF4A6A, 0xFFD23F}},
    {"carnival", "\U000F0D02", 0xFFD23F,
     {"\U000F1056", "\U000F0A26", "\U000F0387", "\U000F0A26", "\U000F0A26", "\U000F0E30", "\U000F04CE", "\U000F0D02"},
     {0xFFB020, 0xFF4A4A, 0x4AA8FF, 0x4AA8FF, 0x5AE07A, 0xC89AFF, 0xFFD23F, 0xFF8FA3}},
    {"advent", "\U000F05E2", 0xFFE08A,
     {"\U000F05E2", "\U000F0405", "\U000F05E2", "\U000F05E2", "\U000F0405", "\U000F05E2", "\U000F04CE", "\U000F04CE"},
     {0xFFE08A, 0x2E9E5A, 0xFFE08A, 0xFFE08A, 0x2E9E5A, 0xFFE08A, 0xFFD23F, 0xFFFFFF}},
    {"st_nicholas", "\U000F0B47", 0x8B5A2B,
     {"\U000F0B47", "\U000F0E44", "\U000F04CE", "\U000F0B47", "\U000F0E44", "\U000F0405", "\U000F05E2", "\U000F04CE"},
     {0x8B5A2B, 0xFF4A4A, 0xFFD23F, 0x8B5A2B, 0x5AE07A, 0x2E9E5A, 0xFFE08A, 0xFFFFFF}},
    {"st_martin", "\U000F06B5", 0xFFB020,
     {"\U000F06B5", "\U000F0C93", "\U000F06B5", "\U000F06B5", "\U000F0C93", "\U000F06B5", "\U000F04CE", "\U000F04CE"},
     {0xFFB020, 0xC86A2A, 0xFF8A3A, 0xFFD23F, 0xA84E10, 0xFF6A4A, 0xFFFFFF, 0xFFE08A}},
    {"starwars", "\U000F08D8", 0x8A93A6,
     {"\U000F04E5", "\U000F04CE", "\U000F0463", "\U000F04E5", "\U000F04CE", "\U000F089A", "\U000F04CE", "\U000F04CE"},
     {0x4AA8FF, 0xFFFFFF, 0xD0D0D8, 0xFF4A4A, 0xFFFFFF, 0x5AE07A, 0xFFD23F, 0xFFFFFF}},
    {"pi", "\U000F03FF", 0x4AA8FF,
     {"\U000F03FF", "\U000F00EB", "\U000F03FF", "\U000F03FF", "\U000F00EB", "\U000F03FF", "\U000F04CE", "\U000F04CE"},
     {0x4AA8FF, 0xFFD23F, 0xFF8FA3, 0x5AE07A, 0xFFB020, 0xC89AFF, 0xFFFFFF, 0xFFD23F}},
    {"towel", "\U000F0463", 0xD0D0D8,
     {"\U000F0463", "\U000F04CE", "\U000F089A", "\U000F04CE", "\U000F0356", "\U000F04CE", "\U000F04CE", "\U000F08D8"},
     {0xD0D0D8, 0xFFFFFF, 0x5AE07A, 0xFFD23F, 0x5AE07A, 0xFFFFFF, 0xFFFFFF, 0x8A93A6}},
    {"pirate", "\U000F0A08", 0x2A2A36,
     {"\U000F0BC6", "\U000F04E5", "\U000F1116", "\U000F0BC6", "\U000F04E5", "\U000F0387", "\U000F0BC6", "\U000F04CE"},
     {0xF4F4F4, 0xD0D0D8, 0xFFB020, 0xF4F4F4, 0xD0D0D8, 0x4AA8FF, 0xF4F4F4, 0xFFD23F}},
    {"420", "\U000F07A6", 0x5AE07A,
     {"\U000F07A6", "\U000F07A6", "\U000F0387", "\U000F07A6", "\U000F07A6", "\U000F0387", "\U000F07A6", "\U000F04CE"},
     {0x5AE07A, 0x2E9E5A, 0xFFD23F, 0x2E9E5A, 0x5AE07A, 0xFF8FA3, 0x7AF09A, 0xFFFFFF}},
    {"birthday", "\U000F01A5", 0xFFD23F,
     {"\U000F0A26", "\U000F00EB", "\U000F0A26", "\U000F0A26", "\U000F0E44", "\U000F1056", "\U000F0E44", "\U000F04CE"},
     {0xFF4A4A, 0xFFD23F, 0x4AA8FF, 0x5AE07A, 0xFF8FA3, 0xFFB020, 0xC89AFF, 0xFFFFFF}},
    {"oktoberfest", "\U000F0098", 0xFFB020,
     {"\U000F0098", "\U000F1562", "\U000F1116", "\U000F02DF", "\U000F0098", "\U000F1562", "\U000F04CE", "\U000F1116"},
     {0xFFB020, 0xC8823A, 0xFFD23F, 0x5AE07A, 0xFFB020, 0xC8823A, 0xFFFFFF, 0xFFD23F}},
    {"harvest", "\U000F0D7C", 0xE8C060,
     {"\U000F0D7C", "\U000F07B8", "\U000F0892", "\U000F0BBF", "\U000F0C93", "\U000F0D7C", "\U000F07B8", "\U000F0C93"},
     {0xE8C060, 0xFFD23F, 0x5AE07A, 0xF28C28, 0xC8463C, 0xE8C060, 0xFFD23F, 0xE07A1C}},
    {"clock_change", "\U000F0150", 0xD0D0D8,
     {"\U000F0150", "\U000F0594", "\U000F0150", "\U000F0150", "\U000F0594", "\U000F0150", "\U000F04CE", "\U000F04CE"},
     {0xD0D0D8, 0x9FC4FF, 0xFFD23F, 0x6FB8FF, 0x9FC4FF, 0xFF8FA3, 0xFFFFFF, 0xFFD23F}},
    {"full_moon", "\U000F0F62", 0xF5E6A8,
     {"\U000F04CE", "\U000F0F62", "\U000F04CE", "\U000F04CE", "\U000F0594", "\U000F04CE", "\U000F04CE", "\U000F04CE"},
     {0xFFFFFF, 0xF5E6A8, 0xFFD23F, 0xFFFFFF, 0x9FC4FF, 0xFFFFFF, 0xFFD23F, 0xFFFFFF}},
    {"perseids", "\U000F1741", 0xFFD23F,
     {"\U000F1741", "\U000F04CE", "\U000F1741", "\U000F1741", "\U000F04CE", "\U000F1741", "\U000F04CE", "\U000F04CE"},
     {0xFFD23F, 0xFFFFFF, 0x9FC4FF, 0xFFD23F, 0xFFFFFF, 0xFF8FA3, 0xFFFFFF, 0xFFD23F}},
    {"friday13", "\U000F068C", 0xE8E8E8,
     {"\U000F011B", "\U000F02A0", "\U000F068C", "\U000F011B", "\U000F02A0", "\U000F068C", "\U000F04CE", "\U000F011B"},
     {0x5A606E, 0xE8F0FF, 0xE8E8E8, 0x3A3F4A, 0xE8F0FF, 0xD0D0D8, 0xFFD23F, 0x5A606E}},
    {"future", "\U000F0241", 0xFFD23F,
     {"\U000F07AC", "\U000F0241", "\U000F0150", "\U000F07AC", "\U000F0241", "\U000F0150", "\U000F0241", "\U000F04CE"},
     {0xD0D0D8, 0xFFD23F, 0xFF8A3A, 0xD0D0D8, 0x6FB8FF, 0xFF8A3A, 0xFFD23F, 0xFFFFFF}},
    {"hobbit", "\U000F07EB", 0xFFD23F,
     {"\U000F07EB", "\U000F0405", "\U000F0405", "\U000F07EB", "\U000F0405", "\U000F0405", "\U000F04CE", "\U000F04CE"},
     {0xFFD23F, 0x2E9E5A, 0x5AE07A, 0xFFB020, 0x2E9E5A, 0x5AE07A, 0xFFFFFF, 0xFFD23F}},
    {"coffee", "\U000F0176", 0xC8823A,
     {"\U000F0176", "\U000F0176", "\U000F02D1", "\U000F0176", "\U000F0176", "\U000F02D1", "\U000F0176", "\U000F04CE"},
     {0xC8823A, 0xE8C8A8, 0xFF8FA3, 0x8B5A2B, 0xC8823A, 0xFF8FA3, 0xE8C8A8, 0xFFD23F}},
    {"programmer", "\U000F0169", 0x5AE07A,
     {"\U000F0169", "\U000F018D", "\U000F00E4", "\U000F0169", "\U000F018D", "\U000F00E4", "\U000F0169", "\U000F04CE"},
     {0x5AE07A, 0x6FB8FF, 0xFF6A4A, 0xFFD23F, 0x5AE07A, 0xFF6A4A, 0xC89AFF, 0xFFFFFF}},
    {"cat", "\U000F011B", 0xFFB020,
     {"\U000F011B", "\U000F03E9", "\U000F011B", "\U000F011B", "\U000F03E9", "\U000F011B", "\U000F02D1", "\U000F03E9"},
     {0xFFB020, 0xFF8FA3, 0x5A606E, 0xF4F4F4, 0xFF8FA3, 0xC8823A, 0xFF4A6A, 0xFFB020}},
    {"mothers_day", "\U000F1314", 0xFF8FA3,
     {"\U000F09F1", "\U000F02D1", "\U000F1314", "\U000F09F1", "\U000F02D1", "\U000F09F1", "\U000F04CE", "\U000F02D1"},
     {0xFF6A8A, 0xFF4A6A, 0xFF8FA3, 0xFFD23F, 0xFF8FA3, 0xC89AFF, 0xFFFFFF, 0xFF4A6A}},
    {"fathers_day", "\U000F0CE3", 0x4AA8FF,
     {"\U000F0098", "\U000F0CE3", "\U000F0098", "\U000F0098", "\U000F025D", "\U000F0098", "\U000F04CE", "\U000F04CE"},
     {0xFFB020, 0x4AA8FF, 0xFFD23F, 0xFFB020, 0xC8823A, 0xFFD23F, 0xFFFFFF, 0xFFD23F}},
    {"solstice", "\U000F17FE", 0xFFD23F,
     {"\U000F0599", "\U000F17FE", "\U000F188A", "\U000F0599", "\U000F17FE", "\U000F0599", "\U000F04CE", "\U000F04CE"},
     {0xFFD23F, 0xFFB020, 0xFF8A3A, 0xFFD23F, 0xFFB020, 0xFF8A3A, 0xFFFFFF, 0xFFD23F}},
    {"blackfriday", "\U000F0110", 0x3A3F4A,
     {"\U000F0110", "\U000F068B", "\U000F0110", "\U000F0110", "\U000F068B", "\U000F0110", "\U000F068B", "\U000F04CE"},
     {0xD0D0D8, 0xFF4A4A, 0x5A606E, 0xD0D0D8, 0xFF4A4A, 0x5A606E, 0xFFD23F, 0xFFFFFF}},
    {"ufo", "\U000F089A", 0x5AE07A,
     {"\U000F089A", "\U000F14DE", "\U000F0BC9", "\U000F089A", "\U000F14DE", "\U000F0BC9", "\U000F04CE", "\U000F04CE"},
     {0x5AE07A, 0xD0D0D8, 0xC89AFF, 0x7AF09A, 0xFF8A3A, 0x5AE07A, 0xFFFFFF, 0xFFD23F}},
    {"emoji", "\U000F0C71", 0xFFD23F,
     {"\U000F0C71", "\U000F0C6B", "\U000F0C78", "\U000F1214", "\U000F0C70", "\U000F0C72", "\U000F01F9", "\U000F01F7"},
     {0xFFD23F, 0xFFB020, 0xFFD23F, 0xFF8A3A, 0xFFD23F, 0xFF8FA3, 0xFFB020, 0x8B5A2B}},
    {"beer", "\U000F02A6", 0xFFB020,
     {"\U000F02A6", "\U000F0098", "\U000F02A6", "\U000F02A6", "\U000F0098", "\U000F02A6", "\U000F04CE", "\U000F1562"},
     {0xFFB020, 0xFFD23F, 0xFFD23F, 0xFFB020, 0xFFD23F, 0xFFB020, 0xFFFFFF, 0xC8823A}},
    {"spring", "", 0,
     {"\U000F024A", "\U000F09F1", "\U000F0FA1", "\U000F024A", "\U000F1589", "\U000F09F1", "\U000F0599", "\U000F024A"},
     {0xFFD23F, 0xFF6A8A, 0xFFD23F, 0xC89AFF, 0x6FB8FF, 0xFF8A3A, 0xFFD23F, 0xFF8FA3}},
    {"summer", "", 0,
     {"\U000F0599", "\U000F188A", "\U000F0356", "\U000F0599", "\U000F1589", "\U000F04CE", "\U000F04CE", "\U000F0FA1"},
     {0xFFD23F, 0xFF8A3A, 0x5AE07A, 0xFFB020, 0x6FB8FF, 0xFFFFFF, 0xFFD23F, 0xFFD23F}},
    {"autumn", "", 0,
     {"\U000F0C93", "\U000F032A", "\U000F07DF", "\U000F0C93", "\U000F0C93", "\U000F032A", "\U000F07DF", "\U000F0C93"},
     {0xE07A1C, 0xC8A02A, 0xC86A4A, 0xC8463C, 0xFFB020, 0xA84E10, 0xE8C8A8, 0xD05A1A}},
    {"winter", "", 0,
     {"\U000F0717", "\U000F0717", "\U000F04CE", "\U000F0717", "\U000F0717", "\U000F0717", "\U000F04CE", "\U000F0717"},
     {0xE8F0FF, 0x9FC4FF, 0xFFFFFF, 0xE8F0FF, 0x9FC4FF, 0xE8F0FF, 0xFFD23F, 0x9FC4FF}},
};
static const int DECO_POS[8][2] = {{8, 36}, {16, 100}, {6, 168}, {274, 50}, {282, 116}, {268, 186}, {66, 4}, {204, 6}};

static inline bool passed(uint32_t now_ms, uint32_t t) { return t != 0 && (int32_t) (now_ms - t) >= 0; }
static inline uint32_t rnd(uint32_t n) { return esphome::random_uint32() % n; }

}  // namespace yellow
