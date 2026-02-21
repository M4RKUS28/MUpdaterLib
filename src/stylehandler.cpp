#include "stylehandler.h"
#include "mupdater.h"

#include <QDir>
#include <QFile>

StyleHandler::StyleHandler(const QString &organisation, const QString &application,
                           const bool enableStyleSheetThemes, QStringList filteredStyles)
    : organisation(organisation),
      application(application),
      combobox(nullptr),
      m_enableStyleSheetThemes(enableStyleSheetThemes),
      filteredStyles(filteredStyles)
{
    qDebug() << "StyleHandler()";

    Q_INIT_RESOURCE(themes_icon); // Icon-Ressourcen laden (themes_icon.qrc)

    // Beim Start gespeicherten Style aus QSettings wiederherstellen
    setStyle(getCurrentStyle());

    // Standard-Anzeigenamen und Icons für die Theme-ComboBox
    // Format: themeMap[id] = {"Anzeigename", {"hell-icon", "dunkel-icon"}}
    themeMap["windows11"]          = {"Windows 11",         {":/icons/Windows-11-Win-X-Menu-icon.png", ":/icons/Windows-11-Win-X-Menu-icon.png"}};
    themeMap["Fusion"]             = {"Fusion",             {":/icons/auto_icon.png",                  ":/icons/auto_icon_white.png"}};
    themeMap["windowsvista"]       = {"Windows Classic",    {":/icons/sun_icon.png",                   ":/icons/sun_icon_white.png"}};
    themeMap["Windows"]            = {"Windows Old",        {":/icons/auto_icon.png",                  ":/icons/auto_icon_white.png"}};
    themeMap["Fusion_OWN_dark_gray"]  = {"Fusion Gray",      {":/icons/mond_icon.png",                  ":/icons/mond_icon_white.png"}};
    themeMap["Fusion_OWN_dark_blue"]  = {"Fusion Blue",      {":/icons/mond_icon.png",                  ":/icons/mond_icon_white.png"}};
    themeMap["Fusion_OWN_dark_blue2"] = {"Fusion LightBlue", {":/icons/mond_icon.png",                  ":/icons/mond_icon_white.png"}};
    themeMap["QDarkStyleDark"]     = {"QDarkStyle",         {":/icons/mond_icon.png",                  ":/icons/mond_icon_white.png"}};
    themeMap["QDarkStyleLight"]    = {"QDarkStyleLight",    {":/icons/sun_icon.png",                   ":/icons/sun_icon_white.png"}};

    // System-Farbschema-Änderungen verfolgen (z.B. Hell/Dunkel-Umschaltung in den Systemeinstellungen)
    connect(QApplication::styleHints(), &QStyleHints::colorSchemeChanged,
            this, &StyleHandler::colorSchemeChanged);
}
StyleHandler::~StyleHandler()
{
    Q_CLEANUP_RESOURCE(themes_icon);

    if (combobox) {
        // darkstyleicons nur entladen wenn die ComboBox vorhanden ist
        // (sie wird nur bei Bedarf erzeugt, deshalb auch nur dann geladen)
        delete combobox;
        combobox = nullptr;
        Q_CLEANUP_RESOURCE(darkstyleicons);
    }
}

bool StyleHandler::setStyle(const QString &style)
{
    qDebug() << "StyleHandler::setStyle:" << style;

    // Eventuell vorhandenes QSS-Stylesheet zurücksetzen
    qApp->setStyleSheet("");

    // Prüfen ob style ein Qt-nativer Style aus QStyleFactory ist (Groß-/Kleinschreibung ignorieren)
    const auto keys = QStyleFactory::keys();
    bool is_fac_style = false;
    for (const auto &e : keys)
        if (e.toLower() == style.toLower())
            is_fac_style = true;

    if (is_fac_style) {
        // Qt-nativer Style — Palette zurücksetzen und Style direkt anwenden
        QApplication::setPalette(QPalette());
        QApplication::processEvents();
        qApp->setStyle(QStyleFactory::create(style));

    } else {
        // Eigene Fusion-Paletten oder QSS-basierte Styles
        QPalette palette;
        if (style == "Fusion_OWN_dark_gray") {       // Dunkel-Grau Fusion-Palette
                palette.setColor(QPalette::Window, QColor(53, 53, 53));
                palette.setColor(QPalette::WindowText, Qt::white);
                palette.setColor(QPalette::Base, QColor(25, 25, 25));
                palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
                palette.setColor(QPalette::ToolTipBase, QColor(53, 53, 53));
                palette.setColor(QPalette::ToolTipText, Qt::white);
                palette.setColor(QPalette::Text, Qt::white);
                palette.setColor(QPalette::Button, QColor(53, 53, 53));
                palette.setColor(QPalette::ButtonText, Qt::white);
                palette.setColor(QPalette::BrightText, Qt::red);
                palette.setColor(QPalette::Link, QColor(42, 130, 218));
        } else if (style == "Fusion_OWN_dark_blue") { // Dunkel-Blau Fusion-Palette (Navy)
                palette.setColor(QPalette::Window, QColor(30, 32, 37));
                palette.setColor(QPalette::WindowText, Qt::white);
                palette.setColor(QPalette::Base, QColor(28, 31, 47));
                palette.setColor(QPalette::AlternateBase, QColor(30, 32, 37));
                palette.setColor(QPalette::ToolTipBase, QColor(30, 32, 37));
                palette.setColor(QPalette::ToolTipText, Qt::white);
                palette.setColor(QPalette::Text, Qt::white);
                palette.setColor(QPalette::Button, QColor(30, 32, 37));
                palette.setColor(QPalette::ButtonText, Qt::white);
                palette.setColor(QPalette::BrightText, Qt::red);
                palette.setColor(QPalette::Link, QColor(138, 211, 230));

        } else if (style == "Fusion_OWN_dark_blue2") { // Dunkel-Blau 2 Fusion-Palette (Navy)
                palette.setColor(QPalette::Window, QColor(28, 31, 47));
                palette.setColor(QPalette::WindowText, Qt::white);
                palette.setColor(QPalette::Base, QColor(38, 43, 58));
                palette.setColor(QPalette::AlternateBase, QColor(30, 32, 37));
                palette.setColor(QPalette::ToolTipBase, QColor(30, 32, 37));
                palette.setColor(QPalette::ToolTipText, Qt::white);
                palette.setColor(QPalette::Text, Qt::white);
                palette.setColor(QPalette::Button, QColor(51, 55, 71));
                palette.setColor(QPalette::ButtonText, Qt::white);
                palette.setColor(QPalette::BrightText, Qt::cyan);
                palette.setColor(QPalette::Link, QColor(138, 211, 230));

        } else if (style == "QDarkStyleDark" || style == "QDarkStyleLight") {
            // QSS-basierter Style — Stylesheet aus eingebetteter Ressource laden
            const QString qssPath = (style == "QDarkStyleDark") ? "://qss_icons/darkstyle.qss"
                                                                 : "://qss_icons/lightstyle.qss";
            QFile file(qssPath);
            if (!file.open(QIODevice::ReadOnly)) {
                qWarning() << "StyleHandler: QSS-Datei konnte nicht geöffnet werden:" << qssPath;
                return false;
            }
            QByteArray data = file.readAll();
            file.close();

            // macOS-Workaround: QComboBox::indicator-Selektor verursacht Darstellungsfehler
            // → Abschnitt auskommentieren
            if (QSysInfo::productType() == "macos") {
                int index = data.indexOf("QComboBox::indicator {");
                if (index != -1) {
                    int index2 = data.indexOf("}", index);
                    if (index2 != -1) {
                        data.insert(index, "/*");
                        data.insert(index2 + 3, "*/"); // +3: hinter '}'  (index verschiebt sich durch erstes insert)
                    }
                }
            }

            qApp->setStyleSheet(data);

        } else {
            return false; // Unbekannter Style
        }

        // Fusion als Basis-Style setzen (eigene Palette wird darüber gelegt)
        QApplication::setPalette(palette);
        QApplication::processEvents();
        qApp->setStyle(QStyleFactory::create("Fusion"));
    }

    // Gewählten Style persistieren (muss VOR updateComboBoxIconColor() stehen,
    // da getCurrentStyle() diesen Wert liest)
    QSettings settingOwnColor(organisation, application);
    settingOwnColor.setValue(ENTRY_NAME, style);

    // Icons in der ComboBox aktualisieren (hell/dunkel je nach aktuellem Farbschema)
    updateComboBoxIconColor();

    emit currentStyleChanged();
    return true;
}

QStringList StyleHandler::getStyles()
{
    // Qt-native Styles als Basis
    auto styles = QStyleFactory::keys();

    // Eigene Fusion-Paletten-Styles anhängen
    styles.append("Fusion_OWN_dark_gray");
    styles.append("Fusion_OWN_dark_blue");
    styles.append("Fusion_OWN_dark_blue2");

    // QSS-basierte Styles nur wenn aktiviert
    if (styleSheetsEnabled()) {
        styles.append("QDarkStyleLight");
        styles.append("QDarkStyleDark");
    }

    // Whitelist anwenden: nur Styles behalten die in filteredStyles vorkommen
    if (!filteredStyles.isEmpty()) {
        styles.removeIf([&](const QString &s) {
            for (const QString &fs : filteredStyles)
                if (fs.toUpper() == s.toUpper())
                    return false; // behalten
            return true; // entfernen
        });
    }

    return styles;
}

void StyleHandler::updateStyleList()
{
    // ComboBox neu befüllen — aktuellen Style vorher merken um ihn nach dem
    // Neubefüllen wieder auszuwählen
    QStringList keys = getStyles();

    if (combobox) {
        if (combobox->count())
            combobox->clear();
        int current = -1;
        const QString cs = getCurrentStyle();
        for (const auto &e : keys) {
            if (themeMap.contains(e)) {
                // Icon je nach aktuellem System-Farbschema wählen
                const QString iconPath = (getCurrentColorTheme() == Qt::ColorScheme::Light)
                                             ? themeMap[e].second.first
                                             : themeMap[e].second.second;
                combobox->addItem(QIcon(iconPath), themeMap[e].first, QVariant(e));
            }
            else {
                // Kein Anzeigename in themeMap → Style-ID direkt verwenden
                combobox->addItem(e, QVariant(e));
            }

            if (e == cs)
                current = combobox->count() - 1;
        }
        if (current != -1)
            combobox->setCurrentIndex(current);
    }
}

QComboBox *StyleHandler::getCombobox()
{
    // QMutexLocker verhindert DCLP-Problem bei gleichzeitigem Zugriff
    QMutexLocker locker(&objMutex);
    if (combobox == nullptr) {
        combobox = new QComboBox();
        Q_INIT_RESOURCE(darkstyleicons); // Icons für QDarkStyle-Einträge
        updateStyleList();
        connect(combobox, &QComboBox::currentIndexChanged,
                this, &StyleHandler::indexChanged);
    }
    return combobox;
}

QMap<QString, QPair<QString, QPair<QString, QString> > > &StyleHandler::getThemeMap()
{
    return themeMap;
}

bool StyleHandler::setAutoStart(bool enabled)
{
#ifdef Q_OS_WIN
    // Windows-Autostart via Registry: HKCU\Software\Microsoft\Windows\CurrentVersion\Run
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                       QSettings::NativeFormat);

    if (enabled) {
        const QString programPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
        if (!settings.isWritable()) {
            qWarning() << "setAutoStart: Registry-Schlüssel nicht beschreibbar";
            return false;
        }
        settings.setValue(application, programPath + " /minimized");
        if (!settings.contains(application)) {
            qWarning() << "setAutoStart: Eintrag konnte nicht gesetzt werden";
            return false;
        }
    } else {
        if (!settings.isWritable()) {
            qWarning() << "setAutoStart: Registry-Schlüssel nicht beschreibbar";
            return false;
        }
        settings.remove(application);
        if (settings.contains(application)) {
            qWarning() << "setAutoStart: Eintrag konnte nicht entfernt werden";
            return false;
        }
    }

    return true;
#else
    Q_UNUSED(enabled)
    qWarning() << "setAutoStart: auf dieser Plattform nicht unterstützt";
    return false;
#endif
}

void StyleHandler::indexChanged(int i)
{
    if(combobox)
        this->setStyle(combobox->itemData(i).toString());

}

QString StyleHandler::getCurrentStyle()
{
    // Standardwert "Fusion" falls noch kein Style gespeichert wurde
    QSettings settingOwnColor(organisation, application);
    return settingOwnColor.value(ENTRY_NAME, "Fusion").toString();
}

void StyleHandler::colorSchemeChanged(Qt::ColorScheme)
{
    // Aktuellen Style neu anwenden damit Icons und Palette dem neuen Farbschema entsprechen
    setStyle(getCurrentStyle());
}

QString StyleHandler::getOrganisation() const
{
    return organisation;
}

QStringList StyleHandler::getFilteredStyles() const
{
    return filteredStyles;
}

void StyleHandler::setFilteredStyles(const QStringList &newFilteredStyles)
{
    filteredStyles = newFilteredStyles;
}

void StyleHandler::updateComboBoxIconColor()
{
    // Icons in der ComboBox aktualisieren: hell-Icon bei hellem Farbschema, dunkel-Icon sonst
    if (combobox == nullptr)
        return;

    for (int i = 0; i < combobox->count(); i++) {
        const QString id = combobox->itemData(i).toString();
        if (themeMap.contains(id)) {
            const QString iconPath = (getCurrentColorTheme() == Qt::ColorScheme::Light)
                                         ? themeMap[id].second.first
                                         : themeMap[id].second.second;
            combobox->setItemIcon(i, QIcon(iconPath));
        }
    }
}

bool StyleHandler::styleSheetsEnabled() const
{
    return m_enableStyleSheetThemes;
}

void StyleHandler::setStyleSheetEnabled(bool enabled)
{
    m_enableStyleSheetThemes = enabled;
}

bool StyleHandler::currentStyleUsesStyleSheets()
{
    const QString cs = getCurrentStyle();
    return cs == "QDarkStyleLight" || cs == "QDarkStyleDark";
}

Qt::ColorScheme StyleHandler::getCurrentColorTheme()
{
    // QDarkStyleLight ist trotz QSS-Basis ein helles Theme → explizit Light zurückgeben
    // Alle anderen Styles folgen dem System-Farbschema
    return (getCurrentStyle() == "QDarkStyleLight") ? Qt::ColorScheme::Light
                                                    : QGuiApplication::styleHints()->colorScheme();
}

QString StyleHandler::getVersion()
{
    return QString("%1.%2.%3")
        .arg(MUPDATER_majorVersion)
        .arg(MUPDATER_minorVersion)
        .arg(MUPDATER_minorMinorVersion);
}
