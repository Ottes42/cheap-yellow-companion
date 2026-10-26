# Changelog

## [Unreleased]

- **First tap of the day** (#20): from 05:00 the first tap greets ("Good morning"/"Hi", name, a line per weekday,
  weather, optional line from `data/greeting`), once a day, remembered over reboots. New blueprint
  `yellow_greeting.yaml` writes that line every morning from an AI Task or an entity. Test commands `greeting_test`
  and `tap`.
- Fox: 17 more sayings about courage and fear (30 in total).
- Okti: 20 more sayings (mostly clumsy, 30 in total); robot: 19 more (overly precise, 30 in total).

## [0.3.0] – 2026-10-25

- **Dashboard card** (#11): the real Yellow in a Lovelace card, the unchanged firmware headers and LVGL compiled to
  WebAssembly (`web/`, one module per language). Touch like on the device; follows the device's character,
  decoration, mood, tension and energy and a weather entity. Install and build: `docs/card.md` (+ German copy); the
  files are attached to the release.
- `docs_de/FEATURES.md`: German feature list

- `docs/homeassistant.md` (+ German copy): setup, entities, language, every blueprint with import button and example;
  the README links it instead of the blueprint table (#33)

## [0.2.0] – 2026-10-25

English first, translatable at compile time (#1). **Breaking:** MQTT interface, file names and substitutions are
English now; see the migration table below.

- Languages: substitution `language: en` (default) or `de`; all on-screen texts in `esphome/lang/<language>.h`,
  entity names and YAML texts in `esphome/lang/<language>.yaml`. Date and decimal format follow the language.
  Switching the language gives new entity IDs in Home Assistant.
- All code, identifiers and comments in English; select logic uses indices instead of option texts.
- Docs: `README.de.md` and `docs_de/` (German), `docs/galerie.md` → `docs/gallery.md`, media files with English names.
- `FEATURES.md`: complete feature list with "since" version (#31)
- Gallery page and README GIFs, recorded pixel-exact from the device
- Optional package `esphome/yellow-screenshot.yaml` (command `image`) and `tools/screenshot.py` for screenshots and
  GIFs over MQTT, `tools/frame.py` puts them into a drawn CYD board
- Blueprint "send data": new input `language` for the words on the device ("Today", "Alarm")

### Migration from 0.1.0

| What | 0.1.0 | 0.2.0 |
|---|---|---|
| Packages | `yellow-basis.yaml`, `yellow-zuhause.yaml`, `yellow-unterwegs.yaml`, `yellow-bild.yaml`, `example-zuhause.yaml` | `yellow-base.yaml`, `yellow-home.yaml`, `yellow-mobile.yaml`, `yellow-screenshot.yaml`, `example-home.yaml` |
| Substitutions | `geraet`, `mqtt_passwort`, `figur_name`, `figur_fest`, `yellow_pfad`, `farbe_tag(_2)`, `farbe_nacht(_2)`, `ldr_dunkel`, `drehung` | `device`, `mqtt_password`, `companion_name`, `fixed_character`, `yellow_path`, `color_day(_2)`, `color_night(_2)`, `ldr_dark`, `rotation`; new `language` (set `de` to keep the German texts and entity IDs) |
| Restart reason id | `neustartgrund` | `reset_reason` (the `debug` text sensor id the telemetry reads) |
| Topics | `befehl`, `ereignis`, `telemetrie`, `daten/…`, `bild/…` | `command`, `event`, `telemetry`, `data/…`, `image/…` |
| Data names | anlass, wecker, termin, hinweis, wetter, wetter_lage, aussen_temp, raum_temp, raum_feuchte, sprueche, klingel | occasion, alarm, appointment, notice, weather, weather_condition, outdoor_temp, room_temp, room_humidity, sayings, doorbell |
| Command | `{"aktion": …}`; nachricht, liebesbrief, ausdruck, requisit, deko_test, effekt, langeweile_test, verreist, netz_test, wetter, schmuck, accessoire, spiel, frage, geste, blick, nabu_richtung, schlafen, telemetrie, wlan_dialog, bild | `{"action": …}`; message, love_letter, expression, prop, decor_test, effect, boredom_test, away, network_test, weather, jewelry, accessory, game, question, gesture, look, nabu_direction, sleep, telemetry, wifi_dialog, image |
| Fields | inhalt, warten, sekunden, minuten, an, karte, stufe, lage, optionen, richtung, geste, ausdruck, requisit, anlass, spiel, frage | text, wait, seconds, minutes, on, card, level, condition, options, direction, gesture, expression, prop, occasion, game, question |
| Values | gestures (winken → wave, huepfen → hop …), expressions (froh → happy …), props (sonnenbrille → sunglasses …), effects, accessories, directions, occasions (ostern → easter …), game `reaktion` → `reaction` | see the lists in the README |
| Events | `typ`: gelesen, antwort, spiel (`spiel`, `wert`), gestreichelt, timer_aus | `type`: read, answer, game (`game`, `value`), petted, timer_off |
| Love letter prefix | `LIEBE|` | `LOVE|` |
| Telemetry keys | laufzeit_s, neustartgrund, frei, wlan, dunkel, ansicht (`uhr`), figur, heute, protokoll | uptime_s, reset_reason, free, wifi, dark, view (`clock`), character, today, log |
| Blueprints | yellow_daten, yellow_klingel, yellow_post_gelesen, yellow_sprachassistent_ring, yellow_nachricht (inputs `geraet` …) | yellow_data, yellow_doorbell, yellow_post_read, yellow_voice_assistant_ring, yellow_message (inputs `device` …): re-import |

## [0.1.0] – 2026-10-25

First public version, in daily use on one bedside display.

- Characters Okti, fox and robot (shapes only), mood/tension/energy, expressions, gestures, props, special moves,
  jewellery and accessories
- Touch: tap, pet, hold, 4× tap, help page, game picker, fist bump
- Games: reaction test, tic-tac-toe, Simon Says
- Post and love letters with read receipt, questions with answer buttons, timer display
- Occasions, seasons, weather view, Matrix rain, night rules, clock view
- Profiles: home (local MQTT + Home Assistant) and away (MQTT over TLS, SNTP)
- MQTT for commands, data, events, telemetry with activity log, status with last will
- Network status (red Wi‑Fi, orange server/internet) with Wi‑Fi dialog and on-screen keyboard
- Remote one-liner list, stored on the device
- Optional voice assistant package (looks at the LED ring)
- Home Assistant blueprints: send data, doorbell, voice assistant ring, read receipt, message script
