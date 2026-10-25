#!/usr/bin/env python3
"""Puts screenshots (PNG) or GIFs from tools/screenshot.py into a drawn Cheap Yellow Display (landscape):
yellow board with mounting holes, black glass edge, RGB LED and light sensor at the edge.

  frame.py in.png|gif out.png|gif [--width 520]
"""
import argparse
from PIL import Image, ImageDraw, ImageFilter, ImageSequence

ap = argparse.ArgumentParser()
ap.add_argument('src')
ap.add_argument('dst')
ap.add_argument('--width', type=int, default=520, help='width of the screen in the result (the board is bigger)')
a = ap.parse_args()

SW = a.width
SH = SW * 3 // 4
k = SW / 320                      # scale: 1 display pixel
glass = int(10 * k)               # black edge around the active area
edge_l, edge_r, edge_t, edge_b = int(16 * k), int(50 * k), int(14 * k), int(14 * k)  # visible board (wider on the right)
shadow = int(10 * k)
PW = SW + 2 * glass + edge_l + edge_r
PH = SH + 2 * glass + edge_t + edge_b
GW, GH = PW + 2 * shadow, PH + 2 * shadow
ox, oy = shadow, shadow // 2  # board in the whole picture


def board():
    image = Image.new('RGBA', (GW, GH), (0, 0, 0, 0))
    s = Image.new('RGBA', (GW, GH), (0, 0, 0, 0))
    ImageDraw.Draw(s).rounded_rectangle([ox + 4, oy + 8, ox + PW + 4, oy + PH + 8], int(8 * k), fill=(0, 0, 0, 110))
    image.alpha_composite(s.filter(ImageFilter.GaussianBlur(shadow // 2)))
    d = ImageDraw.Draw(image)
    d.rounded_rectangle([ox, oy, ox + PW, oy + PH], int(8 * k), fill=(246, 196, 32), outline=(214, 160, 0), width=max(2, int(k)))
    for cx in (ox + int(9 * k), ox + PW - int(9 * k)):  # mounting holes with solder ring
        for cy in (oy + int(9 * k), oy + PH - int(9 * k)):
            r = int(5 * k)
            d.ellipse([cx - r, cy - r, cx + r, cy + r], fill=(222, 222, 210))
            d.ellipse([cx - r * 0.55, cy - r * 0.55, cx + r * 0.55, cy + r * 0.55], fill=(40, 34, 20))
    gx, gy = ox + edge_l, oy + edge_t
    d.rounded_rectangle([gx, gy, gx + SW + 2 * glass, gy + SH + 2 * glass], int(4 * k), fill=(12, 12, 14))
    d.line([gx + 3, gy + 3, gx + SW * 0.4, gy + 3], fill=(60, 60, 66), width=max(1, int(k)))  # shiny edge
    # right: RGB LED (white frame) and light sensor (LDR)
    lx = ox + PW - edge_r // 2
    r = int(4 * k)
    d.rounded_rectangle([lx - r, oy + PH // 2 - int(30 * k) - r, lx + r, oy + PH // 2 - int(30 * k) + r], 2, fill=(240, 240, 236))
    d.ellipse([lx - r * 0.5, oy + PH // 2 - int(30 * k) - r * 0.5, lx + r * 0.5, oy + PH // 2 - int(30 * k) + r * 0.5], fill=(255, 120, 90))
    d.ellipse([lx - r, oy + PH // 2 + int(20 * k) - r, lx + r, oy + PH // 2 + int(20 * k) + r], fill=(196, 70, 40))
    d.line([lx - r * 0.6, oy + PH // 2 + int(20 * k), lx + r * 0.6, oy + PH // 2 + int(20 * k)], fill=(250, 210, 120), width=max(1, int(k / 2)))
    return image, (gx + glass, gy + glass)


base, pos = board()


def insert(frame):
    b = base.copy()
    b.paste(frame.convert('RGB').resize((SW, SH), Image.NEAREST), pos)
    return b


src = Image.open(a.src)
if a.dst.lower().endswith('.gif'):
    frames, durations = [], []
    for f in ImageSequence.Iterator(src):
        bottom = f.convert('RGB').crop((0, f.height - 1, f.width, f.height))
        if bottom.getextrema() == ((0, 0), (0, 0), (0, 0)):  # first picture not completely drawn yet
            continue
        frames.append(insert(f.convert('RGBA')).convert('RGB'))
        durations.append(f.info.get('duration', 100))
    # shared palette so the yellow does not flicker
    pal = frames[0].quantize(colors=255, method=Image.Quantize.MEDIANCUT)
    q = [f.quantize(palette=pal, dither=Image.Dither.NONE) for f in frames]
    q[0].save(a.dst, save_all=True, append_images=q[1:], duration=durations, loop=0, optimize=True, disposal=1)
else:
    insert(src.convert('RGBA')).save(a.dst)
print(a.dst)
