#include "stylehandler.h"

StyleHandler::StyleHandler(const QString &organisation, const QString &application)
    : organisation(organisation), application(application), combobox(nullptr)
{
    setStyle(getCurrentStyle());
    themeMap["Fusion"] = "Fusion (Auto)";
    themeMap["windowsvista"] = "Windows Classic (Hell)";
    themeMap["Windows"] = "Windows Old (Auto)";
    themeMap["Fusion_OWN_dark_gray"] = "Fusion Gray (Dunkel)";
    themeMap["Fusion_OWN_dark_blue"] = "Fusion Blue (Dunkel)";
    themeMap["Fusion_OWN_dark_blue2"] = "Fusion LightBlue (Dunkel)";

}
StyleHandler::~StyleHandler()
{
    if(combobox) {
        delete combobox;
        combobox = nullptr;
    }
}

bool StyleHandler::setStyle(QString style)
{
    qDebug() << style;

    auto keys = QStyleFactory::keys();
    bool is_fac_style = false;
    for(const auto & e : keys)
        if(e == style)
            is_fac_style = true;

    if(is_fac_style) {
        QApplication::setPalette(QPalette());
        QApplication::processEvents();
        qApp->setStyle( QStyleFactory::create(style) );

    } else {
        qApp->setStyle( QStyleFactory::create("Fusion") );
        QApplication::processEvents();
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

        } else
            return false;
        QApplication::setPalette(QPalette());
        QApplication::processEvents();
        QApplication::setPalette(palette);
        QApplication::processEvents();
        QApplication::setPalette(palette);
        QApplication::processEvents();
        QApplication::setPalette(palette);
    }

    QSettings settingOwnColor(organisation, application);
    settingOwnColor.setValue(ENTRY_NAME, style );
    return true;
}

QStringList StyleHandler::getStyles()
{
    auto styles = QStyleFactory::keys();
    styles.append("Fusion_OWN_dark_gray");
    styles.append("Fusion_OWN_dark_blue");
    styles.append("Fusion_OWN_dark_blue2");
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
            combobox->addItem(themeMap.contains(e) ? themeMap[e] : e, QVariant(e));
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
            combobox = new QComboBox();
            updateStyleList();
            connect(combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(indexChanged(int)));
        }
        StyleHandler::objMutex.unlock();
    }
    return combobox;
}

QMap<QString, QString> &StyleHandler::getThemeMap()
{
    return themeMap;
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

QString StyleHandler::getVersion()
{
    return QString::number(majorVersion) + "." +QString::number(minorVersion) + "." + QString::number(minorMinorVersion);
}

int StyleHandler::getMajorVersion()
{
    return majorVersion;
}

int StyleHandler::getMinorVersion()
{
    return minorVersion;
}

int StyleHandler::getPatchVersion()
{
    return minorMinorVersion;
}
