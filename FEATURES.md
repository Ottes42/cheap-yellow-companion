# Features

Everything Yellow can do today, as a reference for the curious and a quick orientation for anyone (human or agent)
working on this repo. The [README](README.md) stays short; this list is complete. Changes per release:
[CHANGELOG.md](CHANGELOG.md).

> Command reference: [README, MQTT topics](README.md#mqtt-topics-yellowdevice). This page describes features in user
> terms and does not list command, field or gesture names.

Where it lives: `esphome/yellow.h` (class `Companion`, tick loop) and `esphome/yellow_*.h` (topics), `esphome/yellow-*.yaml`
(packages), `esphome/lang/` (all texts per language), `homeassistant/blueprints/` (Home Assistant side).

## Characters

| Feature | Description | Since |
|---|---|---|
| Three characters | Okti (reversible octopus, yellow and happy or red and grumpy, eight arms), a fox (brave, a bit cheeky, bushy tail) and a small robot (visor, antenna, pulsing heart light). Chosen on the device or fixed at build time. | 0.1.0 |
| Shapes only | Drawn from LVGL shapes, arcs and lines, no bitmaps; fits an ESP32 without PSRAM. | 0.1.0 |
| Languages | English or German, chosen at compile time (`language: en\|de`): every on-screen text, entity name and the boot greeting; date and decimal format follow. German was the only language until 0.1.0. | 0.2.0 |
| Alive when idle | Breathes, blinks, looks around, hops, yawns, wiggles, waves, walks off one side and comes back from the other; every few seconds a small idea. | 0.1.0 |
| Entrance | When he becomes visible he comes in from the side: the fox runs, Okti crawls, the robot rolls. | 0.1.0 |
| Startup | Random line on the boot screen, then a wave and a greeting (with the owner's and the figure's name if set). | 0.1.0 |
| Mood | Slider from happy and yellow to grumpy and red (mouth corners, brows, Okti's colour). Bad weather nudges it. | 0.1.0 |
| Tension | Slider from relaxed to very nervous: arm and tail speed, blinking, glances, breathing, sweat, startles. | 0.1.0 |
| Energy | Slider from dead tired to wide awake: breathing, yawning, half-closed eyes, how fast he gets bored. | 0.1.0 |
| Expressions | Normal, happy, surprised, dizzy, tired, annoyed, sad, asleep, yawning; for a few seconds or until the next touch. | 0.1.0 |
| Gestures | Hop, wiggle, wag/flail, nod, shake head, wave, startle, wilt (arms hang, sad), happy dance, rising hearts, magic-8-ball oracle (random answer sign). | 0.1.0 |
| Effects | Sweat drops, tear, blush, sparkle; also automatic (blush when petted, sweat during games and the last timer seconds, tear when sad). | 0.1.0 |
| Look direction | Look left, right, up, down or to the middle for a few seconds. | 0.1.0 |
| Props | Sunglasses, umbrella, scarf, hand fan (the fox fans himself with his tail instead), watering can, laundry basket, bin, pillow, a comic smoking roll-up; for some seconds or until removed. | 0.1.0 |
| Accessories | Hat, crown, star or flower on the head (an occasion hat wins). | 0.1.0 |
| Jewellery | Gold chain with a sparkling pendant; the fox also gets ear studs. Combines with hats. | 0.1.0 |
| Special moves | Okti: ink cloud, tied-up arms, juggling (sometimes drops a ball). Fox: mouse dive into a mound, tail fan, chasing his own tail. Robot: visor glitch with "restart", maintenance (visor opens, gears turn), propeller lift-off. Random in idle time or on command. | 0.1.0 |
| Clumsy signs | Sometimes the sign is upside down or has swapped letters and he corrects it, annoyed (upside down only if RAM allows). Okti sometimes drops his sign. | 0.1.0 |
| Personality | Own idle one-liners per character (Okti clumsy, robot over-precise, fox brave), the fox shows courage, the robot blinks his LED on every sign. | 0.1.0 |
| Daily counters | Status row with today's taps, pets, games and fist bumps; he comments on them ("tapped 10 times today. Enough."). Reset at midnight. | 0.1.0 |
| Boredom ladder | From the last touch: 5 min curious, 10 min bored (taps his arm), 20 min tired, 40 min dozing, 60 min sulking (red, looks away; a tap gets "Hmpf", petting twice makes up). | 0.1.0 |
| Sleep | Put him to bed: Okti in a bed under a blanket, the fox curled up, the robot in standby with a pulsing dot. A tap wakes him. | 0.1.0 |
| Light scare | When the room light goes on after darkness he startles, each character differently (the fox only blinks). | 0.1.0 |
| Travelling | Yellow is away: a suitcase with stickers and a note (your postcard text); he comes back with a souvenir prop. | 0.1.0 |
| Visits | Yellow walks out of the picture to visit the voice assistant and returns; or a mini voice assistant walks in and Yellow greets it. | 0.1.0 |
| Matrix rain | Green ones and zeros fall behind the figure for a while (the 13:37 nerd event; the trigger at 13:37 comes from your automation). | 0.1.0 |
| Status LED | The RGB LED on the back follows him: flashes on taps, pulses for waiting post, colours in Simon Says, red/orange while a timer rings. | 0.1.0 |

## Touch and screen

| Feature | Description | Since |
|---|---|---|
| Tap | He hops and is happy; a tap also picks up waiting post, answers a ringing timer or wakes him. | 0.1.0 |
| Follows your finger | Eyes follow the finger; the nearest arm (Okti), the robot's hand or the fox's paw reaches for it. | 0.1.0 |
| Pet (swipe) | Swiping pets him: happy, blushing, warm LED. Reports which sign was showing (his "favourite line"). | 0.1.0 |
| Hold | Long press shows the day's info as signs: alarm, appointment, weather, hint, room temperature. | 0.1.0 |
| 4× quick tap | He does not like it: dizzy and "please don't"; Okti sometimes vanishes in ink, the robot glitches. | 0.1.0 |
| Fist bump | Now and then (or on command) a fist comes forward with "Fist?"; tap it for a "Boom!" with sparks, otherwise he is disappointed after 8 s. | 0.1.0 |
| Help page | **?** bottom right shows how to use Yellow; any tap closes it, it closes itself after 20 s. | 0.1.0 |
| Game picker | Gamepad button bottom left opens the game menu. | 0.1.0 |
| Small clock | Clock top right on Yellow's screen; tap it for 15 s of the big clock view. | 0.1.0 |

## Games and questions

| Feature | Description | Since |
|---|---|---|
| Reaction test | "When I jump: tap!"; shows your time in ms, catches early taps and falling asleep. | 0.1.0 |
| Tic-tac-toe | You start (X); he blocks most of the time and cheats when losing ("didn't see that"). Gives up after 2 min without a move. | 0.1.0 |
| Simon Says | Four colour fields (and the LED) in a growing sequence; he counts your rounds. | 0.1.0 |
| Results | Each game reports its result (time, win/draw/loss, rounds, or aborted) as an event. Also startable from Home Assistant. | 0.1.0 |
| Questions | A sign with two touch answer buttons (default yes/no); waits until he is visible, gives up after 10 min, reports the answer. Can be cancelled from Home Assistant. | 0.1.0 |

## Post and messages

| Feature | Description | Since |
|---|---|---|
| Signs | Yellow is mute and holds up cardboard signs in handwriting; long texts are split over several signs. | 0.1.0 |
| Instant message | Shown right away for a few seconds (only while he is visible). | 0.1.0 |
| Post | A message waits in an envelope (up to 5); he waves, hops, "Post for you!", the LED pulses, until someone taps. Survives the clock view and darkness. | 0.1.0 |
| Love letters | Pink envelope with a heart seal; on opening he blushes and hearts rise. | 0.1.0 |
| Read receipt | Every opened message is reported back (love letters marked as such). | 0.1.0 |
| Post from Home Assistant | Play the next waiting message or clear all, as buttons. | 0.1.0 |
| Timer display | Shows a running timer (name and time), checks in now and then, counts the last 5 s with big digits, bounces while it rings; a tap stops it and reports back. | 0.1.0 |
| Remote one-liners | Your own list of idle lines over MQTT; stored on the device, so it survives a restart without network. Every third idle line comes from it. | 0.1.0 |

## Occasions, seasons and weather

| Feature | Description | Since |
|---|---|---|
| Halloween and Christmas scenes | Full backgrounds: moon, bats and pumpkins (pumpkin hat); tree, string lights, present and snow (Santa hat). Automatically mid-October to 1 November and 1–26 December, or fixed by a select. | 0.1.0 |
| Occasions | Hat symbol plus decoration around the edge for: Easter, New Year's Eve, Valentine's Day (pink Yellow), carnival, Advent (as many candles lit as the Advent Sunday), St Nicholas, St Martin, Star Wars Day, Pi Day, Towel Day, Talk Like a Pirate Day, 4/20, birthday, Oktoberfest, harvest festival, clock change, full moon, Perseids, Friday the 13th, Back to the Future day, Hobbit Day, coffee day, programmers' day, cat day, Mother's Day, Father's Day, solstice, Black Friday, UFO day, emoji day, beer day. Which one is today comes from your side (MQTT data). | 0.1.0 |
| Seasons | Spring, summer, autumn and winter decoration sets, also from the occasion data. | 0.1.0 |
| Decoration select | Auto, off, Halloween or Christmas; plus a test command to try any set. | 0.1.0 |
| Weather view | Behind the figure: sun, clouds, rain, snow, sleet, hail, fog, wind and lightning, animated. | 0.1.0 |
| Weather reactions | On a change he looks up and reacts per character ("Snow!", "Sun!", fox: "Pff. Thunder.", Okti gets blown about, the robot "measures" the wind). | 0.1.0 |

## Day, night and the clock view

| Feature | Description | Since |
|---|---|---|
| Night rule | In a dark room (light sensor) Yellow never shows or reacts; a tap shows a very dim clock for 30 s. Optional: screen fully off at night. | 0.1.0 |
| Clock view | Landscape clock with blinking colon, date, room and outdoor climate, next alarm, next appointment, weather and a hint line (empty lines hide). Day and night colours. | 0.1.0 |
| View select | Auto, always Yellow or always clock. | 0.1.0 |
| Wake-up mode | A switch that lets Yellow be visible in the dark (for a sunrise alarm automation). | 0.1.0 |
| Auto brightness | Backlight follows the light sensor and touches; can be switched off. | 0.1.0 |
| Doorbell | "It's ringing" panel with time for 60 s (only when the room is lit); each character reacts in its own way. | 0.1.0 |

## Network

| Feature | Description | Since |
|---|---|---|
| Network status | Red icon and "waiting for network: …" without Wi-Fi; orange cloud when the MQTT server is unreachable for 30 s, orange globe when the Wi-Fi has no internet. | 0.1.0 |
| Internet check | While MQTT is down, a configurable URL is polled every minute; any HTTP answer counts. | 0.1.0 |
| Wi-Fi dialog | Tap the red icon: list of networks (strongest first), password with on-screen keyboard, saved on the device. Closes after 5 min without touch. | 0.1.0 |
| Profiles | Home: local broker without TLS, time from Home Assistant, ESPHome API for OTA and logs. Away: own public broker over TLS, SNTP time, keeps running offline. | 0.1.0 |

## MQTT

| Feature | Description | Since |
|---|---|---|
| Commands | One JSON command topic for everything above (messages, love letters, expressions, props, effects, gestures, look direction, games, questions, timer, travelling, sleep, Matrix, weather, decoration, jewellery, accessories). | 0.1.0 |
| Data topics | Retained data for occasion, alarm, appointment, hint, weather text and condition, outdoor and room temperature, humidity, one-liner list; events without retain for doorbell and voice assistant ring. | 0.1.0 |
| Events | JSON events back: message read, answer, game result, petted (with the last sign), timer stopped. | 0.1.0 |
| Telemetry | Retained, on connect and every 30 min: firmware, uptime, reset reason, free RAM and largest block, Wi-Fi and signal, dark or not, current view and character, today's counters, plus the activity log since the last report (up to 30 entries). Also on command. | 0.1.0 |
| Status | Online/offline, retained, with last will. | 0.1.0 |
| Test commands | Pretend a boredom duration, a network state, a weather condition or an occasion; open the Wi-Fi dialog without touch. | 0.1.0 |

## Home Assistant

| Feature | Description | Since |
|---|---|---|
| Device entities | Over the ESPHome API: sliders for mood, tension and energy; switches for sleep, night off, wake-up mode and auto brightness; selects for view, decoration and character; buttons for play post, clear post, cancel question and the three games; sensors for waiting messages, today's counters, visible, Wi-Fi name, light level and free memory; display backlight and status LED as lights. | 0.1.0 |
| Blueprint: send data | Weather, next alarm, next appointment, a hint line, outdoor/room temperature and humidity. | 0.1.0 |
| Blueprint: doorbell | Sends the doorbell event. | 0.1.0 |
| Blueprint: voice assistant ring | Yellow looks over when the assistant's LED ring lights up. | 0.1.0 |
| Blueprint: read receipt | Your actions when a message or love letter was read. | 0.1.0 |
| Blueprint: message script | Message, post or love letter; expose it to Assist for "Tell Yellow …". | 0.1.0 |

## Voice assistant

Optional, details in [docs/nabu.md](docs/nabu.md).

| Feature | Description | Since |
|---|---|---|
| Looks at the LED ring | With `yellow-nabu.yaml`: when the assistant's ring lights up, he looks over, jealous ("Show-off.") or curious (at most every 20 s). | 0.1.0 |
| Knows where it stands | Left, right or another room; his glance goes there. | 0.1.0 |
| Timers | The assistant's timers show on Yellow; tapping him while it rings stops the timer. | 0.1.0 |
| Visits | Yellow visits the assistant, or a mini assistant visits Yellow (the assistant's display side is not in this repo). | 0.1.0 |

## Tools

| Feature | Description | Since |
|---|---|---|
| Screenshots and GIFs | Optional package `yellow-screenshot.yaml` plus `tools/screenshot.py`: pixel-exact frames over MQTT, no camera; `tools/frame.py` puts them into a drawn board. Include only temporarily. | 0.2.0 |
