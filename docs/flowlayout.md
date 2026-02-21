# FlowLayout – Fließendes Layout

## Übersicht

`FlowLayout` ist ein benutzerdefiniertes Qt-Layout, das Widgets **wie Wörter in einem Fließtext** anordnet: Elemente werden von links nach rechts nebeneinander platziert. Sobald der horizontale Platz nicht mehr ausreicht, wird automatisch in die **nächste Zeile umgebrochen**.

Es basiert auf dem gleichnamigen offiziellen Qt-Beispiel und ergänzt die Standard-Layouts (`QHBoxLayout`, `QVBoxLayout`, `QGridLayout`), die kein automatisches Umbrechen unterstützen.

---

## Klasse `FlowLayout`

```cpp
#include "flowlayout.h"
```

Erbt von: `QLayout`

---

## Konstruktoren

```cpp
// Mit übergeordnetem Widget
FlowLayout(QWidget *parent, int margin = -1, int hSpacing = -1, int vSpacing = -1);

// Ohne übergeordnetes Widget (z. B. für späteres setLayout())
FlowLayout(int margin = -1, int hSpacing = -1, int vSpacing = -1);
```

| Parameter | Beschreibung |
|---|---|
| `parent` | Übergeordnetes Widget |
| `margin` | Innenabstand ringsum in Pixeln; `-1` = Systemstandard |
| `hSpacing` | Horizontaler Abstand zwischen Widgets; `-1` = Systemstandard |
| `vSpacing` | Vertikaler Abstand zwischen Zeilen; `-1` = Systemstandard |

---

## Öffentliche Methoden

| Methode | Rückgabe | Beschreibung |
|---|---|---|
| `addItem(QLayoutItem*)` | `void` | Fügt ein Layout-Item hinzu |
| `count()` | `int` | Anzahl der enthaltenen Items |
| `itemAt(int index)` | `QLayoutItem*` | Gibt Item an Position zurück |
| `takeAt(int index)` | `QLayoutItem*` | Entfernt und gibt Item zurück |
| `horizontalSpacing()` | `int` | Horizontaler Abstand (oder Systemstandard) |
| `verticalSpacing()` | `int` | Vertikaler Abstand (oder Systemstandard) |
| `hasHeightForWidth()` | `bool` | Gibt `true` zurück – Höhe hängt von der Breite ab |
| `heightForWidth(int width)` | `int` | Berechnet benötigte Höhe für gegebene Breite |
| `sizeHint()` | `QSize` | Entspricht `minimumSize()` |
| `minimumSize()` | `QSize` | Kleinste mögliche Größe (größtes enthaltenes Widget) |
| `expandingDirections()` | `Qt::Orientations` | Keine expandierende Richtung |
| `setGeometry(QRect)` | `void` | Ordnet alle Widgets gemäß Fließlayout an |

---

## Funktionsweise

Das interne `doLayout()` iteriert über alle enthaltenen Widgets und platziert sie nacheinander:

1. Widget wird an aktueller `x`-Position platziert.
2. `x` wird um `Widget-Breite + horizontalSpacing` vorgeschoben.
3. Überschreitet `x` den rechten Rand, wird ein **Zeilenumbruch** ausgeführt:
   - `x` zurück auf Anfang
   - `y` um die Höhe der letzten Zeile + `verticalSpacing` erhöht

---

## Typische Anwendungsfälle

- **Contributor-Listen** im Über-Dialog (variabler Inhalt)
- **Tag-/Chip-Leisten** (z. B. Schlagwörter, Filter)
- **Symbolleisten** mit vielen Buttons
- Jede Sammlung von Widgets, deren Anzahl zur Laufzeit variiert

---

## Verwendungsbeispiel

```cpp
#include "flowlayout.h"

FlowLayout *layout = new FlowLayout(this, /*margin=*/10, /*hSpacing=*/5, /*vSpacing=*/5);

for (const QString &name : contributors) {
    layout->addWidget(new QLabel(name, this));
}

ui->contributorsWidget->setLayout(layout);
```

Das Layout passt die Höhe automatisch an, wenn sich die Fensterbreite ändert.

---

## Abhängigkeiten

- Qt Widgets (`QLayout`, `QLayoutItem`, `QWidget`, `QStyle`)
