# StyleHandler – Style- und Theme-Verwaltung

## Übersicht

`StyleHandler` verwaltet das visuelle Erscheinungsbild einer Qt-Anwendung. Er unterstützt alle nativen Qt-Styles (über `QStyleFactory`), eigene Fusion-basierte Dark-Themes sowie optionale QSS-Stylesheets. Der aktuell gewählte Style wird persistent über `QSettings` gespeichert und beim nächsten Start automatisch wiederhergestellt. Optional kann ein vorkonfiguriertes `QComboBox`-Widget für die Benutzeroberfläche erstellt werden.

---

## Klasse `StyleHandler`

```cpp
#include "stylehandler.h"
```

Erbt von: `QObject`

---

## Konstruktor

```cpp
StyleHandler(
    const QString  &organisation,
    const QString  &application,
    const bool      enableStyleSheetThemes = false,
    QStringList     filteredStyles = QStringList()
);
```

| Parameter | Beschreibung |
|---|---|
| `organisation` | Organisationsname für `QSettings` |
| `application` | Anwendungsname für `QSettings` |
| `enableStyleSheetThemes` | Aktiviert QSS-basierte Themes (Dark/Light Stylesheets) |
| `filteredStyles` | Liste von Style-IDs, die nicht angeboten werden sollen |

---

## Öffentliche Methoden

### Style-Verwaltung

| Methode | Rückgabe | Beschreibung |
|---|---|---|
| `setStyle(QString style)` | `bool` | Setzt den Anwendungsstyle; speichert ihn in `QSettings` |
| `getStyles()` | `QStringList` | Gibt alle verfügbaren Styles zurück (Qt-Styles + eigene) |
| `currentStyleUsesStyleSheets()` | `bool` | `true`, wenn der aktuelle Style QSS verwendet |
| `getCurrentColorTheme()` | `Qt::ColorScheme` | Gibt das aktuelle Farbschema zurück (`Light` / `Dark`) |

### Theme-Map

```cpp
QMap<QString, QPair<QString, QPair<QString, QString>>> &getThemeMap();
```

Gibt die interne Theme-Map zurück. Jeder Eintrag hat folgende Struktur:

```
themeMap["styleID"] = { "Anzeigename", { "Icon-Hell", "Icon-Dunkel" } }
```

Vordefinierte Einträge:

| Style-ID | Anzeigename | Art |
|---|---|---|
| `windows11` | Windows 11 | Qt-Style |
| `Fusion` | Fusion | Qt-Style (Auto hell/dunkel) |
| `windowsvista` | Windows Classic | Qt-Style (Hell) |
| `Windows` | Windows Old | Qt-Style |
| `Fusion_OWN_dark_gray` | Fusion Gray | Eigener Dark-Style |
| `Fusion_OWN_dark_blue` | Fusion Blue | Eigener Dark-Style |
| `Fusion_OWN_dark_blue2` | Fusion LightBlue | Eigener Dark-Style |
| `QDarkStyleDark` | QDarkStyle | QSS Dark-Style |
| `QDarkStyleLight` | QDarkStyleLight | QSS Light-Style |

Die Map kann nach der Konstruktion angepasst werden, um Anzeigenamen oder Icons zu ändern.

### ComboBox

| Methode | Rückgabe | Beschreibung |
|---|---|---|
| `getCombobox()` | `QComboBox*` | Erstellt die Combo-Box beim ersten Aufruf; gibt danach dieselbe zurück |

Die ComboBox zeigt alle verfügbaren Styles mit passenden Icons an und wechselt den Style automatisch bei Auswahl.

### Weitere Getter / Setter

| Methode | Beschreibung |
|---|---|
| `getVersion()` | Gibt die MUpdater-Bibliotheksversion zurück |
| `getOrganisation()` | Gibt den Organisationsnamen zurück |
| `styleSheetsEnabled()` | Gibt zurück, ob QSS-Themes aktiviert sind |
| `setStyleSheetEnabled(bool)` | Aktiviert / deaktiviert QSS-Themes |
| `getFilteredStyles()` | Gibt die gefilterten Styles zurück |
| `setFilteredStyles(QStringList)` | Setzt die gefilterten Styles |
| `setAutoStart(bool)` | Aktiviert/deaktiviert Autostart der Anwendung |

---

## Signale

| Signal | Beschreibung |
|---|---|
| `currentStyleChanged()` | Wird ausgelöst, wenn sich der Style ändert |

---

## Eigene Dark-Themes (Fusion-basiert)

`StyleHandler` enthält drei eingebettete Farbpaletten:

### `Fusion_OWN_dark_gray`
Dunkles Grau-Theme – klassisches Dark Mode Layout.
- Fensterhintergrund: `#353535`
- Basis: `#191919`

### `Fusion_OWN_dark_blue`
Dunkles Blau-Theme – modernes dunkles UI.
- Fensterhintergrund: `#1e2025`
- Basis: `#1c1f2f`

### `Fusion_OWN_dark_blue2`
Blaues Dark-Theme – tiefes Blau.
- Fensterhintergrund: `#1c1f2f`
- Basis: `#262b3a`

---

## Automatische Anpassung bei Systemfarbschema-Änderung

`StyleHandler` reagiert auf `QStyleHints::colorSchemeChanged` und aktualisiert bei Fusion-Auto-Styles die Palette sowie die ComboBox-Icons automatisch.

---

## Verwendungsbeispiel

```cpp
#include "stylehandler.h"

StyleHandler *sh = new StyleHandler("MeineOrg", "MeineApp", true);

// Style programmatisch setzen
sh->setStyle("Fusion_OWN_dark_blue");

// ComboBox in UI einbinden
ui->someLayout->addWidget(sh->getCombobox());

// Auf Style-Änderungen reagieren
connect(sh, &StyleHandler::currentStyleChanged, this, []() {
    qDebug() << "Style wurde geändert!";
});
```

---

## Abhängigkeiten

- Qt Widgets (`QApplication`, `QComboBox`, `QStyleFactory`)
- Qt Core (`QSettings`, `QMutex`, `QStringList`)
- Ressourcendatei `themes_icon.qrc` (Icons für helle/dunkle Themes)
- Ressourcendatei `darkstyleicons.qrc` (Icons für QSS-Themes, nur wenn QSS aktiviert)
- [MUpdater](mupdater.md) (für `getVersion()`)
