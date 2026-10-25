# Changelog

## [Unreleased]

- Gallery page and README GIFs, recorded pixel-exact from the device
- Optional package `esphome/yellow-bild.yaml` (command `bild`) and `tools/bild.py` for screenshots and GIFs over MQTT,
  `tools/rahmen.py` puts them into a drawn CYD board

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
