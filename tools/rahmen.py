#!/usr/bin/env python3
"""Setzt Screenshots (PNG) oder GIFs von tools/bild.py in ein gezeichnetes Cheap Yellow Display (Querformat):
gelbe Platine mit Befestigungslöchern, schwarzer Glasrand, RGB-LED und Lichtsensor am Rand.

  rahmen.py eingabe.png|gif ausgabe.png|gif [--breite 520]
"""
import argparse
from PIL import Image, ImageDraw, ImageFilter, ImageSequence

ap = argparse.ArgumentParser()
ap.add_argument('ein')
ap.add_argument('aus')
ap.add_argument('--breite', type=int, default=520, help='Breite des Bildschirms im Ergebnis (Platine wird größer)')
a = ap.parse_args()

SW = a.breite
SH = SW * 3 // 4
k = SW / 320                      # Maßstab: 1 Display-Pixel
glas = int(10 * k)                # schwarzer Rand ums aktive Feld
rand_l, rand_r, rand_o, rand_u = int(16 * k), int(50 * k), int(14 * k), int(14 * k)  # Platine sichtbar (rechts breiter)
schatten = int(10 * k)
PW = SW + 2 * glas + rand_l + rand_r
PH = SH + 2 * glas + rand_o + rand_u
GW, GH = PW + 2 * schatten, PH + 2 * schatten
ox, oy = schatten, schatten // 2  # Platine im Gesamtbild


def platine():
    bild = Image.new('RGBA', (GW, GH), (0, 0, 0, 0))
    s = Image.new('RGBA', (GW, GH), (0, 0, 0, 0))
    ImageDraw.Draw(s).rounded_rectangle([ox + 4, oy + 8, ox + PW + 4, oy + PH + 8], int(8 * k), fill=(0, 0, 0, 110))
    bild.alpha_composite(s.filter(ImageFilter.GaussianBlur(schatten // 2)))
    d = ImageDraw.Draw(bild)
    d.rounded_rectangle([ox, oy, ox + PW, oy + PH], int(8 * k), fill=(246, 196, 32), outline=(214, 160, 0), width=max(2, int(k)))
    for cx in (ox + int(9 * k), ox + PW - int(9 * k)):  # Befestigungslöcher mit Lötring
        for cy in (oy + int(9 * k), oy + PH - int(9 * k)):
            r = int(5 * k)
            d.ellipse([cx - r, cy - r, cx + r, cy + r], fill=(222, 222, 210))
            d.ellipse([cx - r * 0.55, cy - r * 0.55, cx + r * 0.55, cy + r * 0.55], fill=(40, 34, 20))
    gx, gy = ox + rand_l, oy + rand_o
    d.rounded_rectangle([gx, gy, gx + SW + 2 * glas, gy + SH + 2 * glas], int(4 * k), fill=(12, 12, 14))
    d.line([gx + 3, gy + 3, gx + SW * 0.4, gy + 3], fill=(60, 60, 66), width=max(1, int(k)))  # Glanzkante
    # rechts: RGB-LED (weiß gefasst) und Lichtsensor (LDR)
    lx = ox + PW - rand_r // 2
    r = int(4 * k)
    d.rounded_rectangle([lx - r, oy + PH // 2 - int(30 * k) - r, lx + r, oy + PH // 2 - int(30 * k) + r], 2, fill=(240, 240, 236))
    d.ellipse([lx - r * 0.5, oy + PH // 2 - int(30 * k) - r * 0.5, lx + r * 0.5, oy + PH // 2 - int(30 * k) + r * 0.5], fill=(255, 120, 90))
    d.ellipse([lx - r, oy + PH // 2 + int(20 * k) - r, lx + r, oy + PH // 2 + int(20 * k) + r], fill=(196, 70, 40))
    d.line([lx - r * 0.6, oy + PH // 2 + int(20 * k), lx + r * 0.6, oy + PH // 2 + int(20 * k)], fill=(250, 210, 120), width=max(1, int(k / 2)))
    return bild, (gx + glas, gy + glas)


grund, pos = platine()


def einsetzen(frame):
    b = grund.copy()
    b.paste(frame.convert('RGB').resize((SW, SH), Image.NEAREST), pos)
    return b


ein = Image.open(a.ein)
if a.aus.lower().endswith('.gif'):
    frames, dauer = [], []
    for f in ImageSequence.Iterator(ein):
        unten = f.convert('RGB').crop((0, f.height - 1, f.width, f.height))
        if unten.getextrema() == ((0, 0), (0, 0), (0, 0)):  # noch nicht fertig gezeichnetes Anfangsbild
            continue
        frames.append(einsetzen(f.convert('RGBA')).convert('RGB'))
        dauer.append(f.info.get('duration', 100))
    # gemeinsame Palette, damit das Gelb nicht flackert
    pal = frames[0].quantize(colors=255, method=Image.Quantize.MEDIANCUT)
    q = [f.quantize(palette=pal, dither=Image.Dither.NONE) for f in frames]
    q[0].save(a.aus, save_all=True, append_images=q[1:], duration=dauer, loop=0, optimize=True, disposal=1)
else:
    einsetzen(ein.convert('RGBA')).save(a.aus)
print(a.aus)
