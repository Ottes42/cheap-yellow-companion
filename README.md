# Cheap Yellow Companion

🇩🇪 [Deutsche Version](README.de.md)

**Yellow** is a small, mute desk/bedside companion for the *Cheap Yellow Display* (ESP32-2432S028R, 320×240, resistive
touch, RGB LED, no PSRAM), built with **ESPHome + LVGL 9** and talking **MQTT**, with or without **Home Assistant**.

Yellow cannot talk and knows it – he holds up cardboard signs, pulls faces, waves and is a little jealous of the voice
assistant that is allowed to speak. Three characters: **Okti** (a reversible octopus, yellow and happy or red and
grumpy), a **fox** (brave, a bit cheeky) and a small **robot**.

<p align="center">
  <img src="docs/media/okti-juggle.gif" width="32%" alt="Okti juggles">
  <img src="docs/media/fox-chase-tail.gif" width="32%" alt="The fox chases its tail">
  <img src="docs/media/robot-propeller.gif" width="32%" alt="The robot takes off">
</p>

<p align="center"><sub>Real screen output, recorded pixel by pixel from the device. More in the
<a href="docs/gallery.md">gallery</a> – but not everything, find the rest yourself.</sub></p>

## Features

- Animated characters drawn from LVGL shapes, arcs and lines only (no bitmaps): breathing, blinking, looking at your
  finger, arms/tail that reach, hold signs and wave. Mood, tension and energy sliders.
- Touch: tap, swipe to pet, hold for the day's info, 4× quick tap (he does not like that), **?** opens the help,
  the gamepad button (bottom left) starts a game.
- **Games:** reaction test, tic-tac-toe (he cheats when losing), Simon Says.
- **Post and love letters:** messages wait in an envelope until someone taps Yellow; read receipt back.
- **Questions** with two touch answer buttons.
- Gestures, props, special moves per character (ink cloud, fox dive, robot glitch, juggling …), fist bump.
- **Occasions and seasons** with background decoration and hats; weather view; Matrix rain.
- Night rules: in a dark room Yellow never reacts; a tap shows a very dim clock. Landscape clock view with alarm,
  next appointment, weather and room climate.
- **Network status:** red icon when Wi‑Fi is gone (tap it → Wi‑Fi dialog with network list and on-screen keyboard),
  orange when the MQTT server or the internet is missing.
- **Telemetry and activity log** (MQTT, retained), **remote one-liners** (your own list, stored on the device).
- **English or German**, chosen at compile time (`language: en|de`).
- Optional: **together with a voice assistant** (Home Assistant Voice), see [docs/nabu.md](docs/nabu.md).

The complete list, with the version each feature appeared in: [FEATURES.md](FEATURES.md).

## Profiles

| Package | Use |
|---|---|
| `yellow-base.yaml` | Character, display, touch, games, help, Wi‑Fi dialog (always) |
| `yellow-mqtt.yaml` | Commands, data, events, telemetry over MQTT (always) |
| `yellow-home.yaml` | **Home:** local broker without TLS, time from Home Assistant, ESPHome API for OTA and logs |
| `yellow-mobile.yaml` | **On the go:** your public broker over TLS, SNTP, keeps running offline |
| `yellow-nabu.yaml` | Optional: Yellow looks at the voice assistant's LED ring and knows where it stands |
| `yellow-screenshot.yaml` | Optional, only while taking pictures: pixel-exact screenshots over MQTT (`tools/screenshot.py`) |
| `board-cyd.yaml` | Hardware package for the ESP32-2432S028R |

## Language

All on-screen texts live in `esphome/lang/<language>.h`, entity names and the few YAML texts in
`esphome/lang/<language>.yaml`. Set the substitution `language: en` (default) or `de` in your device file.
Home Assistant builds the entity IDs from the entity names, so **switching the language gives new entity IDs**;
adjust your automations or rename the entity IDs afterwards. Another language = one more pair of files with the
same keys.

## MQTT topics (`yellow/<device>/…`)

- `command` – JSON command, e.g. `{"action": "message", "text": "Hello!", "wait": true}`.
  Actions and their fields:
  `message` (text, wait, seconds), `love_letter` (text), `expression` (expression, seconds), `prop` (prop, seconds),
  `effect` (name, seconds), `gesture` (gesture), `look` (direction), `accessory` (name), `jewelry` (on),
  `question` (question, options `"Yes|No"`), `game` (game: reaction, ttt, simon), `timer` (seconds, name),
  `sleep` (on), `away` (on, card), `matrix` (seconds), `nabu_direction` (direction), `telemetry`;
  for testing: `decor_test` (occasion), `weather` (condition), `boredom_test` (minutes), `network_test` (level),
  `wifi_dialog`; `image` (seconds) only with `yellow-screenshot.yaml`.
  - expressions: normal, happy, surprised, dizzy, tired, annoyed, sad, asleep, yawn
  - gestures: hop, wobble, wiggle, nod, shake_head, wave, startle, oracle, wilt, dance, hearts, help, blunder,
    fist_bump, light_scare, special, nabu_visits, visit_nabu; per character ink, knot, juggle (Okti), mouse_dive,
    chase_tail, tail_fan (fox), glitch, propeller, visor (robot)
  - props: sunglasses, umbrella, scarf, fan, watering_can, joint, laundry_basket, trash_can, pillow, none;
    effects: sweat, tear, blush, sparkle, none; accessories: hat, crown, star, flower, none;
    directions: left, right, up, down, center
- `data/<name>` (retained) – occasion, alarm, appointment, notice, weather, weather_condition, outdoor_temp,
  room_temp, room_humidity, sayings; events without retain: doorbell, nabu_ring.
  Occasions: easter, new_year, valentine, carnival, advent (`advent|2`), st_nicholas, st_martin, birthday
  (`birthday|Name`), mothers_day, fathers_day, harvest, oktoberfest, beer, clock_change, full_moon, perseids,
  solstice, friday13, starwars, pi, towel, pirate, 420, future, hobbit, coffee, programmer, cat, blackfriday, ufo,
  emoji, spring, summer, autumn, winter
- `event` – JSON from the device, `type`: read (text), answer (text), game (game, value), petted (text), timer_off
- `telemetry` (retained), `status` online/offline (retained, last will)

## Install

1. Clone this repo into your ESPHome config folder (`…/esphome/cheap-yellow-companion`).
2. Copy `esphome/example-home.yaml` next to it (into `…/esphome/`), set the substitutions and secrets, flash.
3. In Home Assistant: an MQTT broker (e.g. the Mosquitto add-on) with a user for Yellow, then the blueprints from
   [docs/homeassistant.md](docs/homeassistant.md).

## Home Assistant

Setup, entities, language and all blueprints with import buttons: **[docs/homeassistant.md](docs/homeassistant.md)**.
Blueprints today: send data (weather, alarm, appointment, climate), doorbell, voice assistant LED ring, read receipt,
message script. Anything else: publish a JSON command to `yellow/<device>/command` (see topics above).

## Status

0.2.0 – runs every day on one bedside display. Generalising the Home Assistant side (blueprints) is in progress.
Ideas and plans live in the issues; changes in [CHANGELOG.md](CHANGELOG.md).

## License

MIT
