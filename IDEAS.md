# Ideas

Undecided ideas. When one is decided it becomes an issue; when it is done it moves to the feature list.

## Real camera clips of Yellow on the desk

The README/gallery GIFs are pixel-exact screen recordings (`tools/bild.py`): sharp, but they look like a screen
capture. A few short real camera clips would show the actual board in a room, with light and reflections.

- A good webcam in front of Yellow, recorded on the Raspberry Pi it is attached to (ffmpeg, MJPEG 720p30, 5–10 s),
  converted to GIF/MP4.
- Candidates: ink cloud (Okti), mouse dive (fox), glitch (robot), fist bump with a real finger, love letter opened.
- Manual exposure for the bright display, some room light, no lamp reflecting on the glass.

## Polish by playing (owner's acceptance list)

Things that work in code but should be played with live and tuned:

- **Sulking and making up:** after 60 min without touch Yellow sulks; tapping gets "Hmpf", petting twice makes up
  (test: command `langeweile_test` with `minuten` 60).
- **Questions and games by finger:** answer buttons, reaction test, tic-tac-toe and Simon Says played on the device.
- **Voice assistant by voice:** "tell Yellow …", "Yellow should wave", banter, visits (see docs/nabu.md).
- **Taste:** arm speed, colours, mood slider, font size, tone of the one-liners (daily pool, banter, postcards).

## Sunrise alarm and evening routine

20 minutes before the alarm the display slowly brightens with a warm LED glow and Yellow sleeps, then holds up
"Kaffee?"; in the evening a calm routine. Exists in the owner's Home Assistant package (switches off by default); could
become a blueprint once tried live.

## Yellow "reads along" with the voice assistant

When the assistant answers, Yellow holds up a short keyword sign about it. (Was issue #21.)

## Relationship levels per person

Yellow knows who is using him (via Home Assistant) and has a relationship level per person; unlocks accessories,
per-person leaderboard. (Was issue #28.)

## From the research list

Mood diary, emergency full screen, underwater bubbles, blackjack, thought bubble instead of a sign, camouflage colour.
Pick one and make it an issue when it is taken up. (Was issue #30.)
