# MUpdater – Automatisches Update-System

## Übersicht

`MUpdater` ist eine Qt-Klasse, die Anwendungs-Updates über das **Qt Installer Framework / Maintenance Tool** verwaltet. Sie prüft automatisch beim Start auf Updates, zeigt bei Bedarf einen Hinweisdialog an und startet das Maintenance Tool zum Installieren der Updates.

Aktuelle Bibliotheksversion: `1.1.0`

---

## Versionsmakros

```cpp
#define MUPDATER_majorVersion 1
#define MUPDATER_minorVersion 1
#define MUPDATER_minorMinorVersion 0
```

Versionskompatibilität: Minor-Versionen sind abwärtskompatibel, Major-Versionen nicht.

---

## Klasse `MUpdater`

```cpp
#include "mupdater.h"
```

Erbt von: `QObject`

---

## Status-Enum `UPDATE_STATUS`

| Wert | Bedeutung |
|---|---|
| `NOT_CHECKED` | Noch nicht auf Updates geprüft |
| `NO_UPDATER` | Kein Maintenance Tool gefunden / Webversion |
| `CHECKING` | Update-Prüfung läuft gerade |
| `UP_TO_DATE` | Anwendung ist aktuell |
| `UPDTAE_NEEDED` | Update verfügbar |
| `UPDATING` | Update wird gerade installiert |
| `UPDATE_ERROR` | Ein Fehler ist aufgetreten |
| `UPDATE_FINISHED` | Update abgeschlossen – Neustart erforderlich |

---

## Konstruktor

```cpp
MUpdater(
    QString maintananceToolPath,
    QString organisation,
    QString application,
    bool    doAutoUpdateIfEnabled = true
);
```

| Parameter | Beschreibung |
|---|---|
| `maintananceToolPath` | Pfad zum `maintenancetool.exe` / `maintenancetool` |
| `organisation` | Organisationsname für `QSettings` |
| `application` | Anwendungsname für `QSettings` |
| `doAutoUpdateIfEnabled` | Führt automatisch eine Updateprüfung beim Start durch, wenn diese Option in den Einstellungen aktiviert ist |

---

## Getter-Methoden

| Methode | Rückgabe | Beschreibung |
|---|---|---|
| `getStatus()` | `UPDATE_STATUS` | Aktueller Update-Status |
| `getStatusStr()` | `QString` | Lesbarer Statustext (auf Deutsch) |
| `getNewVersion()` | `QString` | Neue Versionsnummer, wenn Update verfügbar |
| `getError()` | `QString` | Fehlermeldung im Fehlerfall |
| `getExtraErrorInfo()` | `QString` | Detailliertes Log des Maintenance Tools |
| `getAutoSearchForUpdateStatus()` | `bool` | `true`, wenn automatische Suche aktiviert ist |
| `getMaintananceToolPath()` | `QString` | Pfad zum Maintenance Tool |
| `getVersion()` | `QString` | Version der MUpdater-Bibliothek als String |
| `getMajorVersion()` | `int` | Major-Version |
| `getMinorVersion()` | `int` | Minor-Version |
| `getPatchVersion()` | `int` | Patch-Version |

---

## Setter-Methoden

| Methode | Beschreibung |
|---|---|
| `setAutoSearchForUpdate(bool)` | Aktiviert / deaktiviert die automatische Updatesuche (gespeichert in `QSettings`) |

---

## Haupt-Methoden

| Methode | Rückgabe | Beschreibung |
|---|---|---|
| `checkForUpdates(bool showMsgBox, bool onlyIfUpdateAvail)` | `bool` | Prüft auf Updates; zeigt Hinweisdialog wenn nötig |
| `showUpdateMessageBox()` | `bool` | Zeigt den Update-Hinweisdialog explizit an |
| `startUpdate()` | `bool` | Startet das Maintenance Tool, wenn Status `UPDTAE_NEEDED` ist |
| `resetAll()` | `bool` | Stoppt alle Prozesse und setzt den Status zurück auf `NOT_CHECKED` |
| `restartApp()` | `void` | Startet die Anwendung neu |

---

## Signale & Slots

### Signal

| Signal | Beschreibung |
|---|---|
| `statusChanged()` | Wird ausgelöst, wenn sich `UPDATE_STATUS` ändert |

### Slots

| Slot | Beschreibung |
|---|---|
| `updateDialogButtonClicked(QAbstractButton*)` | Verarbeitet Button-Klicks im Update-Hinweisdialog |

---

## Automatische Update-Prüfung beim Start

Wenn `doAutoUpdateIfEnabled = true` und `getAutoSearchForUpdateStatus() == true`, startet `MUpdater` intern einen Timer, der bis zu **4 Mal alle 15 Sekunden** versucht, einen Update-Check durchzuführen. Er stoppt automatisch, sobald `UP_TO_DATE` oder `UPDTAE_NEEDED` erreicht wird.

---

## Plattform-Unterstützung

Für Web-Builds (`Q_OS_WEB`) entfällt die Prozesssteuerung – der Status wird direkt auf `NO_UPDATER` gesetzt.

---

## Verwendungsbeispiel

```cpp
#include "mupdater.h"

MUpdater *updater = new MUpdater("maintenancetool.exe", "MeineOrg", "MeineApp");

connect(updater, &MUpdater::statusChanged, this, [updater]() {
    if (updater->getStatus() == MUpdater::UPDTAE_NEEDED) {
        updater->startUpdate();
    }
});
```

---

## Abhängigkeiten

- Qt Core (`QObject`, `QProcess`, `QSettings`, `QTimer`)
- Qt Widgets (`QMessageBox`)
- Qt Installer Framework / Maintenance Tool
