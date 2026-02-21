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
#include <QApplication>


class StyleHandler : public QObject
{
    Q_OBJECT
public:
    StyleHandler(const QString &organisation, const QString &application, const bool enableStyleSheetThemes = false, QStringList filteredStyles = QStringList());
    ~StyleHandler();

    bool setStyle(QString style);
    //set Style

    QStringList getStyles();
    //returns list of possible styles ( qt + own ones )

    QComboBox * getCombobox();
    //creates combobox at first call, after that, returns always the same one

    QString getVersion();
    //returns version as string

    QMap<QString, QPair<QString, QPair<QString, QString>> > &getThemeMap();
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

    bool setAutoStart(bool enabled);

    bool styleSheetsEnabled() const;
    void setStyleSheetEnabled(bool status);
    bool currentStyleUsesStyleSheets();

    Qt::ColorScheme getCurrentColorTheme();
    
    QStringList getFilteredStyles() const;
    void setFilteredStyles(const QStringList &newFilteredStyles);

    QString getOrganisation() const;

signals:
    QString currentStyleChanged();

private slots:
    void indexChanged(int i);
    void updateStyleList();
    QString getCurrentStyle();
    void colorSchemeChanged(Qt::ColorScheme colorScheme);

private:
    const QString organisation;
    const QString application;
    const QString ENTRY_NAME = "MUPDATER_STYLE";
    QComboBox * combobox;
    QMutex objMutex;
    QMap<QString, QPair<QString, QPair<QString, QString>>> themeMap;
    bool enablestelyesheetthemes;
    QStringList filteredStyles;
    void updateComboBoxIconColor();
};

#endif // STYLEHANDLER_H
