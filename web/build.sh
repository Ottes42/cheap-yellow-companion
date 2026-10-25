#!/usr/bin/env bash
# Builds Yellow as WebAssembly: dist/yellow-<language>.js + .wasm per language in esphome/lang/, plus yellow-card.js.
# Needs emcc (emsdk), node/npm (lv_font_conv), python3 with pyyaml, freetype-py and esphome-glyphsets
# (PYTHON=... to pick another interpreter). OPT=-O0 EXTRA=-g2 for a debug build.
set -euo pipefail
cd "$(dirname "$0")"
LVGL_VERSION=9.5.0 # same as ESPHome 2026.10 (components/lvgl, LVGL_VERSION)
B=build
OBJ=$B/obj${OPT:-}
mkdir -p "$OBJ" dist

[ -d $B/lvgl ] || git clone -q --depth 1 -b v$LVGL_VERSION https://github.com/lvgl/lvgl.git $B/lvgl
[ -x $B/node_modules/.bin/lv_font_conv ] || npm i -s --prefix $B lv_font_conv@1.5.3 >/dev/null
"${PYTHON:-python3}" fonts.py $B/fonts $B/cache $B/node_modules/.bin/lv_font_conv

# -Os like on the device; with -O2 (emcc 6.0.13) LVGL 9.5 hangs in lv_display_create
FLAGS=(${OPT:--Os} -DLV_CONF_INCLUDE_SIMPLE -DLV_LVGL_H_INCLUDE_SIMPLE -I. -I$B/lvgl -I$B/fonts)
# LVGL and fonts one by one (parallel, only what changed)
find $B/lvgl/src $B/fonts -name '*.c' | while read -r c; do
  o=$OBJ/$(echo "$c" | tr / _).o
  [ "$o" -nt "$c" ] && [ "$o" -nt lv_conf.h ] || echo "$c $o"
done | xargs -r -P "$(nproc)" -n 2 sh -c 'emcc '"${FLAGS[*]}"' -c "$0" -o "$1"'

for lang in ../esphome/lang/*.h; do
  l=$(basename "$lang" .h)
  em++ "${FLAGS[@]}" -std=gnu++20 -Ishim -I../esphome -DYELLOW_LANG_H="\"lang/$l.h\"" main.cpp "$OBJ"/*.o \
    -o dist/yellow-$l.js -sMODULARIZE -sEXPORT_ES6 -sENVIRONMENT=web,node -sALLOW_MEMORY_GROWTH ${EXTRA:-} \
    -sEXPORTED_FUNCTIONS=_start,_step,_frame,_pointer,_kind,_mood,_tension,_energy,_decor,_occasion,_weather,_set_time,_gesture,_message,_info_temp,_visible \
    -sEXPORTED_RUNTIME_METHODS=cwrap,HEAPU8
done
cp yellow-card.js dist/
ls -l dist
