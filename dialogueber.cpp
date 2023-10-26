#include "dialogueber.h"
#include "ui_dialogueber.h"

#include <QTime>
#include <QApplication>
#include <QFile>
#include <QPalette>
#include <QTabBar>
#include <QApplication>



DialogUeber::DialogUeber(const QString &maintananceToolPath, const QString &organisation,
                         const QString &application, const QString &version, const QColor & color,
                         QWidget *parent, const bool parentCouldBeInvisSoDontExitIfUpdterNotifierIsClosed) :
    QDialog(parent),
    ui(new Ui::DialogUeber),
    version(version),
    color(color)
{
    ui->setupUi(this);
    if(parentCouldBeInvisSoDontExitIfUpdterNotifierIsClosed)
        qApp->setQuitOnLastWindowClosed(false); // Fix bug when shown updater window

    ui->labelTitel->setText(application + " " + version);

//    for(int i = 0; i < 5; i++) {
//        ui->tabWidget->tabBar()->setTabTextColor(i, color);
//    }

    qApp->setOrganizationName(organisation);
    ui->label_website->setHidden(true);
    ui->label_website_value->setHidden(true);
    ui->tabWidget->setTabVisible(0, false);
    ui->tabWidget->setTabVisible(1, false);
    ui->tabWidget->setTabVisible(2, false);
    ui->tabWidget->setTabVisible(4, false);
    ui->tabWidget->setCurrentIndex(0);

    //set Style Sheet:
    ui->tabWidget->setStyleSheet("QTabWidget::pane {"
                                 "  border: 1px solid gray;"
                                 "  background: rgba(220, 220, 220, 30);"
                                 "}"
                                 "QTabBar::tab {"
                                 "  background: rgba(220, 220, 220, 30); "
                                 "  border: 1px solid gray; "
                                 "  padding: 6px;"
                                 "  border-radius: 4px;"
                                 "  margin-bottom: -3px;"
                                 "} "
                                 "QTabBar::tab:selected { "
                                 "  border: 2px solid " + color.name() + ";"
                                 "  border-top-width: 4px;"
                                 "  margin-bottom: -3px;"
                                 "  padding-top: 5px;"
                                 "}");

    QString frameColorStyleSheet = "{"
                                   "	border:  1px solid " + color.name() + " ;"
                                   " 	border-radius: 10px;"
                                   "	padding: 1px;"
                                   "}";

    ui->updaterFrame->setStyleSheet("QFrame#updaterFrame" + frameColorStyleSheet);
    ui->frameueberborder->setStyleSheet("QFrame#frameueberborder" + frameColorStyleSheet);
    ui->framedebrounder->setStyleSheet("QFrame#framedebrounder" + frameColorStyleSheet);
    ui->lineItemDebLine->setStyleSheet("border: 2px solid " + color.name() + ";");


    m_updater = new MUpdater(maintananceToolPath, organisation, application, true);
    this->ui->labelUpdateStatus->setText(m_updater->getStatusStr());
    ui->radioButtonautosearchupdates->setChecked(m_updater->getAutoSearchForUpdateStatus());

    ui->label_version->setText("SpotifyEnhancer " + version);
    ui->label_qt_version->setText("Qt " + QString(QT_VERSION_STR));
    ui->label_compile_date->setText(QString(__DATE__) + QString(" at ") + QString(__TIME__));
    ui->label_os->setText(QSysInfo::productType() + " " +  QSysInfo::productVersion());
    ui->label_arch->setText(QSysInfo::currentCpuArchitecture());
    ui->label_issue_website->hide();
    ui->label_5issuetext->hide();
    ui->label_4issue2->hide();


    updaterStatusChanged();
    QObject::connect(m_updater, SIGNAL(statusChanged()), this, SLOT(updaterStatusChanged()));
}

void DialogUeber::setDescription(const QString &website_url, const QString &descrition, const QString &alternative_url_name)
{
    qApp->setOrganizationDomain(website_url);
    if(!website_url.isEmpty()) {
        ui->label_website_value->setHidden(false);
        ui->label_website_value->setText("<a style=\"text-decoration:none; color: " + color.name()+ ";\" href=\"" + website_url +"\">" + (alternative_url_name.isEmpty() ? website_url : alternative_url_name) + "</a>");
        ui->label_website->setHidden(false);



    }
    ui->label_description->setText(descrition);
    ui->tabWidget->setTabVisible(0, true);
    ui->tabWidget->setCurrentIndex(0);
}

#include <iostream>
void DialogUeber::setDescription(const QString &website_url, QFile descrition, const QString &alternative_url_name)
{
    if(!descrition.open(QIODevice::ReadOnly)) {
        if(!descrition.open(QIODevice::ReadOnly)) {
            std::cout << ("open file failed!") << descrition.errorString().toStdString() << std::endl;
            return;
        }
    }

    setDescription(website_url, descrition.readAll(),alternative_url_name);

    descrition.close();
}

void DialogUeber::setContributorList(const QStringList &/*contributors*/)
{
    //
    ui->tabWidget->setTabVisible(1, true);
    ui->tabWidget->setCurrentIndex(0);
}

void DialogUeber::setPixmap(const QPixmap &ico)
{
    this->ui->label_icon->setPixmap(ico);
}


void DialogUeber::setLicence(QFile licence, bool center_with_html)
{
    if(!licence.open(QIODevice::ReadOnly)) {
        if(!licence.open(QIODevice::ReadOnly)) {
            std::cout << ("open file failed!") << licence.errorString().toStdString() << std::endl;
            return;
        }
    }

    setLicence(licence.readAll(), center_with_html);

    licence.close();

}

void DialogUeber::setLicence(QString licence, bool center_with_html)
{
    if(center_with_html)
        ui->textEdit_licence->setHtml("<html><head/><body><p align=\"center\"><span style=\" font-size:8.25pt;\">" + (licence).replace("<", "'").replace(">", "'").replace("\n", "</span></p><p align=\"center\"><span style=\" font-size:8.25pt;\">") + "</span></p></body></html>");
    else
        ui->textEdit_licence->setPlainText( licence );

    ui->tabWidget->setTabVisible(2, true);
    ui->tabWidget->setCurrentIndex(0);
}

void DialogUeber::setIssueWebsite(QString url)
{
    ui->label_issue_website->setText("<a style=\"text-decoration:none; color: " + color.name()+ ";\" href=\"" + url +"\">hier</a>");
    ui->label_issue_website->show();
    ui->label_5issuetext->show();
    ui->label_4issue2->show();


}

DialogUeber::~DialogUeber()
{
    delete ui;
    delete m_updater;
}

MUpdater *DialogUeber::updater()
{
    return m_updater;
}

void DialogUeber::updaterStatusChanged()
{
    ui->pushButtonUpdaterButton->hide();
    ui->labelUpdateStatus->setText(updater()->getStatusStr());
    ui->plainTextEdit_updaer_error_log->appendPlainText("\n --> " + updater()->getStatusStr());

    switch (updater()->getStatus()) {
    case MUpdater::NOT_CHECKED:
        ui->pushButtonUpdaterButton->show();
        this->ui->pushButtonUpdaterButton->setText("Nach Updates suchen");
        break;
    case MUpdater::UPDTAE_NEEDED:
        ui->pushButtonUpdaterButton->show();
        this->ui->pushButtonUpdaterButton->setText("Aktualisieren");
        ui->tabWidget->setTabVisible(4, false);
        break;
    case MUpdater::UP_TO_DATE:
        ui->labelUpdateStatus->setText("Sie verwenden die neuste Version!");
        this->ui->pushButtonUpdaterButton->setText("Erneut nach Updates suchen");
        ui->pushButtonUpdaterButton->show();
        ui->tabWidget->setTabVisible(4, false);
        break;
    case MUpdater::UPDATING: {
        ui->pushButtonUpdaterButton->show();
        this->ui->pushButtonUpdaterButton->setText("Neustarten");
        break;
    }
    case MUpdater::UPDATE_ERROR: {
        ui->pushButtonUpdaterButton->show();
        this->ui->pushButtonUpdaterButton->setText("Erneut nach Updates suchen");
        this->setUpdateErrorLog(m_updater->getExtraErrorInfo());
        break;
    }
    default:
        break;
    }
}

//void DialogUeber::delay(int sec)
//{
//    QTime dieTime= QTime::currentTime().addSecs(sec);
//    while (QTime::currentTime() < dieTime)
//        QApplication::processEvents(QEventLoop::AllEvents, 100);
//}

void DialogUeber::setUpdateErrorLog(QString msg)
{
    ui->tabWidget->setTabVisible(4, true);
    if(!msg.isEmpty())
        ui->plainTextEdit_updaer_error_log->appendPlainText("\n Error Info: " + msg);
}


void DialogUeber::on_pushButtonUpdaterButton_clicked()
{
    if(updater()->getStatus() == MUpdater::UPDATE_STATUS::NOT_CHECKED || updater()->getStatus() == MUpdater::UPDATE_STATUS::UP_TO_DATE || updater()->getStatus() == MUpdater::UPDATE_STATUS::UPDATE_ERROR) {
        ui->plainTextEdit_updaer_error_log->clear();
        updater()->checkForUpdates();
    } else if(updater()->getStatus() == MUpdater::UPDATE_STATUS::UPDTAE_NEEDED) {
        updater()->startUpdate();
    } else if(updater()->getStatus() == MUpdater::UPDATE_STATUS::UPDATE_FINISHED) {
        qApp->quit();
        QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
    }
}


void DialogUeber::on_pushButtonClose_clicked()
{
    this->done(0);
}


void DialogUeber::on_radioButtonautosearchupdates_clicked(bool checked)
{
    m_updater->setAutoSearchForUpdate(checked);
    ui->radioButtonautosearchupdates->setChecked(m_updater->getAutoSearchForUpdateStatus());
}

