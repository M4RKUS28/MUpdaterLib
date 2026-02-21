# DialogUeber – Über-Dialog

## Übersicht

`DialogUeber` ist ein vollständiger „Über"-Dialog für Qt-Anwendungen. Er kombiniert Anwendungsinformationen (Version, Beschreibung, Lizenz, Mitwirkende), einen integrierten Updater (`MUpdater`) sowie einen Style-Handler (`StyleHandler`) in einem einzigen, wiederverwendbaren `QDialog`.

---

## Klasse `DialogUeber`

```cpp
#include "dialogueber.h"
```

Erbt von: `QDialog`

---

## Konstruktor

```cpp
DialogUeber(
    const QString &maintananceToolPath,
    const QString &organisation,
    const QString &application,
    const QString &version,
    const QColor  &color,
    QWidget       *parent = nullptr,
    const bool    parentCouldBeInvisSoDontExitIfUpdterNotifierIsClosed = false,
    const bool    autoHideUpdterPart = false,
    bool          enaibleStyleSHeetThemes = true,
    QStringList   filter = QStringList()
);
```

### Parameter

| Parameter | Beschreibung |
|---|---|
| `maintananceToolPath` | Pfad zum Qt Maintenance Tool (z. B. `maintenancetool.exe`) |
| `organisation` | Organisationsname (für `QSettings`) |
| `application` | Anwendungsname (für Titel und `QSettings`) |
| `version` | Versionsstring, z. B. `"1.2.3"` |
| `color` | Akzentfarbe für Rahmen und Linie im Dialog |
| `parent` | Eltern-Widget (kann `nullptr` sein) |
| `parentCouldBeInvisSoDontExitIfUpdterNotifierIsClosed` | Verhindert, dass die App schließt, wenn der Updater-Hinweis geschlossen wird |
| `autoHideUpdterPart` | Blendet den Updater-Bereich automatisch aus, wenn kein Tool vorhanden ist |
| `enaibleStyleSHeetThemes` | Aktiviert QSS-basierte Themes im `StyleHandler` |
| `filter` | Liste von Styles, die aus der Style-Auswahl herausgefiltert werden |

---

## Öffentliche Methoden

### Inhaltseinstellungen

| Methode | Beschreibung |
|---|---|
| `setDescription(url, text, altName)` | Setzt Beschreibungstext und Website-Link |
| `setDescription(url, QFile, altName)` | Liest Beschreibung aus einer Datei |
| `setContributorList(QStringList)` | Befüllt die Mitwirkenden-Liste |
| `setPixmap(QPixmap)` | Setzt das Anwendungslogo |
| `setPixmap(QString iconFilePath)` | Setzt das Logo aus einem Dateipfad |
| `setLicence(QFile, bool center_with_html)` | Setzt Lizenztext aus Datei |
| `setLicence(QString, bool center_with_html)` | Setzt Lizenztext direkt als String |
| `setIssueWebsite(QString url)` | Setzt die URL zur Fehlermeldeseite |
| `setUpdaterFinishedMsgBoxFilePath(QString path)` | Pfad zu einer Datei, deren Inhalt nach erfolgreichem Update als MessageBox angezeigt wird |

### Zugriff auf Unterobjekte

| Methode | Rückgabe | Beschreibung |
|---|---|---|
| `updater()` | `MUpdater*` | Gibt den internen Updater zurück |
| `styleHandler()` | `StyleHandler*` | Gibt den internen StyleHandler zurück |

---

## Tab-Struktur des Dialogs

Der Dialog enthält ein `QTabWidget` mit folgenden Tabs (nur sichtbar, wenn entsprechende Inhalte gesetzt wurden):

| Tab-Index | Inhalt |
|---|---|
| 0 | Beschreibung / Website |
| 1 | Mitwirkende |
| 2 | Lizenz |
| 3 | Updates / Updater-Status *(immer sichtbar)* |
| 4 | Style-Auswahl |

---

## Signale & Slots (intern)

| Slot | Beschreibung |
|---|---|
| `updaterStatusChanged()` | Reagiert auf Statusänderungen des `MUpdater` |
| `on_pushButtonUpdaterButton_clicked()` | Startet Update oder prüft auf Updates |
| `on_pushButtonClose_clicked()` | Schließt den Dialog |
| `onActionAutoUSearchClicked()` | Schaltet automatische Updatesuche um |
| `onActionStartMaintanaceToolCliecked()` | Startet das Maintenance Tool direkt |
| `on_pushButtonMenueAutoUpdate_clicked()` | Öffnet das Kontext-Menü für Updateoptionen |

---

## Verwendungsbeispiel

```cpp
#include "dialogueber.h"

DialogUeber *dlg = new DialogUeber(
    "maintenancetool.exe",
    "MeineOrganisation",
    "MeineApp",
    "2.0.0",
    QColor("#00bcd4"),
    this
);

dlg->setPixmap(":/icons/app_logo.png");
dlg->setDescription("https://meine-website.de", "Eine tolle Anwendung.");
dlg->setLicence(QFile(":/LICENSE"), true);
dlg->setContributorList({"Max Mustermann", "Erika Musterfrau"});
dlg->exec();
```

---

## Abhängigkeiten

- [MUpdater](mupdater.md) – Automatisches Update-System
- [StyleHandler](stylehandler.md) – Style- und Theme-Verwaltung
- `dialogueber.ui` – Qt Designer UI-Datei
- Qt Widgets, QSettings, QMenu, QFile
