# Yellow und der Sprachassistent („Nabu“)

🇬🇧 [English version](../docs/nabu.md)

Yellow ist stumm, der Sprachassistent neben ihm nicht. Im Originalaufbau ist das ein **Home Assistant Voice**
(Preview Edition) namens *Nabu* mit eigener ESPHome-Firmware und kleinem OLED. Alles hier ist optional: ohne läuft
Yellow ganz normal.

## Was heute geht

| Funktion | Wie |
|---|---|
| Yellow schaut zu Nabus LED-Ring (neidisch oder neugierig) | `yellow-nabu.yaml` auf Yellow; Home Assistant schickt `on` an `yellow/<gerät>/data/nabu_ring`, wenn der Ring leuchtet (Blueprint *Voice assistant LED ring*) |
| Yellow weiß, wo Nabu steht (links, rechts, anderer Raum) | Befehl `nabu_direction` mit `direction` (left, right, up = anderer Raum); Home Assistant kann den Raum am WLAN-Signal des Assistenten erkennen |
| Timer des Assistenten erscheinen auf Yellow, Antippen beim Klingeln stoppt den Timer | Befehl `timer` (`seconds` > 0 läuft, 0 vorbei, < 0 klingelt); Ereignis `timer_off` zurück |
| „Sag Yellow …“ / Liebesbriefe / Fragen / Spiele per Sprache | Home-Assistant-Skripte, für Assist freigegeben, rufen die MQTT-Befehle auf |
| Schlagabtausch: Nabu sagt einen Satz, Yellow hält eine Antwort hoch (beides schreibt das LLM) | Home-Assistant-Skript + `message`; ein Freundschaftswert ändert den Ton |
| Besuche: Yellow läuft aus seinem Bild und erscheint als Pixelbild auf Nabus OLED, oder ein Mini-Nabu läuft durch Yellows Bild | Geste `visit_nabu` / `nabu_visits` plus das Display des Assistenten |
| Nabu grüßt, wenn er wieder online ist | Geste `wave` |

## Tipps zur Einrichtung

- Einrichtung in Home Assistant und die Blueprints: [homeassistant.md](homeassistant.md).
- Nur die wenigen Yellow-Skripte für Assist freigeben, die man laut sagen will (Nachricht, Spiel, Frage, Liebesbrief);
  jedes mit einer klaren Beschreibung, das LLM wählt danach aus.
- Die Ring-Automation schnell halten: Yellow braucht nur ein `on`-Ereignis, keinen gespeicherten Zustand.
- Die Home-Assistant-Seite wird gerade zu **Blueprints** (Ring → Yellow, Timer → Yellow, Schlagabtausch, Besuche).
  Bis dahin ist das Originalpaket die Vorlage; es nutzt die Entity-IDs einer Installation.

## Nicht für alle

Der OLED-Besuch, der Freundschaftswert und der Ton des Schlagabtauschs sind auf eine Familie zugeschnitten und bleiben
Beispiele; Ring, Timer und Sprachbefehle sind allgemein gedacht.
