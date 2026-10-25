#!/usr/bin/env python3
"""Screenshots and GIFs straight from Yellow (pixel-exact, no camera).

Sends the command `image` over MQTT; Yellow sends every newly drawn stripe (RGB565) to
yellow/<device>/image/x/y/width/height. This program puts the stripes together into a 320×240 picture.

  screenshot.py --broker 192.168.1.10 --user U --password P --device yellow out.png          # one picture
  screenshot.py … --seconds 6 --fps 8 out.gif                                                 # sequence as GIF
  screenshot.py … --command '{"action":"gesture","gesture":"wave"}' --seconds 5 out.gif      # record a gesture

Optional --scale 2 (bigger, sharp pixels). Needs paho-mqtt and Pillow.
"""
import argparse, json, threading, time
import paho.mqtt.client as mqtt
from PIL import Image

ap = argparse.ArgumentParser()
ap.add_argument('out')
ap.add_argument('--broker', required=True)
ap.add_argument('--port', type=int, default=1883)
ap.add_argument('--user')
ap.add_argument('--password')
ap.add_argument('--device', default='yellow')
ap.add_argument('--seconds', type=int, default=0, help='0 = one picture, otherwise a sequence')
ap.add_argument('--fps', type=float, default=8)
ap.add_argument('--command', help='JSON command sent shortly after the capture starts')
ap.add_argument('--scale', type=int, default=1)
ap.add_argument('--swap', action='store_true', help='swap the bytes of each pixel (if the colours are wrong)')
a = ap.parse_args()

W, H = 320, 240
image = Image.new('RGB', (W, H))
lock = threading.Lock()
last = [0.0]


def rgb565(data, swap):
    out = bytearray(len(data) // 2 * 3)
    for i in range(0, len(data) - 1, 2):
        v = (data[i] << 8 | data[i + 1]) if not swap else (data[i + 1] << 8 | data[i])
        j = i // 2 * 3
        out[j] = (v >> 11 & 0x1F) * 255 // 31
        out[j + 1] = (v >> 5 & 0x3F) * 255 // 63
        out[j + 2] = (v & 0x1F) * 255 // 31
    return bytes(out)


def on_message(_c, _u, m):
    x, y, w, h = map(int, m.topic.split('/')[-4:])
    if len(m.payload) != w * h * 2:
        return
    stripe = Image.frombytes('RGB', (w, h), rgb565(m.payload, a.swap))
    with lock:
        image.paste(stripe, (x, y))
        last[0] = time.time()


c = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, client_id=f'yellow-screenshot-{int(time.time())}')
if a.user:
    c.username_pw_set(a.user, a.password)
c.on_message = on_message
c.connect(a.broker, a.port)
c.subscribe(f'yellow/{a.device}/image/#')
c.loop_start()
time.sleep(0.5)
command_topic = f'yellow/{a.device}/command'
c.publish(command_topic, json.dumps({'action': 'image', 'seconds': a.seconds}))
time.sleep(1.5)  # wait for one complete picture first
if a.command:
    c.publish(command_topic, a.command)

frames = []
start = time.time()
if a.seconds:
    while time.time() - start < a.seconds - 1.5:
        time.sleep(1 / a.fps)
        with lock:
            if last[0]:
                frames.append(image.copy())
else:
    while not last[0] or time.time() - start < 4.5:  # the device sends for 4 s
        if time.time() - start > 15:
            raise SystemExit('no image data received')
        time.sleep(0.2)
c.loop_stop()

size = (W * a.scale, H * a.scale)
if a.seconds:
    frames = [f.resize(size, Image.NEAREST) for f in frames]
    frames[0].save(a.out, save_all=True, append_images=frames[1:], duration=int(1000 / a.fps), loop=0, optimize=True)
    print(f'{a.out}: {len(frames)} frames')
else:
    image.resize(size, Image.NEAREST).save(a.out)
    print(a.out)
