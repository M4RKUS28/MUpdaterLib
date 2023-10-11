#ifndef UPDATER_H
#define UPDATER_H



#include <QString>
#include <QSettings>
#include <QProcess>
#include <QMessageBox>
#include <QObject>

//#define Q_OS_WEB



class MUpdater : public QObject
{
    Q_OBJECT
public:
    enum UPDATE_STATUS {
        NOT_CHECKED = 0,
        NO_UPDATER  = 1,
        CHECKING    = 2,
        UP_TO_DATE  = 3,
        UPDTAE_NEEDED = 4,
        UPDATING    = 5,
        UPDATE_ERROR  = 6,
        UPDATE_FINISHED = 7
    };
    UPDATE_STATUS status;

    MUpdater(QString maintananceToolPath, QString organisation, QString application, bool doAutoUpdateIfEnabled = true);
    ~MUpdater();

    //Getter
    bool getAutoSearchForUpdateStatus();
    bool updateExists();
    QString getNewVersion();
    QString getError() const;
    UPDATE_STATUS getStatus() const;
    QString getStatusStr();

    //Setter
    void setAutoSearchForUpdate(const bool & status);
    void setAutoShowMessageBox(const bool & status);
    bool showUpdateMessageBox();

    //
    bool checkForUpdates();
    bool startUpdate();

private:
#ifndef Q_OS_WEB
    QProcess updaterPrz;
    QProcess maintaneceToolPrz;
#endif
    QMessageBox * updateMsgBox;

signals:
    emit void statusChanged();


public slots:
    void updateDialogButtonClicked(QAbstractButton *button);
#ifndef Q_OS_WEB
    void onUpdateCheckFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onUpdateMaintanenceTollFinished(int exitCode, QProcess::ExitStatus exitStatus);
#endif

private:

    bool zustandWechseln(const QString &action, const QString &value = "");
    void setStatus(const UPDATE_STATUS & status, const QString &error_msg = "No Error");
    void do_checkForUpdates();
    void do_UpdateCheckFinished(const QString &value);
    void do_showUpdateMessageBox();
    void do_startUpdate();


    QString newVersion;
    QString error;
    QString organisation;
    QString application;

    QString maintananceToolPath;
    bool showMsgBox;
};










#endif // UPDATER_H
