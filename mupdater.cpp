#include "mupdater.h"

#include <QAbstractButton>
#include <QApplication>
#include <QFile>

MUpdater::MUpdater(QString maintananceToolPath, QString organisation, QString application, bool doAutoUpdateIfEnabled)
    : status(UPDATE_STATUS::NOT_CHECKED), updateMsgBox(nullptr), organisation(organisation), application(application), maintananceToolPath(maintananceToolPath), showMsgBox(true)
{
    qDebug() << "Updater()";

    //load color from qsetting:
    if(doAutoUpdateIfEnabled && getAutoSearchForUpdateStatus()) {
        this->checkForUpdates(true);
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
    qDebug() << "~Updater()";
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


}

void MUpdater::setAutoSearchForUpdate(const bool &status)
{
    QSettings settingOwnColor(organisation, application);
    settingOwnColor.setValue("AUTO_SEARCH_FOR_UPDATE", status );
}

bool MUpdater::getAutoSearchForUpdateStatus()
{
    QSettings settingOwnColor(organisation, application);
    return (settingOwnColor.contains("AUTO_SEARCH_FOR_UPDATE")) ? settingOwnColor.value("AUTO_SEARCH_FOR_UPDATE").toBool() : true;
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
        return "Updater ausgeführt!";
    }
    case MUpdater::UPDATE_ERROR: {
        return "Während der Suche nach Updates ist ein Fehler aufgetreten: " + this->getError();
    }
    case MUpdater::UPDATE_FINISHED: {
        return "Neustarten um Update abzuschließen:";
    }
    }
    return "";
}

bool MUpdater::checkForUpdates(bool showMessageBox)
{
    this->showMsgBox = showMessageBox;
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
    zustandWechseln("onUpdateCheckFinished()", (exitStatus == QProcess::NormalExit && exitCode == 0) ? "QProcess::NormalExit;ExitValue==0" : "<Error>");
}

void MUpdater::onUpdateMaintanenceTollFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    zustandWechseln("onUpdateMaintanenceTollFinished()", (exitStatus == QProcess::NormalExit && exitCode == 0) ? "QProcess::NormalExit;ExitValue==0" : "<Error>");
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
        } else {
            return false;   //Error
        }
        break;
    }
    case NOT_CHECKED: {
        if(action == "checkForUpdates()") {
            do_checkForUpdates();
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

        if(action == "showUpdateMessageBox()") {
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
        if(action == "updaterFinished()") {
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
        } else {
            return false;   //Error
        }
        break;
    }
    case NO_UPDATER:
        return false;
    case UPDATE_FINISHED:
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

                newVersion = "SpotifyEnhancer-" + output.mid(start + 10, ende - start - 10);
                qDebug() << newVersion;
            }

            setStatus(UPDATE_STATUS::UPDTAE_NEEDED);
            if(showMsgBox)
                zustandWechseln("showUpdateMessageBox()");

        } else {
            setStatus(UPDATE_STATUS::UP_TO_DATE);
        }
    } else {
        QString err =  updaterPrz.readAllStandardError();
        setStatus(UPDATE_STATUS::UPDATE_ERROR, "Update fehlgeschlagen!", (err.isEmpty() ? output : err));
    }
}

void MUpdater::do_showUpdateMessageBox()
{
    updateMsgBox = new QMessageBox(QMessageBox::Information, "Update Available", "Es ist eine neuere Version für SpotifyEnhancer verfügbar: " + newVersion + "\nBitte aktualiseren sie die Anwendung!");
    updateMsgBox->addButton("Jetzt aktualisieren", QMessageBox::AcceptRole);
    updateMsgBox->addButton("Später aktualisieren", QMessageBox::RejectRole);
    updateMsgBox->show();
    connect(updateMsgBox, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(updateDialogButtonClicked(QAbstractButton*)));
}

void MUpdater::do_startUpdate()
{
#ifndef Q_OS_WEB
    if(!QFile(maintananceToolPath).exists()) {
        setStatus(UPDATE_STATUS::NO_UPDATER, "Not installed with updater!");
    } else {
        qDebug() << "Start MaintenanceTool...";
        maintaneceToolPrz.start(maintananceToolPath);
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
    } else {
        QString err =  updaterPrz.readAllStandardError();
        QString output = (err.isEmpty() ? updaterPrz.readAllStandardOutput() : err);
        setStatus(UPDATE_STATUS::UPDATE_ERROR, "Konnte Maintanace Programm nicht starten!", output);
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

QString MUpdater::getExtraErrorInfo() const
{
    return extraErrorInfo;
}

