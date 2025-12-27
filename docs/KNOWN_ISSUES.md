# Known Issues - Phase 4

**Created:** 2025-12-27
**Status:** ✅ RESOLVED - All gates cleared

## Gate A: Panel Resolution - UNVERIFIED

**Problem:** Widerspruch zwischen Quellen
- Schematic (docs/ESP32-S3-Touch-LCD-4.3-Sch.pdf): "800×480 Pixels"
- Projektname: "Waveshare43RGB**600**480_debug"

**Question for User:**
```
Was ist die korrekte Auflösung deines Displays?
  a) 800×480 (wie im Schematic)
  b) 600×480 (wie im Projektnamen)
  c) Andere: ___×___
```

---

## Gate B: Pin Conflict - DE vs Touch IRQ

**Problem:** GPIO5 wird möglicherweise doppelt verwendet
- Schematic Tabelle zeigt: IO5 = CTP_IRQ (Touch Interrupt)
- Schematic Text zeigt: IO5 oder IO6 für DE (Data Enable)
- Aktueller Code: `LCD_GPIO_DE = GPIO_NUM_5`

**Question for User:**
```
Welcher GPIO ist für DE (Data Enable) korrekt?
  a) GPIO5 (dann kein Touch IRQ möglich auf GPIO5)
  b) GPIO6 (dann ist GPIO5 frei für Touch IRQ)
  c) Anderer GPIO: ___

Und: Welcher GPIO ist für Touch IRQ (CTP_IRQ)?
  a) GPIO5
  b) Kein direkter GPIO (via IO Expander CH422G)
  c) Anderer GPIO: ___
```

---

## Gate C: Missing RGB Data Pins - R3, R4

**Problem:** R3 und R4 sind NICHT in der Schematic-Pinout-Tabelle
- Schematic Tabelle zeigt nur: R5=IO42, R6=IO41, R7=IO40
- R3 und R4 fehlen komplett
- Aktueller Code verwendet PLACEHOLDER: R3=GPIO4, R4=GPIO6 (GERATEN!)

**Möglichkeiten:**
1. R3/R4 sind auf anderen GPIOs (welche?)
2. R3/R4 sind intern im Display auf GND/VCC gelegt (nur 3-bit Red)
3. R3/R4 gehen durch IO Expander CH422G

**Question for User:**
```
Hast du Zugang zum Waveshare Demo-Code oder einer funktionierenden Referenz?
Falls ja: Welche GPIOs werden dort für R3 und R4 verwendet?

Alternativ: Kannst du die Pins R3/R4 am Board physisch mit Multimeter/Logicanalyzer tracen?
```

---

## Requested Information Sources

Um die Gates zu entsperren, wäre eine der folgenden Quellen hilfreich:

1. **Waveshare Official Demo Code** (Arduino oder ESP-IDF)
   - GitHub: waveshareteam/ESP32-S3-Touch-LCD-4.3
   - Enthält typischerweise komplette Pin-Konfiguration

2. **Hardware Test**
   - Multimeter: Durchgangsprüfung R3/R4 Pins vom LCD-Connector zum ESP32

3. **Waveshare Wiki/Docs**
   - https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-4.3

4. **Dein aktuell funktionierender Code** (falls vorhanden)
   - Welche Pins verwendest du dort?

---

## Current Assumptions (UNVERIFIED)

Falls keine weiteren Infos verfügbar, könnten wir mit diesen Annahmen fortfahren (mit Risiko):

| Parameter | Annahme | Risiko |
|-----------|---------|--------|
| Resolution | 800×480 | Falsch → Display-Tearing/Offset |
| DE Pin | GPIO5 | Falsch → Kein Bild ODER kein Touch |
| R3 | GPIO4 | Falsch → Farben falsch (Rot fehlt) |
| R4 | GPIO6 | Falsch → Farben falsch + evtl. DE Konflikt |
| Touch IRQ | NC (Polling) | Performance-Hit, aber funktional |

**User Decision Required:**
- [ ] Proceed with assumptions (accept risk)
- [ ] Provide verified pin mapping
- [ ] Research Waveshare demo code first

---
Last Updated: 2025-12-27T17:00:00Z
