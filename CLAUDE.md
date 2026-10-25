# Notes for working on this repo (also for AI assistants)

Layout: `esphome/` firmware (packages + `yellow*.h`), `esphome/lang/` (texts per language), `homeassistant/blueprints/`
(automation + script), `docs/` (English: gallery, `nabu.md`, media), `docs_de/` (German copies), `tools/` (screenshots,
board frame), `FEATURES.md` (complete feature list, read it first to see what exists). Ideas and plans are GitHub issues;
the owner's private installation (family automations, servers, devices) is not part of this repo.

- Owner writes German; everything in the repo is English: code, identifiers, comments, MQTT interface, docs
  (`README.md`, `docs/`). German copies: `README.de.md`, `docs_de/` (incl. `docs_de/FEATURES.md`) (keep them in sync when a doc changes).
- Every on-screen text lives only in `esphome/lang/<language>.h` (`txt::NAME`) and every entity name or YAML text in
  `esphome/lang/<language>.yaml` (`${t_name}`); a new text needs an entry in **each** language file. Never compare
  select options by text (they are translated), use the index. The MQTT interface is not translated.
- ESPHome + LVGL 9 on an ESP32 **without PSRAM**. RAM is the hard limit: about 95–120 KB free, the largest free block
  ~49–64 KB. Transformed or semi-transparent large objects need an LVGL layer of width × height × 4 bytes in one block;
  if that does not fit, LVGL waits forever and the task watchdog reboots (see `sign_blunder`). Check
  `heap_caps_get_largest_free_block` before adding such effects, and watch the telemetry (`free`, `block`).
- `yellow.h` holds the class `Companion` and the tick loop; topics live in `yellow_*.h` as out-of-line `Companion::`
  methods. Nothing touches LVGL before `build()` (`ready()`), `on_value` handlers can fire during setup.
- The device file sets `yellow_path` if the repo is not cloned as `cheap-yellow-companion/` next to it
  (ESPHome resolves `includes:` relative to the device file).
- Two device files with the same node name (e.g. a test build) share one build folder: clean the build files before
  switching between them, or the old firmware gets flashed again.
- After flashing, wait until the boot is marked successful (about a minute) before stress-testing; a crash in the first
  minute rolls the OTA back.
- Touch calibration is per board (`board-cyd.yaml`, `transform`/`calibration`); check raw values with log level DEBUG.
- Every new feature or idea: decide whether it is generic (here) or family-specific (stays in the private config),
  note it in the issue, add a CHANGELOG line.
- Home Assistant side: ship and update blueprints; a new or changed blueprint gets its section in
  `docs/homeassistant.md` and `docs_de/homeassistant.md` (import button, inputs, example). The voice assistant part
  lives in `docs/nabu.md`.

## Issues workflow

- Undecided ideas go to `IDEAS.md`; a decided idea becomes an issue; done work goes to the CHANGELOG (and the feature
  list, issue #31).
- The owner drops raw ideas as issues (German, keywords are fine). At the start of a work session, or when asked,
  fetch new issues and comments (`gh issue list`, `gh issue view N --comments`), rewrite title and body into a clear
  English issue, keep the original as a quote at the end, and label it (`enhancement`/`documentation`, plus whether it
  is generic or a family example). Family-only ideas move to the private config, not this repo.
- **Plan first, for every issue:** before any change, write a plan (what changes where, risks such as RAM, and how
  to verify each point), review it (ideally by a second, independent agent or against the code), fix the plan, then
  post the reviewed plan as an issue comment ("Plan (reviewed)") and only then start. When done, use the plan as the
  test checklist: tick every point with how it was verified (screenshot, log, compile) in the closing comment.
- Before working on an issue, read all comments; ask questions as comments. Reference the issue in commits
  (`Fixes #N`), close it with a short comment on what was done and what was left out, add a CHANGELOG line.

## Screenshots and GIFs

Include `esphome/yellow-screenshot.yaml` only temporarily, then run `tools/screenshot.py` (pixel-exact frames over MQTT);
crop/arrange with ImageMagick. Remove the package again afterwards.
