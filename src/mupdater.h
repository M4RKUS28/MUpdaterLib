#ifndef UPDATER_H
#define UPDATER_H

/**
 * @file mupdater.h
 * @brief Automatischer Update-Manager auf Basis des Qt Installer Frameworks.
 *
 * MUpdater kapselt den gesamten Update-Lebenszyklus:
 *   - Prüfen ob eine neue Version verfügbar ist (via MaintenanceTool --check-updates)
 *   - Anzeigen eines Hinweisfensters
 *   - Starten des MaintenanceTool-Update-Modus
 *   - Neustart der Anwendung nach erfolgreichem Update
 *
 * Der Zustand wird als einfache State-Machine in @ref zustandWechseln() verwaltet.
 * Das Signal @ref statusChanged() wird bei jeder Zustandsänderung emittiert.
 */

#include <QMessageBox>
#include <QObject>
#include <QProcess>
#include <QSettings>
#include <QString>
#include <QTimerEvent>

// Versionsnummer von MUpdater selbst
// Minor-Versionen sind rückwärtskompatibel; Major-Änderungen sind breaking.
constexpr int MUPDATER_majorVersion      = 1;
constexpr int MUPDATER_minorVersion      = 1;
constexpr int MUPDATER_minorMinorVersion = 0;

/**
 * @brief Verwaltet Update-Prüfung und -Installation via Qt Installer Framework.
 *
 * Erstellt ein Objekt und übergibt den Pfad zum MaintenanceTool. Ist
 * @p doAutoUpdateIfEnabled true und die Option in den QSettings aktiviert,
 * wird nach einer kurzen Verzögerung automatisch nach Updates gesucht.
 *
 * @note Auf Web-Plattformen (Q_OS_WEB) ist der Updater deaktiviert
 *       und der Status wird sofort auf NO_UPDATER gesetzt.
 */
class MUpdater : public QObject
{
    Q_OBJECT
public:

    /**
     * @brief Update-Zustandsmaschine.
     *
     * Zustandsübergänge:
     * @code
     *  NOT_CHECKED  --checkForUpdates()--> CHECKING
     *  CHECKING     --finished(0)-------> UP_TO_DATE
     *  CHECKING     --finished(!=0)-----> UPDTAE_NEEDED  (Tippfehler per Design beibehalten)
     *  UPDTAE_NEEDED --startUpdate()----> UPDATING
     *  UPDATING     --finished(0)-------> UPDATE_FINISHED
     *  UPDATE_FINISHED --restartApp()---> (Neustart)
     *  any          --error occured----> UPDATE_ERROR
     *  any          --no tool found---->  NO_UPDATER
     * @endcode
     */
    enum UPDATE_STATUS {
        NOT_CHECKED     = 0, ///< Noch kein Update-Check durchgeführt
        NO_UPDATER      = 1, ///< Kein MaintenanceTool vorhanden (manuelle Installation)
        CHECKING        = 2, ///< Update-Check läuft
        UP_TO_DATE      = 3, ///< Anwendung ist aktuell
        UPDTAE_NEEDED   = 4, ///< Neue Version verfügbar (Schreibfehler absichtlich beibehalten)
        UPDATING        = 5, ///< Installation läuft
        UPDATE_ERROR    = 6, ///< Fehler aufgetreten (Details via getError() / getExtraErrorInfo())
        UPDATE_FINISHED = 7  ///< Installation abgeschlossen, Neustart erforderlich
    };

    /**
     * @brief Konstruktor.
     * @param maintananceToolPath    Absoluter Pfad zum Qt Installer Framework
     *                               MaintenanceTool (inkl. Dateiname).
     * @param organisation           Organisations-Name (für QSettings).
     * @param application            Anwendungs-Name (für QSettings und Anzeige).
     * @param doAutoUpdateIfEnabled  Wenn true: startet automatisch einen Update-Check
     *                               sofern die Option in QSettings aktiviert ist.
     */
    MUpdater(const QString &maintananceToolPath,
             const QString &organisation,
             const QString &application,
             bool doAutoUpdateIfEnabled = true);

    ~MUpdater();

    // ── Getter ──────────────────────────────────────────────────────────────

    /** @brief Gibt true zurück wenn der automatische Update-Check beim Start
     *         aktiviert ist (gespeichert über setAutoSearchForUpdate()). */
    bool getAutoSearchForUpdateStatus();

    /** @brief Neue Versionsnummer falls nach checkForUpdates() ein Update
     *         gefunden wurde, sonst leerer String. */
    QString getNewVersion();

    /** @brief Letzte Fehlermeldung (Kurzbeschreibung). */
    QString getError() const;

    /** @brief Aktueller Zustand der Update-State-Machine. */
    UPDATE_STATUS getStatus() const;

    /** @brief Lokalisierter Statustext für die Anzeige in der UI. */
    QString getStatusStr();

    /** @brief Ausführlicher Log-Output des MaintenanceTool-Prozesses. */
    QString getExtraErrorInfo() const;

    /** @brief Versionsnummer von MUpdater selbst als String. */
    QString getVersion();

    /** @brief Absoluter Pfad zum MaintenanceTool. */
    QString getMaintananceToolPath() const;

    /** @brief MUpdater Major-Version. */
    int getMajorVersion();
    /** @brief MUpdater Minor-Version (rückwärtskompatibel). */
    int getMinorVersion();
    /** @brief MUpdater Patch-Version. */
    int getPatchVersion();

    // ── Setter ──────────────────────────────────────────────────────────────

    /** @brief Aktiviert/deaktiviert den automatischen Update-Check beim
     *         nächsten Programmstart (gespeichert in QSettings). */
    void setAutoSearchForUpdate(const bool &enabled);

    // ── Hauptfunktionen ────────────────────────────────────────────────────

    /**
     * @brief Startet einen Update-Check.
     * @param showMessageBox       Zeigt ein Nachrichtenfenster wenn ein Update
     *                             verfügbar oder ein Fehler aufgetreten ist.
     * @param onlyIfUpdateAvailable  Bei showMessageBox=true: Fenster nur zeigen
     *                             wenn ein Update vorhanden ist.
     * @return false wenn der Status-Übergang nicht erlaubt war.
     */
    bool checkForUpdates(bool showMessageBox = false, bool onlyIfUpdateAvailable = true);

    /** @brief Zeigt das Update-Nachrichtenfenster (sofern noch nicht sichtbar). */
    bool showUpdateMessageBox();

    /** @brief Startet die Installation (nur wenn Status == UPDTAE_NEEDED). */
    bool startUpdate();

    /** @brief Bricht laufende Prozesse ab und setzt den Status zurück auf NOT_CHECKED. */
    bool resetAll();

    /** @brief Beendet die Anwendung und startet sie neu (nach einem Update). */
    void restartApp();

#ifndef Q_OS_WEB
private:
    QProcess updaterPrz;        ///< Prozess für --check-updates
    QProcess maintaneceToolPrz; ///< Prozess für --start-updater
#endif

signals:
    /** @brief Wird bei jeder Zustandsänderung emittiert. */
    void statusChanged();

public slots:
    /** @brief Reagiert auf den Klick im Update-Nachrichtenfenster. */
    void updateDialogButtonClicked(QAbstractButton *button);
#ifndef Q_OS_WEB
    /** @brief Slot: wird aufgerufen wenn der Check-Prozess abgeschlossen ist. */
    void onUpdateCheckFinished(int exitCode, QProcess::ExitStatus exitStatus);
    /** @brief Slot: wird aufgerufen wenn der Installations-Prozess abgeschlossen ist. */
    void onUpdateMaintanenceTollFinished(int exitCode, QProcess::ExitStatus exitStatus);
#endif

private:
    // ── State-Machine-Implementierung ────────────────────────────────────

    /**
     * @brief Zentrale Zustandsübergangs-Funktion.
     *
     * Wertet im aktuellen Status die übergebene @p action aus und ruft die
     * entsprechende interne do_*()-Methode auf. Emittiert bei gültigem
     * Übergang statusChanged().
     *
     * @param action  Name der Aktion als String (z.B. "checkForUpdates()").
     * @param value   Optionaler Wert (z.B. Exit-Code-Beschreibung).
     * @return false wenn der Übergang im aktuellen Zustand nicht erlaubt ist.
     */
    bool zustandWechseln(const QString &action, const QString &value = "");

    void setStatus(const UPDATE_STATUS &status,
                   const QString &error_msg     = "No Error",
                   const QString &extraErrorInfo = "");

    void do_checkForUpdates();
    void do_UpdateCheckFinished(const QString &value);
    void do_showUpdateMessageBox();
    bool do_showRestartMsgBox();
    void do_startUpdate();
    void do_updaterFinished(const QString &value);

    /** @brief Gibt eine lesbare Fehlerbeschreibung für einen QProcess::ProcessError-Wert. */
    QString getQProzessStartErrorStr(unsigned error);

    /** @brief Mappt den Exit-Code des MaintenanceTool auf einen lesbaren String. */
    QString getUpdterPackageManagerCoreStatusByExitCode(int exitcode, bool isUpdateCheck = true);

    // ── Member-Variablen ─────────────────────────────────────────────────

    UPDATE_STATUS  status;              ///< Aktueller Zustand
    QMessageBox   *updateMsgBox;        ///< Fenster "Update verfügbar" (heap, nullable)
    QMessageBox   *updateInfoMsgBox;    ///< Fortschritts-Info (heap, nullable)
    QString        newVersion;          ///< Gefundene neue Versionsnummer
    QString        error;               ///< Kurze Fehlerbeschreibung
    QString        extraErrorInfo;      ///< Ausführlicher Log-Output
    QString        organisation;        ///< QSettings-Schlüssel
    QString        application;         ///< Anwendungsname für Dialoge
    QString        maintananceToolPath; ///< Pfad zum Qt Installer Framework Tool
    bool           showMsgBox;          ///< Aufruf-Parameter von checkForUpdates()
    bool           onlyIfUpdateAvaible; ///< Aufruf-Parameter von checkForUpdates()

protected:
    int timerCounter; ///< Anzahl Timer-Versuche (max. 4)
    int timerId;      ///< Rückgabe von startTimer() zum späteren killTimer()
    void timerEvent(QTimerEvent *event) override;

private slots:
    /** @brief Einmalig um 1ms verzögert aufgerufen — startet den Auto-Update-Timer. */
    void startSearchForUpdatesAtStartTimer();
};

#endif // UPDATER_H







#endif // UPDATER_H
