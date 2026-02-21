#ifndef STYLEHANDLER_H
#define STYLEHANDLER_H

/**
 * @file stylehandler.h
 * @brief Verwaltet Qt-Styles und optionale QSS-Themes für die gesamte Anwendung.
 *
 * StyleHandler speichert den aktuell gewählten Style in QSettings und
 * stellt eine QComboBox bereit über die der Benutzer Themes umschalten kann.
 *
 * Unterstützte Styles:
 * - Alle Qt-nativen Styles (QStyleFactory::keys())
 * - Eigene Fusion-Paletten: Fusion_OWN_dark_gray, Fusion_OWN_dark_blue,
 *   Fusion_OWN_dark_blue2
 * - QSS-basiert (optional): QDarkStyleDark, QDarkStyleLight
 */

#include <QObject>
#include <QSettings>
#include <QStyleFactory>
#include <QStyle>
#include <QApplication>
#include <QComboBox>
#include <QMutex>
#include <QStringList>
#include <QStyleHints>
#include <QPalette>


/**
 * @brief Verwaltet Qt-Styles und Theme-Persistenz.
 */
class StyleHandler : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Konstruktor.
     * @param organisation          Organisations-Name (für QSettings).
     * @param application           Anwendungs-Name (für QSettings).
     * @param enableStyleSheetThemes  Aktiviert QSS-basierte Themes (QDarkStyle).
     *                              Ohne diesen Parameter sind nur Qt-native
     *                              und Fusion-Paletten-Themes verfügbar.
     * @param filteredStyles        Whitelist der sichtbaren Styles; leer = alle.
     */
    StyleHandler(const QString &organisation, const QString &application,
                 const bool enableStyleSheetThemes = false,
                 QStringList filteredStyles = QStringList());
    ~StyleHandler();

    /**
     * @brief Setzt den aktiven Style und speichert ihn in QSettings.
     * @param style  Style-ID (z.B. "Fusion", "Fusion_OWN_dark_gray", "QDarkStyleDark").
     * @return false wenn der Style unbekannt ist oder die QSS-Datei nicht geöffnet
     *         werden konnte.
     */
    bool setStyle(const QString &style);

    /** @brief Gibt die Liste aller verfügbaren Styles zurück
     *         (Qt-native + eigene, gefiltert via filteredStyles). */
    QStringList getStyles();

    /**
     * @brief Gibt die (einmalig erstellte) Theme-ComboBox zurück.
     *
     * Die Box ist mit setStyle() verbunden und spiegelt immer den
     * aktuell aktiven Style wider. Thread-safe (QMutexLocker).
     */
    QComboBox *getCombobox();

    /** @brief Versionsnummer des MUpdater-Moduls als String. */
    QString getVersion();

    /**
     * @brief Gibt die Theme-Map zur Anpassung der Anzeigenamen und Icons zurück.
     *
     * Format: @code themeMap["Fusion"] = {"Anzeigename", {"hell-icon.png", "dunkel-icon.png"}}; @endcode
     *
     * Vorbelegte Einträge: Fusion, windowsvista, Windows, Fusion_OWN_dark_gray,
     * Fusion_OWN_dark_blue, Fusion_OWN_dark_blue2, QDarkStyleDark, QDarkStyleLight.
     */
    QMap<QString, QPair<QString, QPair<QString, QString>>> &getThemeMap();

    /**
     * @brief Aktiviert/deaktiviert den Autostart der Anwendung.
     * @note  Nur auf Windows unterstützt (schreibt in HKCU\\...\\Run).
     *        Auf anderen Plattformen wird false zurückgegeben.
     * @return false bei Fehler oder nicht unterstützter Plattform.
     */
    bool setAutoStart(bool enabled);

    /** @brief Gibt true zurück wenn QSS-basierte Themes aktiviert sind. */
    bool styleSheetsEnabled() const;

    /** @brief Aktiviert/deaktiviert QSS-basierte Themes zur Laufzeit. */
    void setStyleSheetEnabled(bool enabled);

    /** @brief Gibt true zurück wenn der aktuelle Style QSS-Stylesheets verwendet. */
    bool currentStyleUsesStyleSheets();

    /** @brief Gibt das aktuelle Farbschema zurück (Hell/Dunkel). */
    Qt::ColorScheme getCurrentColorTheme();

    /** @brief Gibt die aktuell eingestellte Style-Whitelist zurück. */
    QStringList getFilteredStyles() const;

    /** @brief Setzt eine neue Style-Whitelist (leer = alle Styles zeigen). */
    void setFilteredStyles(const QStringList &newFilteredStyles);

    /** @brief Gibt den Organisations-Namen zurück. */
    QString getOrganisation() const;

signals:
    /** @brief Wird emittiert nachdem ein neuer Style erfolgreich gesetzt wurde. */
    void currentStyleChanged();

private slots:
    void indexChanged(int i);    ///< Reagiert auf ComboBox-Änderung
    void updateStyleList();      ///< Aktualisiert den Inhalt der ComboBox
    QString getCurrentStyle();   ///< Liest aktuellen Style aus QSettings
    void colorSchemeChanged(Qt::ColorScheme colorScheme); ///< Reagiert auf System-Farbschema-Änderung

private:
    const QString m_organisation;            ///< Organisations-Name (QSettings)
    const QString m_application;             ///< Anwendungs-Name (QSettings)
    const QString ENTRY_NAME = "MUPDATER_STYLE"; ///< QSettings-Schlüssel
    QComboBox    *combobox;                  ///< Lazy-erstellte Theme-ComboBox
    QMutex        objMutex;                  ///< Schutz für getCombobox() (DCLP-Ersatz)
    QMap<QString, QPair<QString, QPair<QString, QString>>> themeMap; ///< ID → {Name, {hell-Icon, dunkel-Icon}}
    bool          m_enableStyleSheetThemes;  ///< QSS-Themes aktiviert?
    QStringList   filteredStyles;            ///< Whitelist (leer = alle Styles)

    /** @brief Aktualisiert die Icons in der ComboBox basierend auf dem aktuellen Farbschema. */
    void updateComboBoxIconColor();
};

#endif // STYLEHANDLER_H
