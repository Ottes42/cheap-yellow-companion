# Dashboard-Karte

🇬🇧 [English version](../docs/card.md)

![Yellow als Dashboard-Karte](../docs/media/card.png)

Eine Lovelace-Karte, auf der das echte Yellow im Browser läuft: dieselben Firmware-Header (`esphome/yellow*.h`) und
dieselbe LVGL-Version wie auf dem Gerät, als WebAssembly. Touch funktioniert wie auf dem Gerät: Antippen, Halten,
Streicheln, `?` für die Hilfe, unten links die Spiele.

Die Karte ist ein **eigenes** Yellow, kein Video vom Gerät. Sie kann den Einstellungen deines Geräts folgen (Figur,
Deko, Stimmung, Anspannung, Energie) und einer Wetter-Entität. Befehle, Post und Nachrichten bekommt sie nicht, und
sie schickt keine Ereignisse zurück; ein virtuelles Yellow, das das kann, ist als Integration geplant.

## Installieren

1. `yellow-card.js`, `yellow-en.js`, `yellow-en.wasm`, `yellow-de.js` und `yellow-de.wasm` aus dem neuesten
   [Release](https://github.com/Ottes42/cheap-yellow-companion/releases) laden oder selbst bauen (unten).
2. In einen Ordner unter `/config/www/` legen, z. B. `/config/www/yellow/`.
3. Einstellungen → Dashboards → ⋮ → Ressourcen → `/local/yellow/yellow-card.js` als JavaScript-Modul hinzufügen. Nach
   einem Update `?v=2` (usw.) an die URL hängen, damit Browser die neue Version laden.
4. Karte hinzufügen (YAML):

```yaml
type: custom:yellow-card
language: de                         # en oder de: die Texte auf dem Bildschirm
character: select.yellow_figur       # oder eine feste Figur: Okti, Robot oder Fox
decor: select.yellow_deko            # optional, ab hier die Entitäten des Geräts
mood: number.yellow_stimmung
tension: number.yellow_anspannung
energy: number.yellow_energie
weather: weather.zuhause             # optional: Wetter wie auf dem Gerät, Temperatur beim langen Drücken
```

Ohne Gerät reicht `character: Okti`. Die Entitäts-IDs hängen vom Gerätenamen und dessen Sprache ab (siehe
[Home Assistant](homeassistant.md#entitäten)); Auswahl-Entitäten zählen nach der Position der Option, deshalb geht
jede Sprache.

Die Karte pausiert, solange sie aus dem Bild gescrollt ist. Mehrere Karten mit derselben Sprache auf einer Seite teilen
sich ein Yellow.

## Bauen

Braucht [Emscripten](https://emscripten.org/) (`emcc`), Node.js und Python 3 mit `pyyaml`, `freetype-py` und
`esphome-glyphsets`:

```bash
web/build.sh        # → web/dist/
```

Das Skript holt LVGL und die Schriften, baut die Schriften aus dem `font:`-Block von `esphome/yellow-base.yaml`
(dieselben Glyphen wie auf dem Gerät) und je Sprache in `esphome/lang/` ein Modul. `web/test/snap.mjs` erzeugt Bilder in
Node ohne Browser (z. B. `node web/test/snap.mjs out.png --kind 2 --gesture wave`), `web/test/card.html` zeigt die Karte
ohne Home Assistant.

So funktioniert es: `web/main.cpp` macht, was `yellow-base.yaml` auf dem Gerät tut (Seite, Schild, Schriften,
Touch-Verteilung, Takt); `web/shim/` ersetzt die paar ESPHome-Funktionen, die die Header nutzen; `web/lv_conf.h` ist die
Konfiguration, die ESPHome erzeugt, angepasst auf 32-Bit-Farbe.
