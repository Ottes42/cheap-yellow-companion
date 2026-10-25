# Yellow and Home Assistant

🇩🇪 [Deutsche Version](../docs_de/homeassistant.md)

Yellow runs without Home Assistant, but with it he knows the weather, your alarm and appointments, reacts to the
doorbell and the voice assistant, and you can send him messages. This page covers the setup, the entities, the
language and every blueprint. The MQTT command reference is in the [README](../README.md#mqtt-topics-yellowdevice);
the voice assistant has its own page: [nabu.md](nabu.md).

## Setup

1. **MQTT broker:** the Mosquitto add-on (or any broker) and the MQTT integration in Home Assistant.
2. **One MQTT user per Yellow** (`mqtt_user`/`mqtt_password` in the device file). If your broker supports ACLs, allow
   that user only `yellow/<device>/#`. The substitution `device` is the topic name (`yellow/<device>/…`) and the
   `device` input of every blueprint.
3. **Flash** your device file (copy of `esphome/example-home.yaml`) with the profile `yellow-home.yaml`.
4. **Add the device** in Home Assistant through the ESPHome integration (it is usually discovered; the API key is in
   your secrets). This gives you the entities below, OTA updates and logs. With the profile `yellow-mobile.yaml` there
   is no ESPHome API: no entities, only MQTT, and the blueprints still work.
5. **Import the blueprints** you want (buttons below) and create one automation or script per blueprint.

## Entities

Names from `esphome/lang/en.yaml` (German in `lang/de.yaml`). Home Assistant makes the entity IDs from the device
name, the entity name and sometimes the area, so look them up on the device page instead of guessing.

| Entity | Type | What for |
|---|---|---|
| Yellow mood | number 0–100 | 0 yellow and happy … 100 red and grumpy (mouth, brows, Okti's colour) |
| Yellow tension | number 0–100 | 0 relaxed … 100 very nervous (pace, blinking, sweat, flinching) |
| Yellow energy | number 0–100 | 0 dead tired … 100 wide awake (breathing, yawning, boredom); good to feed by time of day |
| Yellow sleeps | switch | puts him to bed; a tap wakes him and turns it off |
| Wake mode | switch | lets Yellow be visible in the dark (e.g. for a sunrise alarm) |
| Off at night | switch | display off in the dark (a tap still shows a dim clock) |
| Auto brightness | switch | brightness from the light sensor; turn off to set the display light yourself |
| View | select | Auto / Yellow / Clock |
| Decoration | select | Auto (by date and occasion) / Off / Halloween / Christmas |
| Character | select | Okti / Robot / Fox |
| Yellow play post, Yellow clear post | buttons | show the next waiting message, or drop all |
| Yellow reaction test, Yellow tic-tac-toe, Yellow Simon Says | buttons | start a game (it is played on the touch screen) |
| Yellow cancel question | button | removes an open question without an answer event |
| Yellow messages waiting | sensor | number of messages in the envelope |
| Yellow today | text sensor | today's counts `taps|pets|games|fist`, reset at midnight |
| Yellow visible | binary sensor | Yellow is on screen (not dark, not clock view) – commands only show then |
| Display, Status LED | lights | backlight and the RGB LED on the back |
| Ambient light | sensor | volts at the light sensor (higher = darker) |
| Wi‑Fi, Free memory, Largest free block | diagnostics | connected network and RAM (watch these when adding effects) |

## Language

Entity names follow the substitution `language: en|de` of the device. **Switching the language gives new entity
IDs** (Home Assistant derives them from the names): adjust automations and dashboards, or rename the new entity IDs
to the old ones on the entity settings page. Options of the selects are translated too (Clock/Uhr, Robot/Roboter …).

The blueprints talk MQTT and use entity selectors only for your own entities (weather, doorbell …), so they keep
working. Blueprints that send words to the screen have their own `language` input.

## Blueprints

### Send data

[![Import](https://my.home-assistant.io/badges/blueprint_import.svg)](https://my.home-assistant.io/redirect/blueprint_import/?blueprint_url=https%3A%2F%2Fgithub.com%2FOttes42%2Fcheap-yellow-companion%2Fblob%2Fmain%2Fhomeassistant%2Fblueprints%2Fautomation%2Fyellow_data.yaml)
`automation/yellow_data.yaml`

Sends what the clock view and the weather show to `yellow/<device>/data/…` (retained): weather text and condition
(today, from 14:00 tomorrow), next alarm within 20 hours, next appointment of the day, a notice line, outdoor and room
temperature, room humidity. Runs every 10 minutes, at Home Assistant start and when the device comes online.

| Input | |
|---|---|
| `device` | the device's `device` substitution |
| `language` | `en` or `de`: words in the rows ("Today", "Alarm") |
| `weather` | weather entity (daily forecast) |
| `alarm` | optional timestamp sensor, e.g. the phone's next alarm |
| `calendar` | optional calendar for today's next appointment |
| `notice` | optional entity shown as an extra row (e.g. waste collection); `-` or empty hides it |
| `outdoor_temp`, `room_temp`, `room_humidity` | optional sensors |

```yaml
automation:
  - alias: Yellow – data
    use_blueprint:
      path: Ottes42/yellow_data.yaml
      input:
        device: yellow
        language: en
        weather: weather.home
        alarm: sensor.phone_next_alarm
```

### Doorbell

[![Import](https://my.home-assistant.io/badges/blueprint_import.svg)](https://my.home-assistant.io/redirect/blueprint_import/?blueprint_url=https%3A%2F%2Fgithub.com%2FOttes42%2Fcheap-yellow-companion%2Fblob%2Fmain%2Fhomeassistant%2Fblueprints%2Fautomation%2Fyellow_doorbell.yaml)
`automation/yellow_doorbell.yaml`

When the doorbell binary sensor turns on, Yellow shows the doorbell panel for 60 s and reacts (only when the room is
lit). Inputs: `device`, `doorbell` (binary sensor).

### Voice assistant LED ring

[![Import](https://my.home-assistant.io/badges/blueprint_import.svg)](https://my.home-assistant.io/redirect/blueprint_import/?blueprint_url=https%3A%2F%2Fgithub.com%2FOttes42%2Fcheap-yellow-companion%2Fblob%2Fmain%2Fhomeassistant%2Fblueprints%2Fautomation%2Fyellow_voice_assistant_ring.yaml)
`automation/yellow_voice_assistant_ring.yaml`

Yellow looks over, jealous or curious, whenever the assistant's LED ring lights up. Needs the package
`yellow-nabu.yaml` on the device. Inputs: `device`, `ring` (light entity of the ring). More in [nabu.md](nabu.md).

### Read receipt

[![Import](https://my.home-assistant.io/badges/blueprint_import.svg)](https://my.home-assistant.io/redirect/blueprint_import/?blueprint_url=https%3A%2F%2Fgithub.com%2FOttes42%2Fcheap-yellow-companion%2Fblob%2Fmain%2Fhomeassistant%2Fblueprints%2Fautomation%2Fyellow_post_read.yaml)
`automation/yellow_post_read.yaml`

Someone tapped Yellow and read a waiting message or love letter: your actions run. They can use the variables
`text` (the message), `love` (true for a love letter) and `device`. Inputs: `device`, `actions`.

```yaml
automation:
  - alias: Yellow – read receipt
    use_blueprint:
      path: Ottes42/yellow_post_read.yaml
      input:
        device: yellow
        actions:
          - action: notify.mobile_app_phone
            data:
              message: "Read: {{ text }}"
```

### Message (script)

[![Import](https://my.home-assistant.io/badges/blueprint_import.svg)](https://my.home-assistant.io/redirect/blueprint_import/?blueprint_url=https%3A%2F%2Fgithub.com%2FOttes42%2Fcheap-yellow-companion%2Fblob%2Fmain%2Fhomeassistant%2Fblueprints%2Fscript%2Fyellow_message.yaml)
`script/yellow_message.yaml`

A script with the fields `text`, `wait` (deliver as post: envelope until someone taps) and `love_letter` (pink
envelope, hearts). Long texts are split over several signs. Expose it to Assist and say "Tell Yellow …"; the
description of the script tells the language model when to use it. Input: `device`.

```yaml
action: script.yellow_message
data:
  text: Dinner is ready!
  wait: true
```

## Anything else

Every other command (gestures, props, questions, games, timers, travelling …) is a JSON message to
`yellow/<device>/command`:

```yaml
action: mqtt.publish
data:
  topic: yellow/yellow/command
  payload: '{"action": "gesture", "gesture": "wave"}'
```

Events come back on `yellow/<device>/event` (`type`: read, answer, game, petted, timer_off) – use an MQTT trigger
with `trigger.payload_json`. All actions, fields and values: [README, MQTT topics](../README.md#mqtt-topics-yellowdevice).

## Tips

- Give every Yellow its own MQTT user; one device per topic name.
- Expose only the scripts you want to say out loud to Assist; the voice assistant side is in [nabu.md](nabu.md).
- Commands only show while Yellow is visible (entity *Yellow visible*); messages with `wait` survive the dark and the
  clock view and are delivered later.
