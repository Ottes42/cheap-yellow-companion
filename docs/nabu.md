# Yellow and the voice assistant ("Nabu")

Yellow is mute; the voice assistant next to him is not. In the original setup that is a **Home Assistant Voice**
(Preview Edition) called *Nabu*, running a custom ESPHome firmware with a small OLED. Everything here is optional:
without it Yellow runs normally.

## What works today

| Feature | How |
|---|---|
| Yellow looks at Nabu's LED ring (jealous or curious) | `yellow-nabu.yaml` on Yellow; Home Assistant publishes `on` to `yellow/<device>/daten/nabu_ring` when the ring lights up |
| Yellow knows where Nabu stands (left, right, other room) | command `nabu_richtung` with `richtung`; Home Assistant can learn the room from the assistant's Wi‑Fi signal |
| Timers from the assistant show on Yellow, tapping Yellow while it rings stops the timer | command `timer` (`sekunden` > 0 running, 0 done, < 0 ringing); event `timer_aus` back |
| "Tell Yellow …" / love letters / questions / games by voice | Home Assistant scripts exposed to Assist call the MQTT commands |
| Banter: Nabu says a line, Yellow holds up a reply (LLM writes both) | Home Assistant script + `nachricht`; a friendship value changes the tone |
| Visits: Yellow walks out of his picture and appears on Nabu's OLED as pixel art, or a mini Nabu walks through Yellow's screen | gesture `besuch` / `nabu_kommt` plus the assistant's display |
| Nabu greets when it comes back online | gesture `winken` |

## Setup tips

- Expose only the few Yellow scripts to Assist that you want to say out loud (message, game, question, love letter);
  give each a clear description, the LLM picks them by that text.
- Keep the assistant's ring automation fast: Yellow only needs an `on` event, no retained state.
- The Home Assistant side is being turned into **blueprints** (ring → Yellow, timer → Yellow, banter, visits).
  Until then the original package is the reference; it uses the entity IDs of one installation.

## Not for everyone

The OLED visit, the friendship value and the banter tone are tuned to one family and stay examples; the ring, timer
and voice commands are meant to be generic.
