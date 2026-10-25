#pragma once
// Yellow: all on-screen texts, English. Chosen at compile time with the substitution
// `language` (lang/<language>.h); every text needs an entry in each language file.

namespace yellow {
namespace txt {

constexpr const char *BOOT_1 = "One moment …";
constexpr const char *BOOT_2 = "Please wait …";
constexpr const char *BOOT_3 = "Loading …";
constexpr const char *BOOT_4 = "Waking up …";
constexpr const char *BOOT_5 = "Looking for my signs …";
constexpr const char *BOOT_6 = "Stretching …";
constexpr const char *BOOT_7 = "Almost there!";
constexpr const char *BOOT_8 = "Coffee?";
constexpr const char *NO_ANSWER = "(no answer)";
constexpr const char *HELLO = "Hello!";
constexpr const char *NO_WIFI_FOUND = "no Wi-Fi found";
constexpr const char *HERE_TEMP = "Here %.1f°";
constexpr const char *NOTHING_TODAY = "Nothing on today.";
constexpr const char *AT_THE_DOOR_TIME = "at the front door, %02d:%02d";
constexpr const char *AT_THE_DOOR = "at the front door";
constexpr const char *HUFF = "Hmpf.";
constexpr const char *OFFENDED = "I'm offended.";
constexpr const char *NOW_ALL_OF_A_SUDDEN = "Oh, now you care?";
constexpr const char *PET_TWICE = "Pet me. Twice.";
constexpr const char *PLEASE_DONT = "Please don't.";
constexpr const char *OFF_TO_NABU = "Off to see Nabu.";
constexpr const char *ORACLE_1 = "Yes.";
constexpr const char *ORACLE_2 = "No.";
constexpr const char *ORACLE_3 = "Definitely.";
constexpr const char *ORACLE_4 = "Better not.";
constexpr const char *ORACLE_5 = "Ask Nabu.";
constexpr const char *ORACLE_6 = "Pizza.";
constexpr const char *ORACLE_7 = "Tomorrow.";
constexpr const char *ORACLE_8 = "Ask again later.";
constexpr const char *ORACLE_9 = "Of course.";
constexpr const char *ORACLE_10 = "Dream on.";
constexpr const char *ORACLE_11 = "Absolutely.";
constexpr const char *NO_COMMENT = "No comment.";
constexpr const char *NOT_SCARED = "Didn't scare me.";
constexpr const char *TAPPED_TODAY = "Tapped %d× today. Enough.";
constexpr const char *NOT_PETTED_TODAY = "Not petted today. Just saying.";
constexpr const char *PETTED_COUNT = "Petted %d×. I'm counting.";
constexpr const char *GAMES_TODAY = "%d games today. I'm exhausted.";
constexpr const char *ZERO_TOUCHES = "Today: 0 touches. Statistics don't lie.";
constexpr const char *HELLO_Q = "Hello?";
constexpr const char *ANYONE_THERE = "Anyone there?";
constexpr const char *BORED = "I'm bored.";
constexpr const char *DOOR = "DOOR!!";
constexpr const char *ILL_CHECK = "I'll go and check.";
constexpr const char *WHO_THERE = "Who's there? Don't worry.";
constexpr const char *DOOR_ROBOT = "DOOR!! (98 %)";
constexpr const char *SHOW_OFF = "Show-off.";
constexpr const char *OOH = "Ooh!";
constexpr const char *HOW_DO_YOU = "How do you do that?";
constexpr const char *VISITOR = "Oh, a visitor!";
constexpr const char *LOVE_LETTER_FOX = "Psst. A love letter!";
constexpr const char *LOVE_LETTER = "Love letter!";
constexpr const char *POST_COUNT = "%d letters for you!";
constexpr const char *POST = "Post for you!";
constexpr const char *TAP_ME = "Tap me!";
constexpr const char *THANKS = "Thanks!";
constexpr const char *OKAY_OKAY = "Okay, okay.";
constexpr const char *NOT_QUITE = "Not quite yet.";
constexpr const char *FINE = "Fine.";
constexpr const char *PULSE_HIGH = "Pulse: high.";
constexpr const char *ALL_QUIET = "All quiet. Right?";
constexpr const char *WHAT_WAS_THAT = "What was that?!";
constexpr const char *OOPS = "Oops.";
constexpr const char *WHO_PUT_ARM = "Who put that arm there?";
constexpr const char *TIMER_LEFT_ROBOT = "%d s left. Exactly.";
constexpr const char *TIMER = "Timer!";
constexpr const char *THUNDER = "Pff. Thunder.";
constexpr const char *SNOW = "Snow!";
constexpr const char *UH_OH = "Uh-oh.";
constexpr const char *RAIN = "Rain.";
constexpr const char *SUN = "Sun!";
constexpr const char *WHEEE = "Wheee!";
constexpr const char *FRESH_AIR = "Fresh air.";
constexpr const char *WIND_ROBOT = "Wind: %d km/h. Estimated.";
constexpr const char *ERROR_FIXED = "Error corrected.";
constexpr const char *AWAY = "Away. Back soon!";
constexpr const char *BACK = "Back again!";
constexpr const char *HELP_TITLE = "How Yellow works";
constexpr const char *HELP_TEXT = "Tap: he's happy, gets the post\n"
                                     "4x fast: better not ...\n"
                                     "Swipe: pet him\n"
                                     "Hold: alarm, appointment, weather\n"
                                     "Clock top right: 15 s clock\n"
                                     "Tap in the dark: clock\n"
                                     "Fist? Tap it!";
constexpr const char *TAP_CLOSES = "Tap to close";
constexpr const char *WHAT_GAME = "What shall we play?";
constexpr const char *GAME_REACTION = "Reaction test";
constexpr const char *GAME_TTT = "Tic-tac-toe";
constexpr const char *GAME_SIMON = "Simon Says";
constexpr const char *TAP_OUTSIDE_CLOSES = "Tap outside to close";
constexpr const char *WAITING_FOR_NETWORK = "waiting for network";
constexpr const char *SERVER_UNREACHABLE = "server unreachable";
constexpr const char *NO_INTERNET = "no internet";
constexpr const char *FIST_Q = "Fist?";
constexpr const char *BOOM = "Boom!";
constexpr const char *LIFT_OFF = "Lift-off!";
constexpr const char *TA_DA = "Ta-da!";
constexpr const char *ALMOST = "Almost!";
constexpr const char *FAST_ONE = "That one's fast.";
constexpr const char *ONE_MOMENT = "One moment.";
constexpr const char *WHICH_ARM = "Which arm was that?";
constexpr const char *PHEW = "Phew.";
constexpr const char *MAINTENANCE = "Maintenance.";
constexpr const char *QUICK_LOOK = "Just a quick look.";
constexpr const char *MOUSE_NO = "Mouse? No.";
constexpr const char *NOTHING = "Nothing.";
constexpr const char *DIRT_YUM = "Dirt. Yum.";
constexpr const char *REBOOT_DONE = "Rebooting… done.";
constexpr const char *THAT_WAS_NOTHING = "That was nothing.";
constexpr const char *SYSTEM_UP = "System booted.";
constexpr const char *AWAKE = "I'm awake. Fully awake.";
constexpr const char *MORNING_ALREADY = "Morning already?";
constexpr const char *WAS_AWAKE = "I was awake. Honestly.";
constexpr const char *BRIGHTNESS_800 = "Brightness +800 %.";
constexpr const char *LIGHT_EYES = "Light! My eyes!";
constexpr const char *WHO_TURNED_ON = "Who turned that on?";
constexpr const char *REACTION_RULES = "When I jump: tap!";
constexpr const char *COPY_ME = "Copy me.";
constexpr const char *YOU_START = "You start.";
constexpr const char *NO = "No";
constexpr const char *YES = "Yes";
constexpr const char *DIDNT_SEE = "I didn't see that.";
constexpr const char *FINE_YOU = "Fine. You.";
constexpr const char *WON = "Ha! I won.";
constexpr const char *DRAW = "Draw.";
constexpr const char *NOW = "NOW!";
constexpr const char *FELL_ASLEEP = "Fell asleep?";
constexpr const char *HAD_ENOUGH = "Had enough?";
constexpr const char *TOO_EARLY = "Too early!";

// Formats and pieces of sentences
constexpr const char *GREETING_FMT = "Hello %s!\nI'm %s.";  // person, companion name
constexpr const char *TRYING_WIFI_FMT = "Trying\n%s.";
constexpr const char *TIMER_PREFIX = "Timer: ";
constexpr const char *TIMER_LEFT_FMT = "%s left";
constexpr const char *IN_NETWORK = " on ";
constexpr const char *ROUNDS_FMT = "%d round%s.";
constexpr const char *ROUNDS_PLURAL = "s";
constexpr const char *DATE_FMT = "%s %d %s";  // weekday, day, month
constexpr bool DECIMAL_COMMA = false;
static const char *const DAYS[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
static const char *const MONTHS[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

// Idle sayings: shared plus per character (Okti clumsy, robot overly precise, fox brave)
static const char *const SAYINGS[] = {"…", "Not saying a thing.", "Hello.", "Boring.", "Psst.", "No comment.",
                                      "Pet me?", "Hungry. For power.", "Busy.", "Don't look at me like that.",
                                      "Today is a good day.", "I've been thinking. Nothing.", "Break.", "Still here."};
static const char *const SAYINGS_OKTI[] = {"Eight arms. Zero words.", "Yellow. Not Okti. Okay, both.",
                                           "I'm yellow.", "Which arm was where?", "Oops.", "Arm fell asleep.",
                                           "Got it all under control. Eightfold.", "Ink? Emergencies only.",
                                           "Knot? What knot?", "Red? Nope. Yellow."};
static const char *const SAYINGS_ROBOT[] = {"Computing…", "Boredom: 87 %.", "Beep.", "All systems yellow.",
                                            "Error 404: fun.", "Uptime: optimal.", "Maintenance? Later.",
                                            "I count. You don't.", "Logic says: break.", "Update? No thanks.",
                                            "Battery: 100 %. Mood: 73 %."};
static const char *const SAYINGS_FOX[] = {"I'm not scared.", "Clever as a fox.", "Heard something.",
                                          "Be brave. Done.", "A fox never sleeps. Almost.", "You've got this.",
                                          "Courage is doing it anyway.", "Fear? Only heard of it.",
                                          "I've got your back.", "Let's try something new today.",
                                          "Chin up, ears too.", "I'll go first.", "This will be good."};

}  // namespace txt
}  // namespace yellow
