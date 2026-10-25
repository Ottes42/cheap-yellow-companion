# Funktionen

🇬🇧 [English version](../FEATURES.md)

Alles, was Yellow heute kann – als Nachschlagewerk für Neugierige und als schnelle Orientierung für alle (Mensch oder
Agent), die an diesem Repo arbeiten. Die [README](../README.de.md) bleibt kurz, diese Liste ist vollständig. Änderungen
je Version: [CHANGELOG.md](../CHANGELOG.md) (englisch).

> Befehlsreferenz: [README, MQTT topics](../README.md#mqtt-topics-yellowdevice) (englisch). Diese Seite beschreibt die
> Funktionen aus Nutzersicht und nennt keine Befehls-, Feld- oder Gestennamen.

Wo es steht: `esphome/yellow.h` (Klasse `Companion`, Tick-Schleife) und `esphome/yellow_*.h` (Themen),
`esphome/yellow-*.yaml` (Pakete), `esphome/lang/` (alle Texte je Sprache), `homeassistant/blueprints/`
(Home-Assistant-Seite).

## Figuren

| Funktion | Beschreibung | Seit |
|---|---|---|
| Drei Figuren | Okti (Wende-Oktopus, gelb und froh oder rot und grummelig, acht Arme), ein Fuchs (mutig, etwas frech, buschiger Schwanz) und ein kleiner Roboter (Visier, Antenne, pulsierendes Herzlicht). Am Gerät gewählt oder beim Build fest eingebrannt. | 0.1.0 |
| Nur Formen | Aus LVGL-Formen, Bögen und Linien gezeichnet, keine Bitmaps; passt auf einen ESP32 ohne PSRAM. | 0.1.0 |
| Sprachen | Englisch oder Deutsch, beim Kompilieren gewählt (`language: en\|de`): alle Bildschirmtexte, Entitätsnamen und die Begrüßung beim Start; Datums- und Dezimalformat wechseln mit. Bis 0.1.0 gab es nur Deutsch. | 0.2.0 |
| Lebendig im Leerlauf | Atmet, blinzelt, schaut sich um, hüpft, gähnt, zappelt, winkt, läuft auf einer Seite hinaus und kommt auf der anderen zurück; alle paar Sekunden eine kleine Idee. | 0.1.0 |
| Auftritt | Wird er sichtbar, kommt er von der Seite herein: der Fuchs läuft, Okti krabbelt, der Roboter rollt. | 0.1.0 |
| Start | Zufälliger Satz auf dem Startbildschirm, dann ein Winken und eine Begrüßung (mit dem Namen der Besitzerin bzw. des Besitzers und der Figur, falls gesetzt). | 0.1.0 |
| Stimmung | Regler von froh und gelb bis grummelig und rot (Mundwinkel, Brauen, Oktis Farbe). Schlechtes Wetter schiebt sie etwas an. | 0.1.0 |
| Anspannung | Regler von entspannt bis sehr nervös: Tempo von Armen und Schwanz, Blinzeln, Blicke, Atem, Schweiß, Zusammenzucken. | 0.1.0 |
| Energie | Regler von todmüde bis hellwach: Atem, Gähnen, halb geschlossene Augen, wie schnell ihm langweilig wird. | 0.1.0 |
| Gesichtsausdrücke | Normal, froh, überrascht, schwindlig, müde, genervt, traurig, schlafend, gähnend; für ein paar Sekunden oder bis zur nächsten Berührung. | 0.1.0 |
| Gesten | Hüpfen, wackeln, wedeln/zappeln, nicken, Kopf schütteln, winken, erschrecken, welk (Arme hängen, traurig), Freudentanz, aufsteigende Herzen, Orakel (zufälliges Antwortschild). | 0.1.0 |
| Effekte | Schweißtropfen, Träne, Erröten, Funkeln; auch automatisch (Erröten beim Streicheln, Schweiß bei Spielen und in den letzten Timer-Sekunden, Träne bei Traurigkeit). | 0.1.0 |
| Blickrichtung | Für ein paar Sekunden nach links, rechts, oben, unten oder in die Mitte schauen. | 0.1.0 |
| Requisiten | Sonnenbrille, Regenschirm, Schal, Fächer (der Fuchs fächelt stattdessen mit dem Schwanz), Gießkanne, Wäschekorb, Mülltonne, Kissen, eine comichafte Tüte; für einige Sekunden oder bis zum Entfernen. | 0.1.0 |
| Accessoires | Hut, Krone, Stern oder Blume auf dem Kopf (ein Anlass-Hut geht vor). | 0.1.0 |
| Schmuck | Goldkette mit funkelndem Anhänger; der Fuchs bekommt dazu Ohrstecker. Kombinierbar mit Hüten. | 0.1.0 |
| Special Moves | Okti: Tintenwolke, verknotete Arme, Jonglieren (lässt manchmal einen Ball fallen). Fuchs: Mäuselsprung in einen Erdhügel, Schwanzfächer, jagt den eigenen Schwanz. Roboter: Visier-Glitch mit „Neustart“, Wartung (Visier klappt auf, Zahnräder drehen sich), Propellerstart. Zufällig im Leerlauf oder per Befehl. | 0.1.0 |
| Tollpatschige Schilder | Manchmal steht das Schild auf dem Kopf oder hat einen Buchstabendreher, und er korrigiert es genervt (Kopfstand nur, wenn der RAM reicht). Okti lässt sein Schild manchmal fallen. | 0.1.0 |
| Charakter | Eigene Leerlauf-Sprüche je Figur (Okti tollpatschig, Roboter übergenau, Fuchs mutig), der Fuchs zeigt Mut, der Roboter blinkt bei jedem Schild mit der LED. | 0.1.0 |
| Tageszähler | Statuszeile mit den heutigen Tipps, Streicheleinheiten, Spielen und Faustgrüßen; er kommentiert sie („Heute 10× angetippt. Reicht.“). Um Mitternacht zurückgesetzt. | 0.1.0 |
| Langeweile-Treppe | Ab der letzten Berührung: 5 min neugierig, 10 min gelangweilt (tippt mit dem Arm), 20 min müde, 40 min döst, 60 min schmollt (rot, schaut weg; Antippen bringt nur „Hmpf.“, zweimal Streicheln versöhnt). | 0.1.0 |
| Schlafen | Ins Bett schicken: Okti im Bett unter einer Decke, der Fuchs zusammengerollt, der Roboter im Standby mit pulsierendem Punkt. Antippen weckt ihn. | 0.1.0 |
| Lichtschreck | Geht nach Dunkelheit das Zimmerlicht an, erschrickt er, jede Figur anders (der Fuchs blinzelt nur). | 0.1.0 |
| Verreist | Yellow ist weg: ein Koffer mit Aufklebern und einem Zettel (dein Postkartentext); er kommt mit einem Souvenir-Requisit zurück. | 0.1.0 |
| Besuche | Yellow läuft aus dem Bild zum Sprachassistenten und kommt zurück; oder ein Mini-Sprachassistent kommt herein und Yellow begrüßt ihn. | 0.1.0 |
| Matrix-Regen | Grüne Einsen und Nullen fallen eine Weile hinter der Figur (das Nerd-Ereignis um 13:37; den Auslöser um 13:37 liefert deine Automation). | 0.1.0 |
| Status-LED | Die RGB-LED am Rücken macht mit: blitzt beim Antippen, pulsiert bei wartender Post, Farben bei Simon Says, rot/orange, solange ein Timer klingelt. | 0.1.0 |

## Touch und Bildschirm

| Funktion | Beschreibung | Seit |
|---|---|---|
| Antippen | Er hüpft und freut sich; Antippen holt auch wartende Post ab, beantwortet einen klingelnden Timer oder weckt ihn. | 0.1.0 |
| Folgt dem Finger | Die Augen folgen dem Finger; der nächste Arm (Okti), die Hand des Roboters oder die Pfote des Fuchses greift danach. | 0.1.0 |
| Streicheln (wischen) | Wischen streichelt ihn: froh, errötet, warme LED. Meldet, welches Schild gerade zu sehen war (sein „Lieblingsspruch“). | 0.1.0 |
| Halten | Langer Druck zeigt die Infos des Tages als Schilder: Wecker, Termin, Wetter, Hinweis, Raumtemperatur. | 0.1.0 |
| 4× schnell tippen | Mag er nicht: schwindlig und „Bitte nicht.“; Okti verschwindet manchmal in Tinte, der Roboter glitcht. | 0.1.0 |
| Faustgruß | Ab und zu (oder per Befehl) kommt eine Faust nach vorn mit „Faust?“; antippen gibt ein „Bumm!“ mit Funken, sonst ist er nach 8 s enttäuscht. | 0.1.0 |
| Hilfeseite | **?** unten rechts zeigt, wie man Yellow bedient; jeder Tipp schließt sie, nach 20 s schließt sie sich selbst. | 0.1.0 |
| Spieleauswahl | Der Gamepad-Knopf unten links öffnet das Spielemenü. | 0.1.0 |
| Kleine Uhr | Uhr oben rechts auf Yellows Bildschirm; antippen zeigt 15 s die große Uhr-Ansicht. | 0.1.0 |

## Spiele und Fragen

| Funktion | Beschreibung | Seit |
|---|---|---|
| Reaktionstest | „Wenn ich springe: tippen!“; zeigt deine Zeit in ms, erkennt zu frühes Tippen und Einschlafen. | 0.1.0 |
| Tic-Tac-Toe | Du fängst an (X); er blockt meistens und schummelt, wenn er verliert („Hab ich nicht gesehen.“). Gibt nach 2 min ohne Zug auf. | 0.1.0 |
| Simon Says | Vier Farbfelder (und die LED) in einer wachsenden Folge; er zählt deine Runden. | 0.1.0 |
| Ergebnisse | Jedes Spiel meldet sein Ergebnis (Zeit, Sieg/Unentschieden/Niederlage, Runden oder abgebrochen) als Ereignis. Auch aus Home Assistant startbar. | 0.1.0 |
| Fragen | Ein Schild mit zwei Antwortknöpfen (Standard Ja/Nein); wartet, bis er sichtbar ist, gibt nach 10 min auf, meldet die Antwort. Aus Home Assistant abbrechbar. | 0.1.0 |

## Post und Nachrichten

| Funktion | Beschreibung | Seit |
|---|---|---|
| Schilder | Yellow ist stumm und hält Pappschilder in Handschrift hoch; lange Texte werden auf mehrere Schilder verteilt. | 0.1.0 |
| Sofortnachricht | Erscheint sofort für ein paar Sekunden (nur, solange er sichtbar ist). | 0.1.0 |
| Post | Eine Nachricht wartet im Umschlag (bis zu 5); er winkt, hüpft, „Post für dich!“, die LED pulsiert, bis jemand antippt. Übersteht Uhr-Ansicht und Dunkelheit. | 0.1.0 |
| Liebesbriefe | Rosa Umschlag mit Herzsiegel; beim Öffnen errötet er und Herzen steigen auf. | 0.1.0 |
| Lesebestätigung | Jede geöffnete Nachricht wird zurückgemeldet (Liebesbriefe als solche gekennzeichnet). | 0.1.0 |
| Post aus Home Assistant | Nächste wartende Nachricht abspielen oder alle löschen, als Knöpfe. | 0.1.0 |
| Timer-Anzeige | Zeigt einen laufenden Timer (Name und Zeit), meldet sich zwischendurch, zählt die letzten 5 s mit großen Ziffern, hüpft beim Klingeln; Antippen stoppt ihn und meldet das zurück. | 0.1.0 |
| Sprüche aus der Ferne | Eigene Liste von Leerlauf-Sprüchen per MQTT; auf dem Gerät gespeichert, übersteht also einen Neustart ohne Netz. Jeder dritte Leerlauf-Spruch kommt daraus. | 0.1.0 |

## Anlässe, Jahreszeiten und Wetter

| Funktion | Beschreibung | Seit |
|---|---|---|
| Halloween- und Weihnachtsszenen | Ganze Hintergründe: Mond, Fledermäuse und Kürbisse (Kürbishut); Tannenbaum, Lichterkette, Geschenk und Schnee (Weihnachtsmütze). Automatisch Mitte Oktober bis 1. November und 1.–26. Dezember, oder fest per Auswahl. | 0.1.0 |
| Anlässe | Hut-Symbol plus Deko am Rand für: Ostern, Silvester, Valentinstag (rosa Yellow), Karneval, Advent (so viele Kerzen brennen, wie Advent ist), Nikolaus, St. Martin, Star-Wars-Tag, Pi-Tag, Handtuchtag, Piratentag, 4/20, Geburtstag, Oktoberfest, Erntedank, Zeitumstellung, Vollmond, Perseiden, Freitag der 13., „Zurück in die Zukunft“-Tag, Hobbit-Tag, Tag des Kaffees, Tag des Programmierers, Katzentag, Muttertag, Vatertag, Sonnenwende, Black Friday, UFO-Tag, Emoji-Tag, Tag des Bieres. Welcher heute ist, kommt von deiner Seite (MQTT-Daten). | 0.1.0 |
| Jahreszeiten | Deko-Sätze für Frühling, Sommer, Herbst und Winter, ebenfalls aus den Anlassdaten. | 0.1.0 |
| Deko-Auswahl | Auto, aus, Halloween oder Weihnachten; dazu ein Testbefehl, um jeden Satz auszuprobieren. | 0.1.0 |
| Wetteransicht | Hinter der Figur: Sonne, Wolken, Regen, Schnee, Schneeregen, Hagel, Nebel, Wind und Blitze, animiert. | 0.1.0 |
| Wetterreaktionen | Bei einem Wechsel schaut er nach oben und reagiert je Figur („Schnee!“, „Sonne!“, Fuchs: „Pff. Donner.“, Okti wird durchgeschüttelt, der Roboter „misst“ den Wind). | 0.1.0 |

## Tag, Nacht und Uhr-Ansicht

| Funktion | Beschreibung | Seit |
|---|---|---|
| Nachtregel | Im dunklen Raum (Lichtsensor) zeigt sich Yellow nie und reagiert nicht; Antippen zeigt 30 s eine ganz schwache Uhr. Optional: Bildschirm nachts ganz aus. | 0.1.0 |
| Uhr-Ansicht | Uhr im Querformat mit blinkendem Doppelpunkt, Datum, Raum- und Außenklima, nächstem Wecker, nächstem Termin, Wetter und einer Hinweiszeile (leere Zeilen verschwinden). Tag- und Nachtfarben. | 0.1.0 |
| Ansicht-Auswahl | Auto, immer Yellow oder immer Uhr. | 0.1.0 |
| Weckmodus | Ein Schalter, mit dem Yellow im Dunkeln sichtbar sein darf (für eine Sonnenaufgangs-Automation). | 0.1.0 |
| Auto-Helligkeit | Das Hintergrundlicht folgt dem Lichtsensor und den Berührungen; abschaltbar. | 0.1.0 |
| Türklingel | Klingel-Panel mit Uhrzeit für 60 s (nur, wenn der Raum hell ist); jede Figur reagiert auf ihre Art. | 0.1.0 |

## Netzwerk

| Funktion | Beschreibung | Seit |
|---|---|---|
| Netzstatus | Rotes Symbol und „wartet auf Netz: …“ ohne WLAN; orange Wolke, wenn der MQTT-Server 30 s nicht erreichbar ist, orange Weltkugel, wenn das WLAN kein Internet hat. | 0.1.0 |
| Internet-Check | Solange MQTT fehlt, wird jede Minute eine einstellbare URL abgefragt; jede HTTP-Antwort zählt. | 0.1.0 |
| WLAN-Dialog | Rotes Symbol antippen: Liste der Netze (stärkstes zuerst), Passwort per Bildschirmtastatur, auf dem Gerät gespeichert. Schließt sich nach 5 min ohne Berührung. | 0.1.0 |
| Profile | Zuhause: lokaler Broker ohne TLS, Zeit von Home Assistant, ESPHome-API für OTA und Logs. Unterwegs: eigener öffentlicher Broker mit TLS, Zeit per SNTP, läuft offline weiter. | 0.1.0 |

## MQTT

| Funktion | Beschreibung | Seit |
|---|---|---|
| Befehle | Ein JSON-Befehls-Topic für alles oben (Nachrichten, Liebesbriefe, Gesichtsausdrücke, Requisiten, Effekte, Gesten, Blickrichtung, Spiele, Fragen, Timer, Verreisen, Schlafen, Matrix, Wetter, Deko, Schmuck, Accessoires). Die Schnittstelle ist seit 0.2.0 englisch. | 0.1.0 |
| Daten-Topics | Gespeicherte (retained) Daten für Anlass, Wecker, Termin, Hinweis, Wettertext und Wetterlage, Außen- und Raumtemperatur, Feuchte, Spruchliste; Ereignisse ohne retain für Türklingel und den Ring des Sprachassistenten. | 0.1.0 |
| Ereignisse | JSON-Ereignisse zurück: Nachricht gelesen, Antwort, Spielergebnis, gestreichelt (mit dem letzten Schild), Timer gestoppt. | 0.1.0 |
| Telemetrie | Retained, beim Verbinden und alle 30 min: Firmware, Laufzeit, Neustartgrund, freier RAM und größter Block, WLAN und Signal, dunkel oder nicht, aktuelle Ansicht und Figur, Tageszähler, dazu das Aktivitätsprotokoll seit dem letzten Bericht (bis zu 30 Einträge). Auch per Befehl. | 0.1.0 |
| Status | Online/offline, retained, mit Last Will. | 0.1.0 |
| Testbefehle | Eine Langeweile-Dauer, einen Netzzustand, eine Wetterlage oder einen Anlass vortäuschen; den WLAN-Dialog ohne Berührung öffnen. | 0.1.0 |

## Home Assistant

Einrichtung, Entitäten und Blueprints im Detail: [docs_de/homeassistant.md](homeassistant.md).

| Funktion | Beschreibung | Seit |
|---|---|---|
| Geräte-Entitäten | Über die ESPHome-API: Regler für Stimmung, Anspannung und Energie; Schalter für Schlafen, Nachts aus, Weckmodus und Auto-Helligkeit; Auswahlen für Ansicht, Deko und Figur; Knöpfe für Post abspielen, Post leeren, Frage abbrechen und die drei Spiele; Sensoren für wartende Nachrichten, Tageszähler, sichtbar, WLAN-Name, Helligkeit und freien Speicher; Hintergrundlicht und Status-LED als Lichter. | 0.1.0 |
| Dashboard-Karte | Das echte Yellow im Browser als Lovelace-Karte (dieselbe Firmware, WebAssembly): Touch wie auf dem Gerät, folgt Figur, Deko und Reglern des Geräts und dem Wetter; ein eigenes Yellow, ohne Befehle oder Nachrichten. [docs_de/card.md](card.md) | 0.3.0 |
| Blueprint: Daten senden | Wetter, nächster Wecker, nächster Termin, eine Hinweiszeile, Außen-/Raumtemperatur und Feuchte. | 0.1.0 |
| Blueprint: Türklingel | Schickt das Klingel-Ereignis. | 0.1.0 |
| Blueprint: Ring des Sprachassistenten | Yellow schaut hin, wenn der LED-Ring des Assistenten leuchtet. | 0.1.0 |
| Blueprint: Lesebestätigung | Eigene Aktionen, wenn eine Nachricht oder ein Liebesbrief gelesen wurde. | 0.1.0 |
| Blueprint: Nachrichten-Skript | Nachricht, Post oder Liebesbrief; für Assist freigeben für „Sag Yellow …“. | 0.1.0 |

## Sprachassistent

Optional, Details in [docs_de/nabu.md](nabu.md).

| Funktion | Beschreibung | Seit |
|---|---|---|
| Schaut zum LED-Ring | Mit `yellow-nabu.yaml`: leuchtet der Ring des Assistenten, schaut er hin, neidisch („Angeber.“) oder neugierig (höchstens alle 20 s). | 0.1.0 |
| Weiß, wo er steht | Links, rechts oder anderer Raum; sein Blick geht dorthin. | 0.1.0 |
| Timer | Die Timer des Assistenten erscheinen auf Yellow; antippen, während er klingelt, stoppt den Timer. | 0.1.0 |
| Besuche | Yellow besucht den Assistenten, oder ein Mini-Assistent besucht Yellow (die Display-Seite des Assistenten ist nicht in diesem Repo). | 0.1.0 |

## Werkzeuge

| Funktion | Beschreibung | Seit |
|---|---|---|
| Screenshots und GIFs | Optionales Paket `yellow-screenshot.yaml` plus `tools/screenshot.py`: pixelgenaue Bilder per MQTT, ohne Kamera; `tools/frame.py` setzt sie in eine gezeichnete Platine. Nur vorübergehend einbinden. | 0.2.0 |
