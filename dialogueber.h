#ifndef DIALOGUEBER_H
#define DIALOGUEBER_H

#include "mupdater.h"
#include <QDialog>
#include <QFile>


namespace Ui {
class DialogUeber;
}

class DialogUeber : public QDialog
{
    Q_OBJECT

public:
    explicit DialogUeber(const QString &maintananceToolPath, const QString &organisation, const QString &application, const QString &version, const QColor & color, QWidget *parent = nullptr, const bool parentCouldBeInvisSoDontExitIfUpdterNotifierIsClosed = false);
    void setDescription(const QString & website_url, const QString &descrition, const QString &alternative_url_name = "");
    void setDescription(const QString & website_url, QFile descrition, const QString &alternative_url_name = "");
    void setContributorList(const QStringList &contributors);
    void setPixmap(const QPixmap &ico);
    void setLicence(QFile licence, bool center_with_html = true);
    void setLicence(QString licence, bool center_with_html = true);
    void setIssueWebsite(QString url);


    ~DialogUeber();
    MUpdater *updater();

private slots:
    void updaterStatusChanged();
    void on_pushButtonUpdaterButton_clicked();
    void on_pushButtonClose_clicked();
    void on_radioButtonautosearchupdates_clicked(bool checked);

private:
    Ui::DialogUeber *ui;
    MUpdater * m_updater;
    QString version;
    QColor color;

//    void delay(int sec = 1);
    void setUpdateErrorLog(QString msg);
};


#endif // DIALOGUEBER_H
