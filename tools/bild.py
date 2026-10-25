#!/usr/bin/env python3
"""Screenshots und GIFs direkt von Yellow (pixelgenau, ohne Kamera).

Schickt den Befehl `bild` per MQTT; Yellow sendet jeden neu gezeichneten Streifen (RGB565) nach
yellow/<gerät>/bild/x/y/breite/höhe. Dieses Programm setzt die Streifen zu einem 320×240-Bild zusammen.

  bild.py --broker 192.168.1.10 --user U --password P --geraet yellow ziel.png            # ein Bild
  bild.py … --sekunden 6 --fps 8 ziel.gif                                                    # Bildfolge als GIF
  bild.py … --befehl '{"aktion":"geste","geste":"winken"}' --sekunden 5 ziel.gif            # Geste aufnehmen

Optional --skalierung 2 (größer, scharfe Pixel). Braucht paho-mqtt und Pillow.
"""
import argparse, json, threading, time
import paho.mqtt.client as mqtt
from PIL import Image

ap = argparse.ArgumentParser()
ap.add_argument('ziel')
ap.add_argument('--broker', required=True)
ap.add_argument('--port', type=int, default=1883)
ap.add_argument('--user')
ap.add_argument('--password')
ap.add_argument('--geraet', default='yellow')
ap.add_argument('--sekunden', type=int, default=0, help='0 = ein Bild, sonst Bildfolge')
ap.add_argument('--fps', type=float, default=8)
ap.add_argument('--befehl', help='JSON-Befehl, der kurz nach Aufnahmestart geschickt wird')
ap.add_argument('--skalierung', type=int, default=1)
ap.add_argument('--tausch', action='store_true', help='Bytes je Pixel tauschen (falls die Farben falsch sind)')
a = ap.parse_args()

W, H = 320, 240
bild = Image.new('RGB', (W, H))
sperre = threading.Lock()
zuletzt = [0.0]


def rgb565(daten, tausch):
    out = bytearray(len(daten) // 2 * 3)
    for i in range(0, len(daten) - 1, 2):
        v = (daten[i] << 8 | daten[i + 1]) if not tausch else (daten[i + 1] << 8 | daten[i])
        j = i // 2 * 3
        out[j] = (v >> 11 & 0x1F) * 255 // 31
        out[j + 1] = (v >> 5 & 0x3F) * 255 // 63
        out[j + 2] = (v & 0x1F) * 255 // 31
    return bytes(out)


def bei_nachricht(_c, _u, m):
    x, y, b, h = map(int, m.topic.split('/')[-4:])
    if len(m.payload) != b * h * 2:
        return
    streifen = Image.frombytes('RGB', (b, h), rgb565(m.payload, a.tausch))
    with sperre:
        bild.paste(streifen, (x, y))
        zuletzt[0] = time.time()


c = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, client_id=f'yellow-bild-{int(time.time())}')
if a.user:
    c.username_pw_set(a.user, a.password)
c.on_message = bei_nachricht
c.connect(a.broker, a.port)
c.subscribe(f'yellow/{a.geraet}/bild/#')
c.loop_start()
time.sleep(0.5)
befehl = f'yellow/{a.geraet}/befehl'
c.publish(befehl, json.dumps({'aktion': 'bild', 'sekunden': a.sekunden}))
time.sleep(1.5)  # erst ein vollständiges Bild abwarten
if a.befehl:
    c.publish(befehl, a.befehl)

bilder = []
start = time.time()
if a.sekunden:
    while time.time() - start < a.sekunden - 1.5:
        time.sleep(1 / a.fps)
        with sperre:
            if zuletzt[0]:
                bilder.append(bild.copy())
else:
    while not zuletzt[0] or time.time() - start < 4.5:  # Gerät sendet 4 s lang
        if time.time() - start > 15:
            raise SystemExit('keine Bilddaten bekommen')
        time.sleep(0.2)
c.loop_stop()

groesse = (W * a.skalierung, H * a.skalierung)
if a.sekunden:
    bilder = [b.resize(groesse, Image.NEAREST) for b in bilder]
    bilder[0].save(a.ziel, save_all=True, append_images=bilder[1:], duration=int(1000 / a.fps), loop=0, optimize=True)
    print(f'{a.ziel}: {len(bilder)} Bilder')
else:
    bild.resize(groesse, Image.NEAREST).save(a.ziel)
    print(a.ziel)
