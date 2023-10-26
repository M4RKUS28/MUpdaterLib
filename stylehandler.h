#ifndef STYLEHANDLER_H
#define STYLEHANDLER_H

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

#define majorVersion 1
#define minorVersion 0
#define minorMinorVersion 0

class StyleHandler : public QObject
{
    Q_OBJECT
public:
    StyleHandler(const QString &organisation, const QString &application);
    ~StyleHandler();

    bool setStyle(QString style);
    //set Style

    QStringList getStyles();
    //returns list of possible styles ( qt + own ones )

    QComboBox * getCombobox();
    //creates combobox at first call, after that, returns always the same one

    QString getVersion();
    //returns version as string

    int getMajorVersion();
    int getMinorVersion();
    int getPatchVersion();
    //returns version as int. Minor Versions are Backwards compatible, major ones not!

    QMap<QString, QString> &getThemeMap();
    /*edit theme names in combobox
      <id><name>
      default names:
        themeMap["Fusion"] = "Fusion (Auto)";
        themeMap["windowsvista"] = "Windows Classic (Hell)";
        themeMap["Windows"] = "Windows Old (Auto)";
        themeMap["Fusion_OWN_dark_gray"] = "Fusion Gray (Dunkel)";
        themeMap["Fusion_OWN_dark_blue"] = "Fusion Blue (Dunkel)";
        themeMap["Fusion_OWN_dark_blue2"] = "Fusion LightBlue (Dunkel)";
    */

private slots:
    void indexChanged(int i);
    void updateStyleList();
    QString getCurrentStyle();

private:
    const QString organisation;
    const QString application;
    const QString ENTRY_NAME = "MUPDATER_STYLE";
    QComboBox * combobox;
    QMutex objMutex;
    QMap<QString, QString> themeMap;
};

#endif // STYLEHANDLER_H
