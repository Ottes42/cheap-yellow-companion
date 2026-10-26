# Yellow und Home Assistant

🇬🇧 [English version](../docs/homeassistant.md)

Yellow läuft auch ohne Home Assistant, aber damit kennt er Wetter, Wecker und Termine, reagiert auf die Türklingel
und den Sprachassistenten, und man kann ihm Nachrichten schicken. Diese Seite beschreibt die Einrichtung, die
Entitäten, die Sprache und jeden Blueprint. Die vollständige MQTT-Befehlsliste steht in der
[englischen README](../README.md#mqtt-topics-yellowdevice), der Sprachassistent hat eine eigene Seite:
[nabu.md](nabu.md).

## Einrichtung

1. **MQTT-Broker:** das Mosquitto-Add-on (oder ein anderer Broker) und die MQTT-Integration in Home Assistant.
2. **Ein MQTT-Benutzer je Yellow** (`mqtt_user`/`mqtt_password` in der Gerätedatei). Kann der Broker ACLs, diesem
   Benutzer nur `yellow/<gerät>/#` erlauben. Die Substitution `device` ist der Topic-Name (`yellow/<gerät>/…`) und
   der Eingang `device` jedes Blueprints.
3. **Flashen:** eigene Gerätedatei (Kopie von `esphome/example-home.yaml`, mit `language: de`) mit dem Profil
   `yellow-home.yaml`.
4. **Gerät hinzufügen:** in Home Assistant über die ESPHome-Integration (wird meist automatisch gefunden; der
   API-Schlüssel steht in den Secrets). Das bringt die Entitäten unten, OTA-Updates und Logs. Mit dem Profil
   `yellow-mobile.yaml` gibt es keine ESPHome-API: keine Entitäten, nur MQTT, die Blueprints funktionieren trotzdem.
5. **Blueprints importieren** (Knöpfe unten) und je Blueprint eine Automation bzw. ein Skript anlegen.

## Entitäten

Namen aus `esphome/lang/de.yaml` (englisch in `lang/en.yaml`). Home Assistant bildet die Entity-IDs aus Gerätename,
Entitätsname und manchmal dem Bereich, also auf der Geräteseite nachsehen statt raten.

| Entität | Art | Wofür |
|---|---|---|
| Yellow Stimmung | Zahl 0–100 | 0 gelb und froh … 100 rot und grummelig (Mund, Brauen, Oktis Farbe) |
| Yellow Anspannung | Zahl 0–100 | 0 entspannt … 100 sehr nervös (Tempo, Blinzeln, Schweiß, Zusammenzucken) |
| Yellow Energie | Zahl 0–100 | 0 todmüde … 100 hellwach (Atem, Gähnen, Langeweile); gut nach Tageszeit zu füttern |
| Yellow schläft | Schalter | legt ihn schlafen; Antippen weckt ihn und schaltet aus |
| Weckmodus | Schalter | Yellow darf im Dunkeln sichtbar sein (z. B. für einen Sonnenaufgangswecker) |
| Nachts aus | Schalter | Display im Dunkeln aus (Antippen zeigt trotzdem eine schwache Uhr) |
| Auto-Helligkeit | Schalter | Helligkeit nach dem Lichtsensor; aus, um das Displaylicht selbst zu setzen |
| Ansicht | Auswahl | Auto / Yellow / Uhr |
| Deko | Auswahl | Auto (nach Datum und Anlass) / Aus / Halloween / Weihnachten |
| Figur | Auswahl | Okti / Roboter / Fuchs |
| Yellow Post abspielen, Yellow Post leeren | Knöpfe | nächste wartende Nachricht zeigen bzw. alle verwerfen |
| Yellow Reaktionstest, Yellow Tic-Tac-Toe, Yellow Simon Says | Knöpfe | ein Spiel starten (gespielt wird am Touchscreen) |
| Yellow Frage abbrechen | Knopf | entfernt eine offene Frage ohne Antwort-Ereignis |
| Yellow Nachrichten wartend | Sensor | Anzahl der Nachrichten im Umschlag |
| Yellow heute | Textsensor | Tageszahlen `Tippen|Streicheln|Spiele|Faust`, um Mitternacht zurückgesetzt |
| Yellow sichtbar | Binärsensor | Yellow ist zu sehen (nicht dunkel, nicht Uhr-Ansicht) – nur dann erscheinen Befehle |
| Display, Status-LED | Lichter | Hintergrundlicht und die RGB-LED am Rücken |
| Umgebungslicht | Sensor | Spannung am Lichtsensor (höher = dunkler) |
| WLAN, Freier Speicher, Größter freier Block | Diagnose | verbundenes Netz und RAM (bei neuen Effekten im Blick behalten) |

## Sprache

Die Entitätsnamen folgen der Substitution `language: de|en` des Geräts. **Ein Sprachwechsel ergibt neue Entity-IDs**
(Home Assistant bildet sie aus den Namen): Automationen und Dashboards anpassen oder die neuen Entity-IDs in den
Entitätseinstellungen auf die alten umbenennen. Auch die Optionen der Auswahlen sind übersetzt (Uhr/Clock,
Roboter/Robot …).

Die Blueprints sprechen MQTT und nutzen Entitätsauswahlen nur für eigene Entitäten (Wetter, Klingel …), sie
funktionieren also weiter. Blueprints, die Wörter auf den Bildschirm schicken, haben einen eigenen Eingang
`language`.

## Blueprints

### Daten senden

[![Import](https://my.home-assistant.io/badges/blueprint_import.svg)](https://my.home-assistant.io/redirect/blueprint_import/?blueprint_url=https%3A%2F%2Fgithub.com%2FOttes42%2Fcheap-yellow-companion%2Fblob%2Fmain%2Fhomeassistant%2Fblueprints%2Fautomation%2Fyellow_data.yaml)
`automation/yellow_data.yaml`

Schickt, was Uhr-Ansicht und Wetter zeigen, an `yellow/<gerät>/data/…` (retained): Wettertext und Wetterlage (heute,
ab 14 Uhr morgen), nächster Wecker in den nächsten 20 Stunden, nächster Termin des Tages, eine Hinweiszeile, Außen-
und Raumtemperatur, Raumfeuchte. Läuft alle 10 Minuten, beim Start von Home Assistant und wenn das Gerät online kommt.

| Eingang | |
|---|---|
| `device` | die Substitution `device` des Geräts |
| `language` | `de` oder `en`: Wörter in den Zeilen („Heute“, „Wecker“) |
| `weather` | Wetter-Entität (Tagesvorhersage) |
| `alarm` | optional: Zeitstempel-Sensor, z. B. der nächste Wecker vom Handy |
| `calendar` | optional: Kalender für den nächsten Termin heute |
| `notice` | optional: Entität als zusätzliche Zeile (z. B. Müllabfuhr); `-` oder leer blendet sie aus |
| `progress` | optional: Entität, die beim Halten als Letztes erscheint (z. B. Template-Sensor „Noch 7 Punkte bis zum Hut“); kurz halten |
| `outdoor_temp`, `room_temp`, `room_humidity` | optionale Sensoren |

```yaml
automation:
  - alias: Yellow – Daten
    use_blueprint:
      path: Ottes42/yellow_data.yaml
      input:
        device: yellow
        language: de
        weather: weather.home
        alarm: sensor.handy_naechster_wecker
```

### Gruß-Zeile

[![Import](https://my.home-assistant.io/badges/blueprint_import.svg)](https://my.home-assistant.io/redirect/blueprint_import/?blueprint_url=https%3A%2F%2Fgithub.com%2FOttes42%2Fcheap-yellow-companion%2Fblob%2Fmain%2Fhomeassistant%2Fblueprints%2Fautomation%2Fyellow_greeting.yaml)
`automation/yellow_greeting.yaml`

Der erste Tipp des Tages (ab 05:00) zeigt einen Gruß: „Guten Morgen“ (ab 11:00 „Hallo“), einen Satz zum Wochentag,
das Wetter aus *Daten senden* und eine Zusatzzeile aus `data/greeting`. Dieser Blueprint schreibt die Zeile jeden
Morgen (Standard 04:30). Mit einer AI-Task-Entität fragt er dein LLM (Anweisung änderbar), ohne sie schickt er den
Zustand einer beliebigen Entität (z. B. `input_text`), ohne beides leert er die Zeile (`-`). Eingänge: `device`,
`time`, `ai_task`, `instructions`, `source`. Test am Gerät: Befehl `greeting_test`, dann Yellow antippen.

```yaml
automation:
  - use_blueprint:
      path: Ottes42/yellow_greeting.yaml
      input:
        device: yellow
        ai_task: ai_task.mein_llm
```

### Pflanzen-Tagebuch

[![Import](https://my.home-assistant.io/badges/blueprint_import.svg)](https://my.home-assistant.io/redirect/blueprint_import/?blueprint_url=https%3A%2F%2Fgithub.com%2FOttes42%2Fcheap-yellow-companion%2Fblob%2Fmain%2Fhomeassistant%2Fblueprints%2Fautomation%2Fyellow_plant.yaml)
`automation/yellow_plant.yaml`

Yellow bedankt sich, wenn die Pflanze gegossen wird (die Bodenfeuchte steigt in einem Schritt um mindestens `jump`
Punkte), und tanzt. Wurde sie `days` Tage nicht gegossen **und** liegt die Feuchte unter `dry`, wird er einmal am Tag
ab `time` vorwurfsvoll: Gießkanne, hängende Arme und ein Schild, das spitzer wird (nach 2× und 3× den Tagen). Mit
`visible` (Sensor „sichtbar“ des Geräts) reagiert er live, nur wenn man ihn sieht; ohne kommt der Vorwurf als Post.
**Vorher einen `input_datetime`-Helfer anlegen** (Datum und Uhrzeit), z. B. „Pflanze zuletzt gegossen“; der
Blueprint setzt ihn, und man kann ihn von Hand setzen, wenn der Sensor das Gießen nicht merkt. Eingänge: `device`,
`language`, `moisture`, `last_watered`, `days` (5), `dry` (30 %, 100 = nur Tage), `jump` (10), `time` (18:00),
`visible`, `name`.

```yaml
automation:
  - use_blueprint:
      path: Ottes42/yellow_plant.yaml
      input:
        device: yellow
        language: de
        moisture: sensor.pflanze_feuchte
        last_watered: input_datetime.pflanze_gegossen
        visible: binary_sensor.yellow_sichtbar
        name: der Kaktus
```

### Begrüßen, wenn jemand reinkommt

[![Import](https://my.home-assistant.io/badges/blueprint_import.svg)](https://my.home-assistant.io/redirect/blueprint_import/?blueprint_url=https%3A%2F%2Fgithub.com%2FOttes42%2Fcheap-yellow-companion%2Fblob%2Fmain%2Fhomeassistant%2Fblueprints%2Fautomation%2Fyellow_greet.yaml)
`automation/yellow_greet.yaml`

Kommt jemand herein, schaut Yellow hin (optional `direction`), winkt und zeigt „Hallo {name}!“ oder eine von ein
paar trockenen Varianten (oder den eigenen `text`). Jede Anwesenheits-Entität geht: ein Bermuda-Raumsensor
(`room_state` = Name des Bereichs; braucht einen Bluetooth-Proxy im Raum, einen eigenen kleinen ESP32 – nicht auf
Yellow, dafür reicht der RAM nicht), ein Belegungs-Binärsensor (`on`), eine Person (`home`) oder zum Testen ein
`input_boolean`. Er grüßt erst, wenn die Person `away_minutes` (15) woanders war und die Anwesenheit `settle`
Sekunden (30) hält; flatternde Anwesenheit wiederholt ihn also nicht. Mit `visible` entfällt ein Gruß, den niemand
sieht. Eine Instanz pro Person.

```yaml
automation:
  - use_blueprint:
      path: Ottes42/yellow_greet.yaml
      input:
        device: yellow
        language: de
        presence: sensor.anna_handy_bereich
        room_state: Büro
        name: Anna
        visible: binary_sensor.yellow_sichtbar
```

### Türklingel

[![Import](https://my.home-assistant.io/badges/blueprint_import.svg)](https://my.home-assistant.io/redirect/blueprint_import/?blueprint_url=https%3A%2F%2Fgithub.com%2FOttes42%2Fcheap-yellow-companion%2Fblob%2Fmain%2Fhomeassistant%2Fblueprints%2Fautomation%2Fyellow_doorbell.yaml)
`automation/yellow_doorbell.yaml`

Geht der Klingel-Binärsensor an, zeigt Yellow 60 s das Klingel-Panel und reagiert (nur wenn der Raum hell ist).
Eingänge: `device`, `doorbell` (Binärsensor).

### LED-Ring des Sprachassistenten

[![Import](https://my.home-assistant.io/badges/blueprint_import.svg)](https://my.home-assistant.io/redirect/blueprint_import/?blueprint_url=https%3A%2F%2Fgithub.com%2FOttes42%2Fcheap-yellow-companion%2Fblob%2Fmain%2Fhomeassistant%2Fblueprints%2Fautomation%2Fyellow_voice_assistant_ring.yaml)
`automation/yellow_voice_assistant_ring.yaml`

Yellow schaut neidisch oder neugierig hin, sobald der LED-Ring des Assistenten leuchtet. Braucht das Paket
`yellow-nabu.yaml` auf dem Gerät. Eingänge: `device`, `ring` (Licht-Entität des Rings). Mehr in [nabu.md](nabu.md).

### Lesebestätigung

[![Import](https://my.home-assistant.io/badges/blueprint_import.svg)](https://my.home-assistant.io/redirect/blueprint_import/?blueprint_url=https%3A%2F%2Fgithub.com%2FOttes42%2Fcheap-yellow-companion%2Fblob%2Fmain%2Fhomeassistant%2Fblueprints%2Fautomation%2Fyellow_post_read.yaml)
`automation/yellow_post_read.yaml`

Jemand hat Yellow angetippt und eine wartende Nachricht oder einen Liebesbrief gelesen: dann laufen die eigenen
Aktionen. Sie können die Variablen `text` (die Nachricht), `love` (true bei einem Liebesbrief) und `device` nutzen.
Eingänge: `device`, `actions`.

```yaml
automation:
  - alias: Yellow – Lesebestätigung
    use_blueprint:
      path: Ottes42/yellow_post_read.yaml
      input:
        device: yellow
        actions:
          - action: notify.mobile_app_handy
            data:
              message: "Gelesen: {{ text }}"
```

### Nachricht (Skript)

[![Import](https://my.home-assistant.io/badges/blueprint_import.svg)](https://my.home-assistant.io/redirect/blueprint_import/?blueprint_url=https%3A%2F%2Fgithub.com%2FOttes42%2Fcheap-yellow-companion%2Fblob%2Fmain%2Fhomeassistant%2Fblueprints%2Fscript%2Fyellow_message.yaml)
`script/yellow_message.yaml`

Ein Skript mit den Feldern `text`, `wait` (als Post zustellen: Umschlag, bis jemand antippt) und `love_letter`
(rosa Umschlag, Herzen). Lange Texte werden auf mehrere Schilder verteilt. Für Assist freigeben und „Sag Yellow …“
sagen; die Beschreibung des Skripts sagt dem Sprachmodell, wann es passt. Eingang: `device`.

```yaml
action: script.yellow_message
data:
  text: Essen ist fertig!
  wait: true
```

## Alles andere

Jeder andere Befehl (Gesten, Requisiten, Fragen, Spiele, Timer, Verreisen …) ist eine JSON-Nachricht an
`yellow/<gerät>/command`:

```yaml
action: mqtt.publish
data:
  topic: yellow/yellow/command
  payload: '{"action": "gesture", "gesture": "wave"}'
```

Ereignisse kommen auf `yellow/<gerät>/event` zurück (`type`: read, answer, game, petted, timer_off) – dafür einen
MQTT-Auslöser mit `trigger.payload_json` nutzen. Alle Aktionen, Felder und Werte:
[README, MQTT topics](../README.md#mqtt-topics-yellowdevice).

## Tipps

- Jedem Yellow einen eigenen MQTT-Benutzer geben; ein Gerät je Topic-Name.
- Nur die Skripte für Assist freigeben, die man laut sagen will; der Sprachassistent steht in [nabu.md](nabu.md).
- Befehle erscheinen nur, wenn Yellow sichtbar ist (Entität *Yellow sichtbar*); Nachrichten mit `wait` überstehen
  Dunkel und Uhr-Ansicht und werden später zugestellt.
