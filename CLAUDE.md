# Notes for working on this repo (also for AI assistants)

Layout: `esphome/` firmware (packages + `yellow*.h`), `homeassistant/blueprints/` (automation + script),
`docs/` (gallery, `nabu.md`, media), `tools/` (screenshots, board frame). Ideas and plans are GitHub issues;
the owner's private installation (family automations, servers, devices) is not part of this repo.

- Owner writes German; code comments and on-screen texts are German, README/CHANGELOG English.
- ESPHome + LVGL 9 on an ESP32 **without PSRAM**. RAM is the hard limit: about 95–120 KB free, the largest free block
  ~49–64 KB. Transformed or semi-transparent large objects need an LVGL layer of width × height × 4 bytes in one block;
  if that does not fit, LVGL waits forever and the task watchdog reboots (see `schild_patzer`). Check
  `heap_caps_get_largest_free_block` before adding such effects, and watch the telemetry (`frei`, `block`).
- `yellow.h` holds the class `Figur` and the tick loop; topics live in `yellow_*.h` as out-of-line `Figur::` methods.
  Nothing touches LVGL before `aufbauen()` (`bereit()`), `on_value` handlers can fire during setup.
- The device file sets `yellow_pfad` if the repo is not cloned as `cheap-yellow-companion/` next to it
  (ESPHome resolves `includes:` relative to the device file).
- After flashing, wait until the boot is marked successful (about a minute) before stress-testing; a crash in the first
  minute rolls the OTA back.
- Touch calibration is per board (`board-cyd.yaml`, `transform`/`calibration`); check raw values with log level DEBUG.
- Every new feature or idea: decide whether it is generic (here) or family-specific (stays in the private config),
  note it in the issue, add a CHANGELOG line.
- Home Assistant side: ship and update blueprints, document setup tips; the voice assistant part lives in
  `docs/nabu.md`.

## Issues workflow

- The owner drops raw ideas as issues (German, keywords are fine). At the start of a work session, or when asked,
  fetch new issues and comments (`gh issue list`, `gh issue view N --comments`), rewrite title and body into a clear
  English issue, keep the original as a quote at the end, and label it (`enhancement`/`documentation`, plus whether it
  is generic or a family example). Family-only ideas move to the private config, not this repo.
- Before working on an issue, read all comments; ask questions as comments. Reference the issue in commits
  (`Fixes #N`), close it with a short comment on what was done and what was left out, add a CHANGELOG line.

## Screenshots and GIFs

Include `esphome/yellow-bild.yaml` only temporarily, then run `tools/bild.py` (pixel-exact frames over MQTT);
crop/arrange with ImageMagick. Remove the package again afterwards.
