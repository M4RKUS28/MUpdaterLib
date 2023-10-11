#ifndef DIALOGUEBER_H
#define DIALOGUEBER_H

#include "mupdater.h"
#include <QDialog>

namespace Ui {
class DialogUeber;
}

class DialogUeber : public QDialog
{
    Q_OBJECT

public:
    explicit DialogUeber(const QString &maintananceToolPath, const QString &organisation, const QString &application, const QString &version, QWidget *parent = nullptr);
    void setDescription(const QUrl & website, const QString &descrition);
    void setContributorList(const QStringList &contributors);
    void setPixmap(const QPixmap &ico);


    ~DialogUeber();
    MUpdater *updater();

private slots:
    void updaterStatusChanged();
    void on_pushButtonUpdaterButton_clicked();
    void on_pushButtonClose_clicked();

private:
    Ui::DialogUeber *ui;
    MUpdater * m_updater;
    QString version, application;
    void delay(int sec = 1);
};


#endif // DIALOGUEBER_H
