#include "dialogueber.h"
#include "ui_dialogueber.h"

#include <QTime>
#include <QApplication>


DialogUeber::DialogUeber(const QString &maintananceToolPath, const QString &organisation, const QString &application, const QString &version, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogUeber),
    version(version),
    application(application)
{
    ui->setupUi(this);
    qApp->setOrganizationName(organisation);


    m_updater = new MUpdater(maintananceToolPath, organisation, application, true);
    this->ui->labelUpdateStatus->setText(m_updater->getStatusStr());
    m_updater->setAutoShowMessageBox(false);

    ui->label_version->setText("SpotifyEnhancer " + version);
    ui->label_qt_version->setText("Qt " + QString(QT_VERSION_STR));
    ui->label_compile_date->setText(QString(__DATE__) + QString(" at ") + QString(__TIME__));
    ui->label_os->setText("productType(): " + QSysInfo::productType() + " " +  QSysInfo::productVersion() + " machineHostName: '" + QSysInfo::machineHostName() + "' kernelType:" + QSysInfo::kernelType());
    ui->label_arch->setText(QSysInfo::currentCpuArchitecture());

    updaterStatusChanged();
    QObject::connect(m_updater, SIGNAL(statusChanged()), this, SLOT(updaterStatusChanged()));
}


void DialogUeber::setDescription(const QUrl &website, const QString &descrition)
{
    qApp->setOrganizationDomain(website.toString());



}

void DialogUeber::setContributorList(const QStringList &contributors)
{


}

void DialogUeber::setPixmap(const QPixmap &ico)
{
    this->ui->label_icon->setPixmap(ico);
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

    switch (updater()->getStatus()) {
    case MUpdater::NOT_CHECKED:
        ui->pushButtonUpdaterButton->show();
        this->ui->pushButtonUpdaterButton->setText("Nach Updates suchen");
        break;
    case MUpdater::UPDTAE_NEEDED:
        ui->pushButtonUpdaterButton->show();
        this->ui->pushButtonUpdaterButton->setText("Aktualisieren");
        break;
    case MUpdater::UP_TO_DATE:
        ui->labelUpdateStatus->setText("Sie verwenden die neuste Version!");
        ui->pushButtonUpdaterButton->show();
        break;
    case MUpdater::UPDATING: {
        ui->pushButtonUpdaterButton->show();
        this->ui->pushButtonUpdaterButton->setText("Neustarten");
        break;
    }
    default:
        break;
    }
}

void DialogUeber::delay(int sec)
{
    QTime dieTime= QTime::currentTime().addSecs(sec);
    while (QTime::currentTime() < dieTime)
        QApplication::processEvents(QEventLoop::AllEvents, 100);
}


void DialogUeber::on_pushButtonUpdaterButton_clicked()
{
    if(updater()->getStatus() == MUpdater::UPDATE_STATUS::NOT_CHECKED || updater()->getStatus() == MUpdater::UPDATE_STATUS::UP_TO_DATE) {
        updater()->checkForUpdates();
    } else if(updater()->getStatus() == MUpdater::UPDATE_STATUS::UPDTAE_NEEDED) {
        updater()->startUpdate();
    }
}


void DialogUeber::on_pushButtonClose_clicked()
{
    this->done(0);
}

