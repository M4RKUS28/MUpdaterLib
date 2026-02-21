#ifndef DIALOGUEBER_H
#define DIALOGUEBER_H

#include "mupdater.h"
#include "stylehandler.h"
#include <QAction>
#include <QDialog>
#include <QFile>
#include <QMenu>

namespace Ui {
class DialogUeber;
}

/**
 * @brief About-Dialog mit integriertem Updater und Theme-Auswahl.
 *
 * Zeigt Informationen über die Anwendung (Version, Beschreibung, Lizenz,
 * Mitwirkende, Issue-Tracker) und bietet Update-Verwaltung über MUpdater
 * sowie Theme-Umschaltung über StyleHandler an.
 *
 * Typische Verwendung:
 * @code
 *   DialogUeber dlg("/path/to/MaintenanceTool", "MyOrg", "MyApp",
 *                   "1.0.0", QColor("#3daee9"), this);
 *   dlg.setDescription("https://example.com", "App-Beschreibung");
 *   dlg.setLicence(QFile(":/LICENSE"));
 *   dlg.exec();
 * @endcode
 */
class DialogUeber : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Erstellt den About-Dialog.
     * @param maintenanceToolPath    Absoluter Pfad zum Qt Installer Framework
     *                               MaintenanceTool.
     * @param organisation           Organisations-/Firmenname (für QSettings).
     * @param application            Anwendungsname (wird in Titeln angezeigt).
     * @param version                Aktuelle Versionsnummer als String.
     * @param color                  Akzentfarbe für Rahmen und Links.
     * @param parent                 Eltern-Widget (optional).
     * @param preventExitOnClose     Wenn true: verhindert, dass das Schließen des
     *                               Update-Benachrichtigungsfensters die Anwendung
     *                               beendet (nötig wenn das Parent unsichtbar ist).
     * @param autoHideUpdaterPart    Versteckt Update-Schaltflächen wenn kein
     *                               MaintenanceTool vorhanden ist.
     * @param enableStyleSheetThemes Aktiviert QSS-basierte Themes (QDarkStyle).
     * @param filter                 Whitelist der angezeigten Themes; leer = alle.
     */
    explicit DialogUeber(const QString &maintenanceToolPath,
                         const QString &organisation,
                         const QString &application,
                         const QString &version,
                         const QColor  &color,
                         QWidget       *parent                = nullptr,
                         bool           preventExitOnClose    = false,
                         bool           autoHideUpdaterPart   = false,
                         bool           enableStyleSheetThemes = true,
                         QStringList    filter                = QStringList());

    ~DialogUeber();

    // ── Inhalt befüllen ────────────────────────────────────────────────────

    /**
     * @brief Setzt Beschreibungstext und optionalen Website-Link.
     * @param websiteUrl         URL die als Link angezeigt wird.
     * @param description        HTML- oder Klartext-Beschreibung.
     * @param alternativeUrlName Anzeigetext für den Link (Standard: URL selbst).
     */
    void setDescription(const QString &websiteUrl,
                        const QString &description,
                        const QString &alternativeUrlName = "");

    /** @overload Liest den Beschreibungstext aus einer Datei. */
    void setDescription(const QString &websiteUrl,
                        QFile          description,
                        const QString &alternativeUrlName = "");

    /**
     * @brief Befüllt den Mitwirkenden-Tab.
     * @param contributors Liste der Namen.
     */
    void setContributorList(const QStringList &contributors);

    /** @brief Setzt das Anwendungs-Icon im Dialog. */
    void setPixmap(const QPixmap &icon);

    /** @overload Lädt das Icon aus einem Dateipfad. */
    void setPixmap(const QString &iconFilePath);

    /**
     * @brief Zeigt eine Lizenz im Lizenz-Tab an.
     * @param licence        Lizenztext als Datei.
     * @param centerWithHtml Wenn true: Text wird zentriert als HTML formatiert.
     */
    void setLicence(QFile licence, bool centerWithHtml = true);

    /** @overload Nimmt den Lizenztext direkt als String. */
    void setLicence(QString licence, bool centerWithHtml = true);

    /**
     * @brief Zeigt einen Issue-Tracker-Link im Dialog an.
     * @param url Vollständige URL zum Bug-Tracker.
     */
    void setIssueWebsite(const QString &url);

    /**
     * @brief Pfad zu einer Textdatei die nach einem abgeschlossenen Update
     *        als Hinweisfenster angezeigt wird.
     * @param path Absoluter Pfad zur Info-Datei.
     */
    void setUpdaterFinishedMsgBoxFilePath(const QString &path);

    // ── Zugriff auf Unterkomponenten ───────────────────────────────────────

    /** @brief Gibt den internen MUpdater zurück (für direkte Steuerung). */
    MUpdater     *updater();

    /** @brief Gibt den internen StyleHandler zurück (für Theme-Zugriff). */
    StyleHandler *styleHandler();

private slots:
    /** Reagiert auf MUpdater::statusChanged() und aktualisiert alle UI-Elemente. */
    void updaterStatusChanged();

    /** Führt je nach Update-Status die passende Aktion aus
     *  (Prüfen / Installieren / Neustart). */
    void on_pushButtonUpdaterButton_clicked();

    void on_pushButtonClose_clicked();
    void on_pushButtonMenueAutoUpdate_clicked();

    /** Speichert die Einstellung "Automatisch nach Updates suchen". */
    void onActionAutoUpdateSearchClicked();

    /** Startet das MaintenanceTool direkt (z.B. zum Deinstallieren). */
    void onActionStartMaintenanceToolClicked();

private:
    /** @brief Hängt @p msg an das interne Fehlerlog an und
     *         macht den Log-Tab sichtbar. */
    void setUpdateErrorLog(const QString &msg);

    // ── UI ─────────────────────────────────────────────────────────────────
    Ui::DialogUeber *ui;

    QMenu   *m_autoUpdaterMenu;           ///< Dropdown-Menü am Updater-Button
    QAction *m_actionAutoUpdate;          ///< "Automatisch nach Updates suchen"
    QAction *m_actionStartMaintenance;    ///< "MaintenanceTool starten" (optional)

    // ── Kernkomponenten ────────────────────────────────────────────────────
    MUpdater     *m_updater;
    StyleHandler *m_styleHandler;

    // ── Konfiguration ──────────────────────────────────────────────────────
    QString m_version;               ///< Gespeicherte Versionsnummer
    QColor  m_color;                 ///< Akzentfarbe für Rahmen/Links
    QString m_msgBoxFilePath;        ///< Pfad zur Update-Info-Datei
    bool    m_autoHideUpdaterPart;   ///< Update-UI ausblenden wenn kein Tool
};






#endif // DIALOGUEBER_H
