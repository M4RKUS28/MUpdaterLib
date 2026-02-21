#include "mupdater.h"

#include <QAbstractButton>
#include <QApplication>
#include <QFile>
#include <QTimer>


MUpdater::MUpdater(QString maintananceToolPath, QString organisation, QString application, bool doAutoUpdateIfEnabled)
    : status(UPDATE_STATUS::NOT_CHECKED), updateMsgBox(nullptr), organisation(organisation),
    application(application), maintananceToolPath(maintananceToolPath), showMsgBox(true)
{
    qDebug() << "       MUpdater()";
    updateInfoMsgBox = new QMessageBox(QMessageBox::NoIcon, application + " Updates", "");

    if(doAutoUpdateIfEnabled && getAutoSearchForUpdateStatus()) {
        //STart timer for update checking
        QTimer::singleShot(1, this, SLOT(startSearchForUpdatesAtStartTimer()));
    }

#ifndef Q_OS_WEB
    //connect to slot
    connect(&updaterPrz, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(onUpdateCheckFinished(int,QProcess::ExitStatus)));
    connect(&maintaneceToolPrz, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(onUpdateMaintanenceTollFinished(int,QProcess::ExitStatus)));
#else
    status = UPDATE_STATUS::NO_UPDATER;
#endif
}

MUpdater::~MUpdater()
{
#ifndef Q_OS_WEB
    if(updaterPrz.state() == QProcess::Running) {
        qDebug() << "Kill updater";
        updaterPrz.kill();
        if(!updaterPrz.waitForFinished()) {
            updaterPrz.terminate();
            qDebug() << "terminate updater";
        }
    }
#endif
    qDebug() << "       ~MUpdater()";
}

void MUpdater::startSearchForUpdatesAtStartTimer()
{
    timerCounter = 0;
    timerEvent(nullptr);
    this->startTimer(15000);
}

//Check for updates at start!
void MUpdater::timerEvent(QTimerEvent *event)
{
    if(timerCounter >= 4) {
        if(event)
            this->killTimer(event->timerId());
        if(this->getStatus() == UPDATE_STATUS::UPDATE_ERROR || this->getStatus() == UPDATE_STATUS::NOT_CHECKED)
            qDebug() << ">>>>>>> timerEv: START UPDATE CHECKING FAILED!! 4 / 4 -> stop timer";
        return;
    } else
        timerCounter++;

    switch (this->getStatus()) {
    case UPDATE_STATUS::NOT_CHECKED:
    case UPDATE_STATUS::UPDATE_ERROR:
        qDebug() << ">>>>>>> timerEv: START UPDATE CHECKING... " << timerCounter << " / 4";
        this->checkForUpdates(true);
        break;
    case UPDATE_STATUS::UP_TO_DATE:
        qDebug() << ">>>>>>> timerEv: UP_TO_DATE... stopping timer";
        if(event)
            this->killTimer(event->timerId());
        break;
    default:
        break;
    }
}

void MUpdater::setAutoSearchForUpdate(const bool &status)
{
    QSettings settingOwnColor(organisation, application);
    settingOwnColor.setValue("AUTO_SEARCH_FOR_UPDATE", status );
}

bool MUpdater::getAutoSearchForUpdateStatus()
{
    QSettings settingOwnColor(organisation, application);
    return (settingOwnColor.contains("AUTO_SEARCH_FOR_UPDATE")) ?
               settingOwnColor.value("AUTO_SEARCH_FOR_UPDATE").toBool() : true;
}

bool MUpdater::showUpdateMessageBox()
{
    return zustandWechseln("showUpdateMessageBox()");
}



QString MUpdater::getNewVersion()
{
    return newVersion;
}

QString MUpdater::getError() const
{
    return error;
}

MUpdater::UPDATE_STATUS MUpdater::getStatus() const
{
    return status;
}

void MUpdater::setStatus(const UPDATE_STATUS &status, const QString &error_msg, const QString &extraErrorInfo)
{
    this->status = status;
    this->error = error_msg;
    this->extraErrorInfo = extraErrorInfo;
}


QString MUpdater::getStatusStr()
{
    switch (this->getStatus()) {
    case MUpdater::NOT_CHECKED: {
        return "Auf Updates prüfen:";
    }
    case MUpdater::NO_UPDATER: {
        return "Diese Version wurde ohne Updater installiert!";
    }
    case MUpdater::CHECKING: {
        return "Es wird auf Updates geprüft...";
    }
    case MUpdater::UP_TO_DATE: {
        return "Es wird die neuste Version verwendet!";
    }
    case MUpdater::UPDTAE_NEEDED: {
        return this->getNewVersion().isEmpty() ? "Es sind Updates verfügbar!" : "Es ist eine Neue Version verfügbar: " + this->getNewVersion();
    }
    case MUpdater::UPDATING: {
        return "Updating...";
    }
    case MUpdater::UPDATE_ERROR: {
        return "Es ist ein Fehler aufgetreten: " + this->getError();
    }
    case MUpdater::UPDATE_FINISHED: {
        return "Neustarten um Update abzuschließen:";
    }
    }
    return "";
}

bool MUpdater::checkForUpdates(bool showMessageBox, bool onlyIfUpdateAvaible)
{
    this->showMsgBox = showMessageBox;
    this->onlyIfUpdateAvaible = onlyIfUpdateAvaible;
    return zustandWechseln("checkForUpdates()");
}

bool MUpdater::startUpdate()
{
    return zustandWechseln("startUpdate()");
}

bool MUpdater::resetAll()
{
    newVersion = "";

    if(this->updateMsgBox != nullptr) {
        disconnect(updateMsgBox, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(updateDialogButtonClicked(QAbstractButton*)));
        updateMsgBox->deleteLater();
        updateMsgBox = nullptr;
    }


    for(auto e : {&this->maintaneceToolPrz, &this->updaterPrz}) {
        if(e->state() == QProcess::Running) {
            e->terminate();
            if(!e->waitForFinished(1000)) {
                e->kill();
                if(!e->waitForFinished(1000)) {
                    setStatus(UPDATE_ERROR, "Erneute Suche nach Updates fehlgeschlagen!", "Konnte laufenden Prozess nicht beenden!");
                    return false;    //Error
                }

            }
        }
    }

    setStatus(UPDATE_STATUS::NOT_CHECKED);
    return true;
}

void MUpdater::updateDialogButtonClicked(QAbstractButton *button)
{
    QMessageBox::ButtonRole role = updateMsgBox->buttonRole(button);
    disconnect(updateMsgBox, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(updateDialogButtonClicked(QAbstractButton*)));
    updateMsgBox->deleteLater();
    updateMsgBox = nullptr;
    zustandWechseln("updateDialogButtonClicked()", (role == QMessageBox::AcceptRole) ? "AcceptRole" : "<Declined>");
}

void MUpdater::onUpdateCheckFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    zustandWechseln("onUpdateCheckFinished()", (exitStatus == QProcess::NormalExit && exitCode == 0) ? "QProcess::NormalExit;ExitValue==0" : getUpdterPackageManagerCoreStatusByExitCode(exitCode));
}

void MUpdater::onUpdateMaintanenceTollFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    zustandWechseln("onUpdateMaintanenceTollFinished()", (exitStatus == QProcess::NormalExit && exitCode == 0) ? "QProcess::NormalExit;ExitValue==0" : getUpdterPackageManagerCoreStatusByExitCode(exitCode));
}



bool MUpdater::zustandWechseln(const QString &action, const QString &value)
{
    switch (this->status) {

    case UP_TO_DATE: {
        if(action == "checkForUpdates()") {
            if(!resetAll())
                return false; // Error
            else
                return zustandWechseln("checkForUpdates()");             // no status changes! --> no emits        --> return
        } else if(action == "showUpdateMessageBoxVersionInfo()") {
            if(showMsgBox && !onlyIfUpdateAvaible) {
                updateInfoMsgBox->setText("Sie verwenden die neuste Version von " + this->application);
                updateInfoMsgBox->show();
            }
            return true;
        } else {
            return false;   //Error
        }
        break;
    }
    case NOT_CHECKED: {
        if(action == "checkForUpdates()") {
            do_checkForUpdates();
            if(showMsgBox && !onlyIfUpdateAvaible) {
                updateInfoMsgBox->setText("Suche nach Aktualisierungen...");
                updateInfoMsgBox->show();
            }
        }  else {
            return false; //Error
        }
        break;
    }
    case CHECKING: {
        if(action == "onUpdateCheckFinished()") {
            do_UpdateCheckFinished(value);
        } else {
            return false; //Error
        }
        break;
    }
    case UPDTAE_NEEDED: {


        if(action == "showUpdateMessageBox()" || action == "checkForUpdates()") {
            if( !updateMsgBox) {
                do_showUpdateMessageBox();
                return true;    // no status changes! --> no emits
            }
        } else if(action == "updateDialogButtonClicked()") {
            if(value == "AcceptRole") {
                startUpdate(); // zustand_wechseln(action == "startUpdate()") mit case UPDTAE_NEEDED -> run do_strat_update();
            } else {
                setStatus(UPDATE_STATUS::UPDTAE_NEEDED);
            }

        } else if(action == "startUpdate()") {
            do_startUpdate();
        } else {
            return false;             //Error
        }
        break;
    }
    case UPDATING: {
        if(action == "onUpdateMaintanenceTollFinished()") {
            do_updaterFinished(value);
        } else {
            return false;
        }
        break;
    }
    case UPDATE_ERROR: {
        if(action == "checkForUpdates()") {
            if(!resetAll())
                return false; // Error
            else
                return zustandWechseln("checkForUpdates()");             // no status changes! --> no emits        --> return
        } else if(action == "showUpdateMessageBoxVersionInfo()") {
            if(showMsgBox && !onlyIfUpdateAvaible) {
                updateInfoMsgBox->setText("Suche nach Updates ist fehlgeschlagen! Infos unter 'Über " + this->application + "'");
                updateInfoMsgBox->show();
            }
            return true;
        } else {
            return false;   //Error
        }
        break;
    }
    case NO_UPDATER:
        return false;
    case UPDATE_FINISHED:
        if(action == "showUpdateMessageBoxForRestart()") {
            return do_showRestartMsgBox();
        }
        break;
    }

    emit statusChanged();
    return true;
}


void MUpdater::do_checkForUpdates()
{
#ifndef Q_OS_WEB
    if(!QFile(maintananceToolPath).exists()) {
        setStatus(UPDATE_STATUS::NO_UPDATER, "Application installed without MaintenanceTool (Installer)!");
    } else if(updaterPrz.state() != QProcess::NotRunning) {
        setStatus(UPDATE_STATUS::UPDATE_ERROR, "Failed to start Updater: Updater Process already Running!");
    } else {
        qDebug() << "Start the updater process";
        // Start the updater process
        updaterPrz.start(maintananceToolPath, QStringList() << "check-updates");
        if(updaterPrz.state() == QProcess::ProcessState::NotRunning && updaterPrz.exitCode() != 0) {
            setStatus(UPDATE_STATUS::UPDATE_ERROR, "Start updater failed!", this->getQProzessStartErrorStr(updaterPrz.error()));
        } else {
            setStatus(UPDATE_STATUS::CHECKING);
        }
    }
#endif
}

void MUpdater::do_UpdateCheckFinished(const QString & value)
{
    qDebug() << "onUpdateCheckFinished updater process";
    QString output = updaterPrz.readAllStandardOutput();

    if (value == "QProcess::NormalExit;ExitValue==0") {
        qDebug() << "updates... show msg box if enabled";

        if (output.contains("<updates>") && output.contains("</updates>")) {

            int start = output.indexOf(" version=\"", output.indexOf("<updates>")); // start from <updates>, otherwise xml file version 1.0 is extracted!
            if(start != -1) {
                int ende = output.indexOf("\"", start + 10);

                newVersion = application + "-" + output.mid(start + 10, ende - start - 10);
                qDebug() << newVersion;
            }

            setStatus(UPDATE_STATUS::UPDTAE_NEEDED);
            if(showMsgBox)
                zustandWechseln("showUpdateMessageBox()");
            updateInfoMsgBox->hide();

        } else {
            setStatus(UPDATE_STATUS::UP_TO_DATE);
            if(showMsgBox && !onlyIfUpdateAvaible)
                zustandWechseln("showUpdateMessageBoxVersionInfo()");
        }
    } else {
        QString err =  updaterPrz.readAllStandardError();
        setStatus(UPDATE_STATUS::UPDATE_ERROR, value, (err.isEmpty() ? output : err));

        if(showMsgBox && !onlyIfUpdateAvaible)
            zustandWechseln("showUpdateMessageBoxVersionInfo()");
    }
}

void MUpdater::do_showUpdateMessageBox()
{
    updateMsgBox = new QMessageBox(QMessageBox::Information, "Update Available", "Es ist eine neuere Version für " + application + " verfügbar: " + newVersion + "\nBitte aktualiseren sie die Anwendung!");
    updateMsgBox->addButton("Jetzt aktualisieren", QMessageBox::AcceptRole);
    updateMsgBox->addButton("Später aktualisieren", QMessageBox::RejectRole);
    updateMsgBox->show();
    connect(updateMsgBox, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(updateDialogButtonClicked(QAbstractButton*)));
}


bool MUpdater::do_showRestartMsgBox()
{
    QMessageBox *msgBox = new QMessageBox(QMessageBox::Information, "App Neustart benötigt!",   "Bitte starten sie "
                          +  application + " neu, um die Aktualisierung abzuschließen.");
    auto restartButton = msgBox->addButton("Jetzt neustarten", QMessageBox::AcceptRole);
    msgBox->addButton("Später neustarten", QMessageBox::RejectRole);


    msgBox->exec();


    if (reinterpret_cast<QPushButton*>(msgBox->clickedButton()) == restartButton) {
        // Der "Jetzt neustarten"-Button wurde geklickt
        this->restartApp();
    }

    return true;
}

void MUpdater::do_startUpdate()
{
#ifndef Q_OS_WEB
    if(!QFile(maintananceToolPath).exists()) {
        setStatus(UPDATE_STATUS::NO_UPDATER, "Not installed with updater!");
    } else {
        qDebug() << "Start MaintenanceTool...";
        maintaneceToolPrz.start(maintananceToolPath, {"--start-updater"});
        if(maintaneceToolPrz.state() == QProcess::NotRunning || maintaneceToolPrz.exitCode() != 0) {
            setStatus(UPDATE_STATUS::UPDATE_ERROR, "Start MaintenanceTool failed", this->getQProzessStartErrorStr(maintaneceToolPrz.error()));
        } else {
            setStatus(UPDATE_STATUS::UPDATING);
        }
    }
#endif
}

void MUpdater::do_updaterFinished(const QString &value)
{
    if(value == "QProcess::NormalExit;ExitValue==0") {
        setStatus(UPDATE_STATUS::UPDATE_FINISHED, "Update Erfogreich ausgeführt!");

        if(showMsgBox)
            zustandWechseln("showUpdateMessageBoxForRestart()");

    } else {
        QString err =  updaterPrz.readAllStandardError();
        QString output = (err.isEmpty() ? updaterPrz.readAllStandardOutput() : err);
        setStatus(UPDATE_STATUS::UPDATE_ERROR, value, output);
    }
}

QString MUpdater::getQProzessStartErrorStr(unsigned int error)
{
    QVector<QString> errorDescriptions = {
        /*0*/ "The process failed to start. Either the invoked program is missing, or you may have insufficient permissions or resources to invoke the program.",
        /*1*/ "The process crashed some time after starting successfully.",
        /*2*/ "The last waitFor...() function timed out. The state of QProcess is unchanged, and you can try calling waitFor...() again.",
        /*3*/ "An error occurred when attempting to read from the process. For example, the process may not be running.",
        /*4*/ "An error occurred when attempting to write to the process. For example, the process may not be running, or it may have closed its input channel.",
        /*5*/ "An unknown error occurred. This is the default return value of error()."
    };
    return (updaterPrz.error() < 6 ? errorDescriptions.at(error) : "Qt Internal Error!");
}

QString MUpdater::getUpdterPackageManagerCoreStatusByExitCode(int exitcode, bool isUpdateCheck)
{
    QString praefix = isUpdateCheck ? "Update checking" : "Installation";
    switch (exitcode) {
    case 0:
        return praefix + " was successful.";
    case 1:
        return praefix + " failed.";
    case 2:
        return praefix + " is in progress.";
    case 3:
        return praefix + " was canceled.";
    case 4:
        return praefix + " was not completed.";
    case 5:
        return "Installation has to be updated.";
    case 6:
        return "Installation essential components were updated.";
    default:
        return "Unknown Error";
    }
}




QString MUpdater::getMaintananceToolPath() const
{
    return maintananceToolPath;
}

QString MUpdater::getExtraErrorInfo() const
{
    return extraErrorInfo;
}

QString MUpdater::getVersion()
{
    return QString::number(MUPDATER_majorVersion) + "." +QString::number(MUPDATER_minorVersion) + "." + QString::number(MUPDATER_minorMinorVersion);
}

void MUpdater::restartApp()
{
    qApp->quit();
    QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
}

int MUpdater::getMajorVersion()
{
    return MUPDATER_majorVersion;
}

int MUpdater::getMinorVersion()
{
    return MUPDATER_minorVersion;
}

int MUpdater::getPatchVersion()
{
    return MUPDATER_minorMinorVersion;
}
