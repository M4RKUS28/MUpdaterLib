# MUpdater – Qt Update & UI Bibliothek

MUpdater ist eine Qt-Bibliothek, die häufig benötigte Komponenten für Qt-Desktop-Anwendungen bündelt:

- Automatisches Update-System (via Qt Maintenance Tool)
- Fertiger „Über"-Dialog
- Style- und Theme-Verwaltung
- Material-Design-Toggle-Schalter
- Single-Instance-Absicherung

---

## Komponenten

| Komponente | Klasse | Beschreibung |
|---|---|---|
| [MUpdater](docs/mupdater.md) | `MUpdater` | Prüft auf Updates und startet das Maintenance Tool |
| [Über-Dialog](docs/dialogueber.md) | `DialogUeber` | Kombinierter About-Dialog mit Update- und Style-Verwaltung |
| [StyleHandler](docs/stylehandler.md) | `StyleHandler` | Verwaltet Qt-Styles, Dark-Themes und QSS-Stylesheets |
| [WakeUpListener](docs/wakeuplistener.md) | `WakeUpListener` | Verhindert mehrfaches Starten derselben Anwendung |
| [Switch](docs/switch.md) | `Switch` | Material-Design-Toggle-Schalter für Qt Widgets |
| [FlowLayout](docs/flowlayout.md) | `FlowLayout` | Layout mit automatischem Zeilenumbruch (Fließtext-Stil) |

---

## Schnellstart

### 1. Bibliothek einbinden

Kopiere den `src/`-Ordner in dein Projekt und füge die gewünschten `.h`/`.cpp`-Dateien deiner `.pro`-Datei hinzu:

```pro
SOURCES += \
    src/mupdater.cpp \
    src/stylehandler.cpp \
    src/dialogueber.cpp \
    src/switch.cpp \
    src/wakeuplistener.cpp \
    src/flowlayout.cpp

HEADERS += \
    src/mupdater.h \
    src/stylehandler.h \
    src/dialogueber.h \
    src/switch.h \
    src/wakeuplistener.h \
    src/style.h \
    src/flowlayout.h

FORMS += \
    src/dialogueber.ui

RESOURCES += \
    src/res/ressources.qrc \
    src/res/themes_icon.qrc \
    src/res/darkstyleicons.qrc
```

---

### 2. Single-Instance und Update-Dialog einrichten

```cpp
// main.cpp
#include "wakeuplistener.h"
#include "dialogueber.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setOrganizationName("MeineOrganisation");
    app.setApplicationName("MeineApp");

    MainWindow w;

    // Einzelinstanz sicherstellen
    WakeUpListener listener(&w, "MeineApp");

    w.show();
    return app.exec();
}
```

```cpp
// In einem Menü-Action-Handler oder Button-Slot:
DialogUeber *dlg = new DialogUeber(
    "maintenancetool.exe",  // Pfad zum Maintenance Tool
    "MeineOrganisation",
    "MeineApp",
    "1.0.0",
    QColor("#00bcd4"),       // Akzentfarbe
    this
);
dlg->setPixmap(":/icons/app_icon.png");
dlg->setDescription("https://meine-website.de", "Beschreibung der App.");
dlg->exec();
```

---

### 3. Style-Toggle-Schalter verwenden

```cpp
#include "switch.h"

Switch *sw = new Switch("Dark Mode aktivieren", this);
connect(sw, &Switch::stateChanged, this, [](int state) {
    if (state == Qt::Checked)
        qDebug() << "Dark Mode an";
});
```

---

## Bibliotheksversion

Die aktuelle Version ist in [mupdater.h](src/mupdater.h) definiert:

```cpp
#define MUPDATER_majorVersion 1
#define MUPDATER_minorVersion 1
#define MUPDATER_minorMinorVersion 0
```

---

## Voraussetzungen

- Qt 6.x (Qt Widgets, Qt Network, Qt Core)
- Qt Installer Framework (für Update-Funktionalität)
- C++17

---

## Dokumentation

| Dokument | Inhalt |
|---|---|
| [docs/mupdater.md](docs/mupdater.md) | Update-System |
| [docs/dialogueber.md](docs/dialogueber.md) | Über-Dialog |
| [docs/stylehandler.md](docs/stylehandler.md) | Style-Verwaltung |
| [docs/wakeuplistener.md](docs/wakeuplistener.md) | Single-Instance |
| [docs/switch.md](docs/switch.md) | Toggle-Schalter |
| [docs/flowlayout.md](docs/flowlayout.md) | Fließendes Layout |

---

## Lizenz

Die `Switch`-Komponente basiert auf einer Implementierung von **Iman Ahmadvand** (Copyright 2018–2020) und steht unter der **GNU General Public License v3**.

Alle weiteren Komponenten: siehe Projektlizenz.
