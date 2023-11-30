#ifndef DIALOGUEBER_H
#define DIALOGUEBER_H

#include "mupdater.h"
#include "stylehandler.h"
#include <QDialog>
#include <QFile>
#include <QMenu>

namespace Ui {
class DialogUeber;
}

class DialogUeber : public QDialog
{
    Q_OBJECT

public:
    explicit DialogUeber(const QString &maintananceToolPath, const QString &organisation, const QString &application,
                         const QString &version, const QColor & color, QWidget *parent = nullptr,
                         const bool parentCouldBeInvisSoDontExitIfUpdterNotifierIsClosed = false,
                         const bool autoHideUpdterPart = false);
    void setDescription(const QString & website_url, const QString &descrition, const QString &alternative_url_name = "");
    void setDescription(const QString & website_url, QFile descrition, const QString &alternative_url_name = "");
    void setContributorList(const QStringList &contributors);
    void setPixmap(const QPixmap &ico);
    void setLicence(QFile licence, bool center_with_html = true);
    void setLicence(QString licence, bool center_with_html = true);
    void setIssueWebsite(QString url);
    void setUpdaterFinishedMsgBoxFilePath(QString path);

    ~DialogUeber();
    MUpdater *updater();
    StyleHandler *styleHandler();

private slots:
    void updaterStatusChanged();
    void on_pushButtonUpdaterButton_clicked();
    void on_pushButtonClose_clicked();

    void on_pushButtonMenueAutoUpdate_clicked();
    void onActionAutoUSearchClicked();
    void onActionStartMaintanaceToolCliecked();

private:
    Ui::DialogUeber *ui;
    QMenu * autoUpdaterMenue;
    QAction * actionupdater,
        *action_start_maintanace_tool;

    MUpdater * m_updater;
    StyleHandler * m_styleHandler;
    QString version;
    QColor color;
    QString msgBoxFilePath;

    bool autoHideUpdterPart;

//    void delay(int sec = 1);
    void setUpdateErrorLog(QString msg);

};






#endif // DIALOGUEBER_H
