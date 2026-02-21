# Switch Widget

## Übersicht

Der `Switch` ist ein Material-Design-Toggle-Schalter für Qt Widgets, basierend auf dem [Material Design Specification für Selection Controls](https://material.io/design/components/selection-controls.html#switches). Die Implementierung stammt ursprünglich von Iman Ahmadvand (Copyright 2018–2020) und ist unter der **GNU General Public License v3** lizenziert.

Der Switch verhält sich wie ein `QCheckBox`, ist aber optisch als animierter Kippschalter (Thumb + Track) dargestellt.

---

## Klassen

### `Animator`

Interne Hilfsklasse, die `QVariantAnimation` erweitert und an ein Zielobjekt (`QObject*`) gebunden werden kann.

| Methode | Beschreibung |
|---|---|
| `Animator(QObject* target, QObject* parent)` | Konstruktor, setzt das Animations-Zielobjekt |
| `setTargetObject(QObject*)` | Setzt das Zielobjekt (nur im gestoppten Zustand) |
| `targetObject()` | Gibt das aktuelle Zielobjekt zurück |
| `isRunning()` | Gibt `true` zurück, wenn die Animation läuft |
| `setup(int duration, QEasingCurve)` | Setzt Dauer und Easing |
| `interpolate(start, end)` | Startet die Animation von `start` nach `end` |
| `setCurrentValue(value)` | Setzt Start und Ende auf denselben Wert (kein Übergang) |

---

### `SelectionControl`

Abstrakte Basisklasse, die `QAbstractButton` erweitert. Bietet Checkstate-Unterstützung und das Signal `stateChanged(int)`.

| Element | Beschreibung |
|---|---|
| `checkState()` | Gibt `Qt::CheckState` zurück |
| Signal `stateChanged(int)` | Wird ausgelöst, wenn sich der Zustand ändert |
| `toggle(Qt::CheckState)` | Abstrakt – wird von `Switch` implementiert |

---

### `Switch`

Konkrete Material-Design-Implementierung von `SelectionControl`.

#### Konstruktoren

```cpp
Switch(QWidget* parent = nullptr, QLabel* praefix = nullptr);
Switch(const QString& text, QWidget* parent = nullptr);
Switch(const QString& text, const QBrush&, QWidget* parent = nullptr);
```

Der optionale `praefix`-Parameter erlaubt das Setzen eines vorangestellten Labels außerhalb des Schalter-Widgets selbst.

#### Wichtige Methoden

| Methode | Beschreibung |
|---|---|
| `sizeHint()` | Gibt die bevorzugte Größe zurück |
| `paintEvent(QPaintEvent*)` | Zeichnet Track und Thumb mit Animationen |
| `resizeEvent(QResizeEvent*)` | Passt Shadow-Pixmap bei Größenänderung an |
| `toggle(Qt::CheckState)` | Startet Animationen beim Umschalten |

#### Konstanten

| Konstante | Wert | Beschreibung |
|---|---|---|
| `CORNER_RADIUS` | 8.0 | Eckenradius des Tracks |
| `THUMB_RADIUS` | 14.5 | Radius des Thumb-Kreises |
| `SHADOW_ELEVATION` | 2.0 | Schattenebene des Thumbs |

---

## Styling (`Style::Switch`)

Die visuelle Konfiguration erfolgt über die Struct `Style::Switch` in [style.h](../src/style.h):

| Eigenschaft | Standard | Beschreibung |
|---|---|---|
| `height` | 36 | Höhe des Widgets |
| `font` | `Roboto medium 13pt` | Schriftart für Label |
| `thumbOnBrush` | `cyan500 (#00bcd4)` | Farbe des Thumbs (aktiviert) |
| `trackOnBrush` | `cyan500` | Farbe des Tracks (aktiviert) |
| `thumbOffBrush` | `gray50 (#fafafa)` | Farbe des Thumbs (deaktiviert) |
| `trackOffBrush` | `black` | Farbe des Tracks (deaktiviert) |
| `thumbDisabled` | `gray400` | Farbe (gesperrt) |
| `thumbBrushAnimation` | Linear, 150 ms | Animation Thumb-Farbe |
| `trackBrushAnimation` | Linear, 150 ms | Animation Track-Farbe |
| `thumbPosAnimation` | InOutQuad, 150 ms | Animation Thumb-Position |

---

## Verwendungsbeispiel

```cpp
#include "switch.h"

// Einfacher Toggle-Schalter
Switch* sw = new Switch("Dark Mode", this);
sw->setChecked(false);

connect(sw, &Switch::stateChanged, this, [](int state) {
    if (state == Qt::Checked) {
        // Eingeschaltet
    }
});
```

---

## Abhängigkeiten

- Qt Widgets (`QtWidgets`)
- `style.h` (Styling-Konfiguration)
- Qt-interne Funktion `qt_blurImage` für den Schatten-Effekt

---

## Lizenz

GPL v3 – Copyright (C) 2018–2020 Iman Ahmadvand
