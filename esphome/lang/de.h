#pragma once
// Yellow: all on-screen texts, German. Chosen at compile time with the substitution
// `language` (lang/<language>.h); every text needs an entry in each language file.

namespace yellow {
namespace txt {

constexpr const char *BOOT_1 = "Moment bitte …";
constexpr const char *BOOT_2 = "Bitte warten …";
constexpr const char *BOOT_3 = "Lade …";
constexpr const char *BOOT_4 = "Wache auf …";
constexpr const char *BOOT_5 = "Suche meine Schilder …";
constexpr const char *BOOT_6 = "Strecke mich …";
constexpr const char *BOOT_7 = "Gleich da!";
constexpr const char *BOOT_8 = "Kaffee?";
constexpr const char *NO_ANSWER = "(keine Antwort)";
constexpr const char *HELLO = "Hallo!";
constexpr const char *NO_WIFI_FOUND = "kein WLAN gefunden";
constexpr const char *HERE_TEMP = "Hier %.1f°";
constexpr const char *NOTHING_TODAY = "Nix los heute.";
constexpr const char *AT_THE_DOOR_TIME = "an der Haustür, %02d:%02d";
constexpr const char *AT_THE_DOOR = "an der Haustür";
constexpr const char *HUFF = "Hmpf.";
constexpr const char *OFFENDED = "Bin beleidigt.";
constexpr const char *NOW_ALL_OF_A_SUDDEN = "Jetzt auf einmal?";
constexpr const char *PET_TWICE = "Streicheln. Zweimal.";
constexpr const char *PLEASE_DONT = "Bitte nicht.";
constexpr const char *OFF_TO_NABU = "Bin kurz bei Nabu.";
constexpr const char *ORACLE_1 = "Ja.";
constexpr const char *ORACLE_2 = "Nein.";
constexpr const char *ORACLE_3 = "Eindeutig.";
constexpr const char *ORACLE_4 = "Lieber nicht.";
constexpr const char *ORACLE_5 = "Frag Nabu.";
constexpr const char *ORACLE_6 = "Pizza.";
constexpr const char *ORACLE_7 = "Morgen.";
constexpr const char *ORACLE_8 = "Frag später nochmal.";
constexpr const char *ORACLE_9 = "Na klar.";
constexpr const char *ORACLE_10 = "Träum weiter.";
constexpr const char *ORACLE_11 = "Auf jeden Fall.";
constexpr const char *NO_COMMENT = "Kein Kommentar.";
constexpr const char *NOT_SCARED = "Hab mich nicht erschreckt.";
constexpr const char *TAPPED_TODAY = "Heute %d× angetippt. Reicht.";
constexpr const char *NOT_PETTED_TODAY = "Heute noch nicht gestreichelt. Nur so.";
constexpr const char *PETTED_COUNT = "%d× gestreichelt. Ich zähle mit.";
constexpr const char *GAMES_TODAY = "%d Spiele heute. Ich bin erschöpft.";
constexpr const char *ZERO_TOUCHES = "Heute: 0 Berührungen. Statistik lügt nicht.";
constexpr const char *HELLO_Q = "Hallo?";
constexpr const char *ANYONE_THERE = "Jemand da?";
constexpr const char *BORED = "Mir ist langweilig.";
constexpr const char *DOOR = "TÜR!!";
constexpr const char *ILL_CHECK = "Ich geh nachsehen.";
constexpr const char *WHO_THERE = "Wer da? Keine Angst.";
constexpr const char *DOOR_ROBOT = "TÜR!! (98 %)";
constexpr const char *SHOW_OFF = "Angeber.";
constexpr const char *OOH = "Ooh!";
constexpr const char *HOW_DO_YOU = "Wie machst du das?";
constexpr const char *VISITOR = "Oh, Besuch!";
constexpr const char *LOVE_LETTER_FOX = "Psst. Ein Liebesbrief!";
constexpr const char *LOVE_LETTER = "Liebesbrief!";
constexpr const char *POST_COUNT = "%d x Post für dich!";
constexpr const char *POST = "Post für dich!";
constexpr const char *TAP_ME = "Tipp mich an!";
constexpr const char *THANKS = "Danke!";
constexpr const char *OKAY_OKAY = "Okay, okay.";
constexpr const char *NOT_QUITE = "Noch nicht ganz.";
constexpr const char *FINE = "Na gut.";
constexpr const char *PULSE_HIGH = "Puls: hoch.";
constexpr const char *ALL_QUIET = "Alles ruhig. Oder?";
constexpr const char *WHAT_WAS_THAT = "Was war das?!";
constexpr const char *OOPS = "Hoppla.";
constexpr const char *WHO_PUT_ARM = "Wer hat den Arm da hingelegt?";
constexpr const char *TIMER_LEFT_ROBOT = "Noch %d s. Exakt.";
constexpr const char *TIMER = "Timer!";
constexpr const char *THUNDER = "Pff. Donner.";
constexpr const char *SNOW = "Schnee!";
constexpr const char *UH_OH = "Oh oh.";
constexpr const char *RAIN = "Regen.";
constexpr const char *SUN = "Sonne!";
constexpr const char *WHEEE = "Huiii!";
constexpr const char *FRESH_AIR = "Frische Luft.";
constexpr const char *WIND_ROBOT = "Wind: %d km/h. Geschätzt.";
constexpr const char *ERROR_FIXED = "Fehler korrigiert.";
constexpr const char *AWAY = "Bin verreist. Bis bald!";
constexpr const char *BACK = "Wieder da!";
constexpr const char *HELP_TITLE = "So geht Yellow";
constexpr const char *HELP_TEXT = "Antippen: freut sich, holt Post ab\n"
                                     "4x schnell: lieber nicht ...\n"
                                     "Wischen: streicheln\n"
                                     "Halten: Wecker, Termin, Wetter\n"
                                     "Uhr oben rechts: 15 s Uhr\n"
                                     "Im Dunkeln antippen: Uhr\n"
                                     "Faust? Antippen!";
constexpr const char *TAP_CLOSES = "Antippen schließt";
constexpr const char *WHAT_GAME = "Was spielen wir?";
constexpr const char *GAME_REACTION = "Reaktionstest";
constexpr const char *GAME_TTT = "Tic-Tac-Toe";
constexpr const char *GAME_SIMON = "Simon Says";
constexpr const char *TAP_OUTSIDE_CLOSES = "Daneben tippen schließt";
constexpr const char *WAITING_FOR_NETWORK = "wartet auf Netz";
constexpr const char *SERVER_UNREACHABLE = "Server nicht erreichbar";
constexpr const char *NO_INTERNET = "kein Internet";
constexpr const char *FIST_Q = "Faust?";
constexpr const char *BOOM = "Bumm!";
constexpr const char *LIFT_OFF = "Abheben!";
constexpr const char *TA_DA = "Ta-da!";
constexpr const char *ALMOST = "Fast!";
constexpr const char *FAST_ONE = "Der ist schnell.";
constexpr const char *ONE_MOMENT = "Moment.";
constexpr const char *WHICH_ARM = "Welcher Arm war das?";
constexpr const char *PHEW = "Puh.";
constexpr const char *MAINTENANCE = "Wartung.";
constexpr const char *QUICK_LOOK = "Kurz reinschauen.";
constexpr const char *MOUSE_NO = "Maus? Nein.";
constexpr const char *NOTHING = "Nix.";
constexpr const char *DIRT_YUM = "Erde. Lecker.";
constexpr const char *REBOOT_DONE = "Neustart… fertig.";
constexpr const char *THAT_WAS_NOTHING = "Das war nichts.";
constexpr const char *SYSTEM_UP = "System hochgefahren.";
constexpr const char *AWAKE = "Bin wach. Ganz wach.";
constexpr const char *MORNING_ALREADY = "Schon morgen?";
constexpr const char *WAS_AWAKE = "Ich war wach. Ehrlich.";
constexpr const char *BRIGHTNESS_800 = "Helligkeit +800 %.";
constexpr const char *LIGHT_EYES = "Licht! Augen!";
constexpr const char *WHO_TURNED_ON = "Wer hat das angemacht?";
constexpr const char *REACTION_RULES = "Wenn ich springe: tippen!";
constexpr const char *COPY_ME = "Mach's mir nach.";
constexpr const char *YOU_START = "Du fängst an.";
constexpr const char *NO = "Nein";
constexpr const char *YES = "Ja";
constexpr const char *DIDNT_SEE = "Hab ich nicht gesehen.";
constexpr const char *FINE_YOU = "Na gut. Du.";
constexpr const char *WON = "Ha! Gewonnen.";
constexpr const char *DRAW = "Unentschieden.";
constexpr const char *NOW = "JETZT!";
constexpr const char *FELL_ASLEEP = "Eingeschlafen?";
constexpr const char *HAD_ENOUGH = "Keine Lust mehr?";
constexpr const char *TOO_EARLY = "Zu früh!";

// Formats and pieces of sentences
constexpr const char *GREETING_FMT = "Hallo %s!\nIch bin %s.";  // person, companion name
constexpr const char *TRYING_WIFI_FMT = "Ich probier's mit\n%s.";
constexpr const char *TIMER_PREFIX = "Timer: ";
constexpr const char *TIMER_LEFT_FMT = "Noch %s";
constexpr const char *IN_NETWORK = " in ";
constexpr const char *ROUNDS_FMT = "%d Runde%s.";
constexpr const char *ROUNDS_PLURAL = "n";
constexpr const char *DATE_FMT = "%s %d. %s";  // weekday, day, month
constexpr bool DECIMAL_COMMA = true;
static const char *const DAYS[7] = {"So", "Mo", "Di", "Mi", "Do", "Fr", "Sa"};
static const char *const MONTHS[12] = {"Jan", "Feb", "Mär", "Apr", "Mai", "Jun", "Jul", "Aug", "Sep", "Okt", "Nov", "Dez"};

// Idle sayings: shared plus per character (Okti clumsy, robot overly precise, fox brave)
static const char *const SAYINGS[] = {"…", "Ich sag nix.", "Hallo.", "Langweilig.", "Psst.", "Kein Kommentar.",
                                      "Streicheln?", "Hab Hunger. Auf Strom.", "Bin beschäftigt.", "Schau nicht so.",
                                      "Heute ist ein guter Tag.", "Ich hab nachgedacht. Nix.", "Pause.", "Noch da."};
static const char *const SAYINGS_OKTI[] = {"Acht Arme. Null Worte.", "Yellow. Nicht Okti. Okay, beides.",
                                           "Ich bin gelb.", "Wo war welcher Arm?", "Hoppla.", "Arm eingeschlafen.",
                                           "Ich hab alles im Griff. Achtfach.", "Tinte? Nur im Notfall.",
                                           "Knoten? Welcher Knoten?", "Rot? Nö. Gelb."};
static const char *const SAYINGS_ROBOT[] = {"Berechne…", "Langeweile: 87 %.", "Piep.", "Alle Systeme gelb.",
                                            "Fehler 404: Spaß.", "Laufzeit: optimal.", "Wartung? Später.",
                                            "Ich zähle. Sie nicht.", "Logik sagt: Pause.", "Update? Nein danke.",
                                            "Akku: 100 %. Laune: 73 %."};
static const char *const SAYINGS_FOX[] = {"Ich hab keine Angst.", "Schlau wie ein Fuchs.", "Hab was gehört.",
                                          "Mutig sein. Erledigt.", "Ein Fuchs schläft nie. Fast.", "Du schaffst das.",
                                          "Mut ist, es trotzdem zu tun.", "Angst? Kenn ich nur vom Hörensagen.",
                                          "Ich pass auf dich auf.", "Heute probieren wir was Neues.",
                                          "Kopf hoch, Ohren auch.", "Ich geh vor.", "Das wird gut."};

}  // namespace txt
}  // namespace yellow
