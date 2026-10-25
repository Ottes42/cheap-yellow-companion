#!/usr/bin/env python3
"""Builds the fonts from the font: block of esphome/yellow-base.yaml as C files with lv_font_conv, so the card has
the same glyphs as the device. Usage: fonts.py <out dir> <cache dir> <lv_font_conv>"""
import hashlib
import pathlib
import re
import subprocess
import sys
import urllib.request

import freetype  # like ESPHome: only characters the font actually has
import yaml

try:
    import esphome_glyphsets as glyphsets  # same glyph sets as ESPHome (pip install esphome-glyphsets)
except ImportError:
    sys.exit("esphome_glyphsets missing: pip install esphome-glyphsets")

BASE = pathlib.Path(__file__).resolve().parent.parent / "esphome" / "yellow-base.yaml"


class Loader(yaml.SafeLoader):
    pass


Loader.add_multi_constructor("!", lambda loader, tag, node: None)  # !lambda, !secret, !include … don't matter here


def fetch(url: str, cache: pathlib.Path) -> pathlib.Path:
    target = cache / (hashlib.sha1(url.encode()).hexdigest()[:16] + ".ttf")
    if not target.exists():
        req = urllib.request.Request(url, headers={"User-Agent": "Mozilla/5.0"} if "googleapis" in url else {})
        target.write_bytes(urllib.request.urlopen(req, timeout=60).read())
    return target


def ttf(source, cache: pathlib.Path) -> pathlib.Path:
    if isinstance(source, dict):
        return fetch(source["url"], cache)
    m = re.match(r"gfonts://([^@]+)(?:@(\d+))?", source)
    if not m:
        sys.exit(f"font source not supported: {source}")
    family, weight = m.group(1), m.group(2) or "400"
    css = fetch(f"https://fonts.googleapis.com/css2?family={family.replace(' ', '+')}:wght@{weight}", cache)
    return fetch(re.search(r"url\((https://[^)]+\.ttf)\)", css.read_text()).group(1), cache)


def available(font: pathlib.Path, codes) -> list[int]:
    face = freetype.Face(str(font))
    return sorted(c for c in codes if face.get_char_index(c))


def chars(glyphs) -> list[int]:
    if isinstance(glyphs, str):
        glyphs = [glyphs]
    return sorted({ord(c) for g in glyphs or [] for c in g})


def main():
    out, cache, conv = pathlib.Path(sys.argv[1]), pathlib.Path(sys.argv[2]), sys.argv[3]
    out.mkdir(parents=True, exist_ok=True)
    cache.mkdir(parents=True, exist_ok=True)
    names = []
    for f in yaml.load(BASE.read_text(), Loader)["font"]:
        codes = set(chars(f.get("glyphs")))
        for gs in f.get("glyphsets", []):
            codes |= set(glyphsets.unicodes_per_glyphset(gs))
        extras = [(ttf(e["file"], cache), chars(e["glyphs"])) for e in f.get("extras", [])]
        for _, ex in extras:  # extras override the main font (as in ESPHome)
            codes -= set(ex)
        name = f["id"]
        main_font = ttf(f["file"], cache)
        cmd = [conv, "--format", "lvgl", "--no-compress", "--bpp", str(f.get("bpp", 1)), "--size", str(f["size"]),
               "--lv-include", "lvgl.h", "-o", str(out / f"{name}.c"),
               "--font", str(main_font), "-r", ",".join(hex(c) for c in available(main_font, codes))]
        for font, ex in extras:
            cmd += ["--font", str(font), "-r", ",".join(hex(c) for c in ex)]
        subprocess.run(cmd, check=True)
        names.append(name)
    (out / "fonts.h").write_text("#pragma once\n#include \"lvgl.h\"\n" +
                                 "".join(f"LV_FONT_DECLARE({n})\n" for n in names))
    print("fonts:", ", ".join(names))


if __name__ == "__main__":
    main()
