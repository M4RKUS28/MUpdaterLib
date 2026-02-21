# WakeUpListener – Single-Instance-Absicherung

## Übersicht

`WakeUpListener` verhindert, dass eine Anwendung mehrfach gleichzeitig gestartet wird. Beim zweiten Start schickt die neue Instanz eine Nachricht an die bereits laufende Instanz, die daraufhin ihr Hauptfenster in den Vordergrund bringt. Die neue Instanz beendet sich dann selbst.

Die Kommunikation erfolgt über **Qt Local Sockets** (`QLocalServer` / `QLocalSocket`), sodass keine Netzwerkverbindung benötigt wird.

---

## Klasse `WakeUpListener`

```cpp
#include "wakeuplistener.h"
```

Erbt von: `QObject`

Die gesamte Implementierung befindet sich im Header ([wakeuplistener.h](../src/wakeuplistener.h)) – es gibt keine separate `.cpp`-Datei.

---

## Konstruktor

```cpp
WakeUpListener(QWidget *widget, QString appName, QObject *parent = nullptr);
```

| Parameter | Beschreibung |
|---|---|
| `widget` | Das Hauptfenster der Anwendung, das in den Vordergrund gebracht werden soll |
| `appName` | Eindeutiger Name der Anwendung – wird als Socket-Name verwendet (`appName + "Server"`) |
| `parent` | Eltern-QObject (optional) |

Der Konstruktor führt beim Aufruf sofort folgende Prüfung durch:

1. **Verbindung möglich** → Es läuft bereits eine Instanz:
   - Sendet `"BringToFront"` an die laufende Instanz
   - Wartet auf Bestätigung (`"SUCCESS"`)
   - Ruft `exit(0)` auf – die neue Instanz beendet sich
2. **Verbindung nicht möglich** → Diese Instanz ist die erste:
   - Erstellt einen `QLocalServer` und wartet auf eingehende Verbindungen

---

## Destruktor

```cpp
~WakeUpListener();
```

Trennt den Server und gibt ihn frei.

---

## Interner Ablauf

```
Neue Instanz startet
       │
       ▼
Verbindung zu "<appName>Server" versuchen
       │
   ┌───┴───┐
Erfolg    Fehler
   │         │
   │         └─▶ QLocalServer starten
   │              └─▶ Auf "BringToFront" warten
   │                   └─▶ widget->raise(), show()
   │                        └─▶ "SUCCESS" senden
   ▼
"BringToFront" senden
       │
   Antwort "SUCCESS"?
   ├── Ja  → exit(0)
   └── Nein → Warnung anzeigen
```

---

## Fehlerverhalten

| Situation | Verhalten |
|---|---|
| Antwort != `"SUCCESS"` | `QMessageBox::warning` mit Hinweis auf falsche Antwort |
| Keine Antwort innerhalb 500 ms | `QMessageBox::warning` – Bestätigung nicht erhalten |
| Bytes nicht geschrieben | `qDebug()` Fehlermeldung |

---

## Verwendungsbeispiel

```cpp
// main.cpp
#include "wakeuplistener.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow w;

    // Muss vor w.show() erstellt werden
    WakeUpListener listener(&w, "MeineAppName");

    w.show();
    return app.exec();
}
```

> **Hinweis:** Der `WakeUpListener` muss vor dem ersten `show()`-Aufruf des Hauptfensters instanziiert werden, damit die Einzelinstanz-Prüfung korrekt funktioniert.

---

## Abhängigkeiten

- `QLocalServer` / `QLocalSocket` (Qt Network)
- `QWidget`, `QObject`, `QMessageBox` (Qt Widgets)
