#ifndef UPDATER_H
#define UPDATER_H



#include <QString>
#include <QSettings>
#include <QProcess>
#include <QMessageBox>
#include <QObject>

//#define Q_OS_WEB
#define majorVersion 1
#define minorVersion 0
#define minorMinorVersion 0

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

    ///Getter
    bool getAutoSearchForUpdateStatus();
    /*return true, if MUpdater automatically searches for updates at start (after construction)
     *return false, if option is diabled with setAutoSearchForUpdate(const bool & status)*/

    QString getNewVersion();
    //return new version if version have been able to be extracted after checkForUpdates(), otherwhise return empty string

    QString getError() const;
    //return Error Status

    UPDATE_STATUS getStatus() const;
    /*return current status*/

    QString getStatusStr();
    /*return current status string for e.g message for user*/

    QString getExtraErrorInfo() const;
    /*contains log of maintenance tool*/

    QString getVersion();
    //returns version as string

    int getMajorVersion();
    int getMinorVersion();
    int getPatchVersion();
    //returns version as int. Minor Versions are Backwards compatible, major ones not!

    ///Setter
    void setAutoSearchForUpdate(const bool & status);
    //set auto check for updates after object construction to true/false

    ///main Functions

    //Check for update if either haven't already been checked, has failed or the program is up to date
    bool checkForUpdates(bool showMessageBox = false);
    /*Only shows Msg Box if Update is needed an there hasn't already one been shown*/
    bool showUpdateMessageBox();
    /*Start Maintanace Tool if status is 'update needed'*/
    bool startUpdate();
    /*stop running processes, reset error msg's and set status to not checked*/
    bool resetAll();

#ifndef Q_OS_WEB
private:
    QProcess updaterPrz;
    QProcess maintaneceToolPrz;
#endif

signals:
    void statusChanged();

public slots:
    void updateDialogButtonClicked(QAbstractButton *button);
#ifndef Q_OS_WEB
    void onUpdateCheckFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onUpdateMaintanenceTollFinished(int exitCode, QProcess::ExitStatus exitStatus);
#endif

private:

    bool zustandWechseln(const QString &action, const QString &value = "");
    void setStatus(const UPDATE_STATUS & status, const QString &error_msg = "No Error", const QString &extraErrorInfo = "" );
    void do_checkForUpdates();
    void do_UpdateCheckFinished(const QString &value);
    void do_showUpdateMessageBox();
    void do_startUpdate();
    void do_updaterFinished(const QString &value);
    QString getQProzessStartErrorStr(unsigned error);


private:
    QMessageBox * updateMsgBox;
    QString newVersion;
    QString error, extraErrorInfo;
    QString organisation;
    QString application;
    QString maintananceToolPath;
    bool showMsgBox;
};










#endif // UPDATER_H
