#ifndef STYLEHANDLER_H
#define STYLEHANDLER_H

#include <QObject>
#include <QSettings>
#include <QStyleFactory>
#include <QStyle>
#include <QApplication>
#include <QComboBox>
#include <QMutex>

#include <QStyleHints>


class StyleHandler : public QObject
{
    Q_OBJECT
public:
    StyleHandler(const QString &organisation, const QString &application);
    ~StyleHandler();

    bool setStyle(QString style);
    void updateStyleList();
    QComboBox * getCombobox();

private slots:
    void indexChanged(int i);
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
