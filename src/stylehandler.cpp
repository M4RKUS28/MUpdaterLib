#include "stylehandler.h"
#include "mupdater.h"

#include <QDir>
#include <qfile.h>

StyleHandler::StyleHandler(const QString &organisation, const QString &application,  const bool enableStyleSheetThemes, QStringList filteredStyles)
    : organisation(organisation), application(application), combobox(nullptr), enablestelyesheetthemes(enableStyleSheetThemes), filteredStyles(filteredStyles)
{
    qDebug() << "       StyleHandler()";

    Q_INIT_RESOURCE(themes_icon);

    setStyle(getCurrentStyle());

    themeMap["windows11"] =                QPair<QString, QPair<QString, QString>>("Windows 11",
                                                                    QPair<QString, QString>(":/icons/Windows-11-Win-X-Menu-icon.png", ":/icons/Windows-11-Win-X-Menu-icon.png"));

    themeMap["Fusion"] =                QPair<QString, QPair<QString, QString>>("Fusion",
                                                                 QPair<QString, QString>(":/icons/auto_icon.png", ":/icons/auto_icon_white.png"));
    themeMap["windowsvista"] =          QPair<QString, QPair<QString, QString>>("Windows Classic",
                                                                       QPair<QString, QString>(":/icons/sun_icon.png", ":/icons/sun_icon_white.png"));
    themeMap["Windows"] =               QPair<QString, QPair<QString, QString>>("Windows Old",
                                                                  QPair<QString, QString>(":/icons/auto_icon.png", ":/icons/auto_icon_white.png"));
    themeMap["Fusion_OWN_dark_gray"] =  QPair<QString, QPair<QString, QString>>("Fusion Gray",
                                                                               QPair<QString, QString>( ":/icons/mond_icon.png", ":/icons/mond_icon_white.png"));
    themeMap["Fusion_OWN_dark_blue"] =  QPair<QString, QPair<QString, QString>>("Fusion Blue",
                                                                               QPair<QString, QString>(":/icons/mond_icon.png", ":/icons/mond_icon_white.png"));
    themeMap["Fusion_OWN_dark_blue2"] = QPair<QString, QPair<QString, QString>>("Fusion LightBlue",
                                                                                QPair<QString, QString>(":/icons/mond_icon.png", ":/icons/mond_icon_white.png"));
    themeMap["QDarkStyleDark"] =        QPair<QString, QPair<QString, QString>>("QDarkStyle",
                                                                         QPair<QString, QString>(":/icons/mond_icon.png", ":/icons/mond_icon_white.png"));
    themeMap["QDarkStyleLight"] =       QPair<QString, QPair<QString, QString>>("QDarkStyleLight",
                                                                          QPair<QString, QString>(":/icons/sun_icon.png", ":/icons/sun_icon_white.png"));


    connect(QApplication::styleHints(), SIGNAL(colorSchemeChanged(Qt::ColorScheme)), this, SLOT(colorSchemeChanged(Qt::ColorScheme)));
}
StyleHandler::~StyleHandler()
{
    Q_CLEANUP_RESOURCE(themes_icon);

    if(combobox) {
        qDebug() << "           ~StyleHandler::~combobox()";
        delete combobox;
        combobox = nullptr;
        Q_CLEANUP_RESOURCE(darkstyleicons);
    }
    qDebug() << "       ~StyleHandler()";
}

bool StyleHandler::setStyle(QString style)
{
    qDebug() << "       > Style: " << style;
    qApp->setStyleSheet("");

    auto keys = QStyleFactory::keys();
    bool is_fac_style = false;
    for(const auto & e : keys)
        if(e.toLower() == style.toLower())
            is_fac_style = true;

    if(is_fac_style) {
        QApplication::setPalette(QPalette());
        QApplication::processEvents();
        qApp->setStyle( QStyleFactory::create(style) );

    } else {
        //QApplication::setPalette(QPalette());
        //QApplication::processEvents();
        QPalette palette;
         if(style == "Fusion_OWN_dark_gray") {
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
        }  else if(style == "Fusion_OWN_dark_blue") {
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

        } else if(style == "Fusion_OWN_dark_blue2") {
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

        }  else if(style == "QDarkStyleDark" || style == "QDarkStyleLight") {

            QFile file(style == "QDarkStyleDark" ? "://qss_icons/darkstyle.qss" : "://qss_icons/lightstyle.qss");
            QByteArray data;

            if(!file.open(QIODevice::ReadOnly)) {
                qDebug()<<"filenot opened";
                return false;
            }  else  {
                data = file.readAll();
            }
            file.close();

            if( QSysInfo::productType() == "macos" /*Qt 6*/ || QSysInfo::productType() == "macos"  /*Qt 5*/  || true) {
                int index = data.indexOf("QComboBox::indicator {");
                if( index != -1 ) {
                    int index2 = data.indexOf("}", index);
                    if( index2 != -1 ) {
                        data.insert(index, "/*");
                        index2+= 2 + 1;
                        data.insert(index2, "*/");
                    }
                }
            }

            qApp->setStyleSheet(data);

        }  else if(style == "QDarkStyleLight") {



        } else
            return false;
        QApplication::setPalette(palette);
        QApplication::processEvents();
        qApp->setStyle( QStyleFactory::create("Fusion") );
        //QApplication::processEvents();
    }

    QSettings settingOwnColor(organisation, application);
    settingOwnColor.setValue(ENTRY_NAME, style );


    //update icons in combobox after new style is saved in q settings!!
    updateComboBoxIconColor();

    emit currentStyleChanged();

    return true;
}

QStringList StyleHandler::getStyles()
{
    auto styles = QStyleFactory::keys();
    styles.append("Fusion_OWN_dark_gray");
    styles.append("Fusion_OWN_dark_blue");
    styles.append("Fusion_OWN_dark_blue2");

    if(styleSheetsEnabled()) {
        styles.append("QDarkStyleLight");
        styles.append("QDarkStyleDark");
    }

    if(filteredStyles.size() > 0 ) {

        for( int i = 0; i < styles.size(); i++) {
            bool contains = false;
            for(const QString &fs : filteredStyles) {
                if(QString(fs).toUpper() == styles.at(i).toUpper()) {
                    contains = true;
                    break;
                }
            }
            if(!contains) {
                styles.remove(i, 1);
                i--;
            }
        }


    }

    return styles;
}

void StyleHandler::updateStyleList()
{
    QStringList keys = getStyles();

    if(combobox) {
        if(combobox->count())
            combobox->clear();
        int current = -1;
        QString cs = getCurrentStyle();
        for(const auto & e : keys) {
            if(themeMap.contains(e))
                combobox->addItem(QIcon( (getCurrentColorTheme() == Qt::ColorScheme::Light)
                                            ? themeMap[e].second.first : themeMap[e].second.second),
                                  themeMap[e].first, QVariant(e));
            else
                combobox->addItem(e, QVariant(e));

            if(e == cs)
                current = combobox->count() - 1;
        }
        if(current != -1)
            combobox->setCurrentIndex(current);
    }
}

QComboBox *StyleHandler::getCombobox()
{
    if(combobox == nullptr) {
        StyleHandler::objMutex.lock();
        if(combobox == nullptr) {
            qDebug() << "           StyleHandler::combobox()";
            combobox = new QComboBox();
            Q_INIT_RESOURCE(darkstyleicons);
            updateStyleList();
            connect(combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(indexChanged(int)));
        }
        StyleHandler::objMutex.unlock();
    }
    return combobox;
}

QMap<QString, QPair<QString, QPair<QString, QString> > > &StyleHandler::getThemeMap()
{
    return themeMap;
}

bool StyleHandler::setAutoStart(bool enabled)
{
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);

    if(enabled) {
        QString programPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
        if( ! settings.isWritable()) {
            qDebug() << "NICHT SCHREIBBAR!!!";
            return false;
        } else
            settings.setValue(application, programPath + " /minimized");
        if( ! settings.contains(application)) {
            qDebug() << "autostart setzten fehlgeschlagen!";
            return false;
        }
        qDebug() << "autostart updated to "<< programPath.toStdString();
    } else {
        if( ! settings.isWritable()) {
            qDebug() << "NICHT SCHREIBBAR!!!";
            return false;
        } else
            settings.remove(application);
        if( settings.contains(application)) {
            qDebug() << "autostart entfernen fehlgeschlagen!";
            return false;
        }
    }

    return true;
}

void StyleHandler::indexChanged(int i)
{
    if(combobox)
        this->setStyle(combobox->itemData(i).toString());

}

QString StyleHandler::getCurrentStyle()
{
    QSettings settingOwnColor(organisation, application);
    return settingOwnColor.value(ENTRY_NAME, "Fusion"/*QApplication::style()->name()*/).toString();
}

void StyleHandler::colorSchemeChanged(Qt::ColorScheme)
{
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
    if(combobox != nullptr) {
        for(int i = 0; i < combobox->count(); i++) {
            QString id = combobox->itemData(i).toString();
            if(themeMap.contains(id))
                combobox->setItemIcon(i, QIcon( (getCurrentColorTheme() == Qt::ColorScheme::Light)
                                            ? themeMap[id].second.first : themeMap[id].second.second));
        }
    }
}

bool StyleHandler::styleSheetsEnabled() const
{
    return enablestelyesheetthemes;
}

void StyleHandler::setStyleSheetEnabled(bool status)
{
    enablestelyesheetthemes = status;
}

bool StyleHandler::currentStyleUsesStyleSheets()
{
    if(getCurrentStyle() == "QDarkStyleLight" || getCurrentStyle()  == "QDarkStyleDark")
        return true;
    else
        return false;
}

Qt::ColorScheme StyleHandler::getCurrentColorTheme()
{
    return getCurrentStyle() == "QDarkStyleLight" ? Qt::ColorScheme::Light
                                                  : QGuiApplication::styleHints()->colorScheme();
}

QString StyleHandler::getVersion()
{
    return QString::number(MUPDATER_majorVersion) + "." +QString::number(MUPDATER_minorVersion) + "." + QString::number(MUPDATER_minorMinorVersion);
}
