#include "dialogueber.h"
#include "ui_dialogueber.h"

#include <QApplication>
#include <QFile>
#include <QMessageBox>
#include <QPalette>
#include <QProcess>
#include <QTabBar>



DialogUeber::DialogUeber(const QString &maintenanceToolPath,
                         const QString &organisation,
                         const QString &application,
                         const QString &version,
                         const QColor  &color,
                         QWidget       *parent,
                         bool           preventExitOnClose,
                         bool           autoHideUpdaterPart,
                         bool           enableStyleSheetThemes,
                         QStringList    filter)
    : QDialog(parent),
      ui(new Ui::DialogUeber),
      m_version(version),
      m_color(color),
      m_autoHideUpdaterPart(autoHideUpdaterPart)
{
    qDebug() << "DialogUeber::DialogUeber()";

    ui->setupUi(this);

    // Wenn das Elternfenster unsichtbar sein kann, darf das Schließen des
    // Update-Benachrichtigungsfensters die Anwendung nicht beenden.
    if(preventExitOnClose)
        qApp->setQuitOnLastWindowClosed(false);

    ui->labelTitel->setText(application + " " + version);
    setWindowTitle(tr("Über ") + application);

    // Organisation registrieren, damit QSettings konsistent sind
    qApp->setOrganizationName(organisation);

    // Alle optionalen Tabs standardmäßig ausblenden; nur Tab 3 (System-Info) ist immer sichtbar
    ui->label_website->setHidden(true);
    ui->label_website_value->setHidden(true);
    ui->tabWidget->setTabVisible(0, false); // Beschreibung
    ui->tabWidget->setTabVisible(1, false); // Mitwirkende
    ui->tabWidget->setTabVisible(2, false); // Lizenz
    ui->tabWidget->setTabVisible(4, false); // Fehlerlog
    ui->tabWidget->setCurrentIndex(3);      // System-Info als Standard

    // Rahmen und Trennlinie mit der Akzentfarbe einfärben
    const QString frameStyle = "{"
        "border: 1px solid " + m_color.name() + ";"
        "border-radius: 10px;"
        "padding: 1px;"
        "}";
    ui->framedebrounder->setStyleSheet("QFrame#framedebrounder" + frameStyle);
    ui->lineItemDebLine->setStyleSheet("border: 2px solid " + m_color.name() + ";");

    // Kernobjekte anlegen
    m_updater     = new MUpdater(maintenanceToolPath, organisation, application, true);
    m_styleHandler = new StyleHandler(organisation, application, enableStyleSheetThemes, filter);

    ui->labelUpdateStatus->setText(m_updater->getStatusStr());

    // Kontextmenü für den Updater-Button aufbauen
    m_autoUpdaterMenu = new QMenu(this);

    // MaintenanceTool-Eintrag nur hinzufügen, wenn das Tool auf dem System vorhanden ist
    if(QFile(m_updater->getMaintananceToolPath()).exists()) {
        m_actionStartMaintenance = new QAction(tr("MaintenanceTool starten"), m_autoUpdaterMenu);
        connect(m_actionStartMaintenance, &QAction::triggered,
                this, &DialogUeber::onActionStartMaintenanceToolClicked);
        m_autoUpdaterMenu->addAction(m_actionStartMaintenance);
    }

    m_actionAutoUpdate = new QAction(tr("Automatisch nach Updates suchen"), m_autoUpdaterMenu);
    m_actionAutoUpdate->setCheckable(true);
    m_actionAutoUpdate->setChecked(m_updater->getAutoSearchForUpdateStatus());
    connect(m_actionAutoUpdate, &QAction::triggered,
            this, &DialogUeber::onActionAutoUpdateSearchClicked);
    m_autoUpdaterMenu->addAction(m_actionAutoUpdate);

    // System-Info-Tab befüllen
    ui->label_version->setText(application + " " + version);
    ui->label_qt_version->setText("Qt " + QString(QT_VERSION_STR));
    ui->label_compile_date->setText(QString(__DATE__) + " at " + QString(__TIME__));
    ui->label_os->setText(QSysInfo::productType() + " " + QSysInfo::productVersion());
    ui->label_arch->setText(QSysInfo::currentCpuArchitecture());

    // Issue-Link standardmäßig ausblenden (wird bei setIssueWebsite() eingeblendet)
    ui->label_issue_website->hide();
    ui->label_5issuetext->hide();
    ui->label_4issue2->hide();

    // Updater-Button als Hyperlink-Look stylen (kein echtes <a>-Element möglich)
    ui->pushButtonUpdaterButton->setStyleSheet(
        "QPushButton {"
        "    border: none;"
        "    background: none;"
        "    color: lightblue;"
        "    text-decoration: underline;"
        "}"
        "QPushButton:hover  { color: blue;   }"
        "QPushButton:pressed { color: purple; }");

    // Initialen Status rendern und auf spätere Änderungen reagieren
    updaterStatusChanged();
    connect(m_updater, &MUpdater::statusChanged, this, &DialogUeber::updaterStatusChanged);
}

void DialogUeber::setDescription(const QString &websiteUrl,
                                  const QString &description,
                                  const QString &alternativeUrlName)
{
    // Domain im QSettings-System für andere Klassen verfügbar machen
    qApp->setOrganizationDomain(websiteUrl);

    if(!websiteUrl.isEmpty()) {
        // Link als farbiges <a>-Element rendern (System-Palette berücksichtigt)
        const QString linkText = alternativeUrlName.isEmpty() ? websiteUrl : alternativeUrlName;
        ui->label_website_value->setText(
            "<a style=\"text-decoration:none; color: " + m_color.name() + ";\" href=\"" +
            websiteUrl + "\">" + linkText + "</a>");
        ui->label_website_value->setHidden(false);
        ui->label_website->setHidden(false);
    }

    ui->label_description->setText(description);
    ui->tabWidget->setTabVisible(0, true);
    ui->tabWidget->setCurrentIndex(0);
}

void DialogUeber::setDescription(const QString &websiteUrl, QFile description, const QString &alternativeUrlName)
{
    if(!description.open(QIODevice::ReadOnly)) {
        qWarning() << "setDescription: open file failed!" << description.errorString();
        return;
    }
    setDescription(websiteUrl, description.readAll(), alternativeUrlName);
    description.close();
}

void DialogUeber::setContributorList(const QStringList &/*contributors*/)
{
    // TODO: Mitwirkende-Liste in ui->listWidget_contributors befüllen
    ui->tabWidget->setTabVisible(1, true);
    ui->tabWidget->setCurrentIndex(0);
}

void DialogUeber::setPixmap(const QPixmap &icon)
{
    ui->label_icon->setPixmap(icon);
}

void DialogUeber::setPixmap(const QString &iconFilePath)
{
    setPixmap(QPixmap(iconFilePath));
}


void DialogUeber::setLicence(QFile licence, bool centerWithHtml)
{
    if(!licence.open(QIODevice::ReadOnly)) {
        qWarning() << "setLicence: open file failed!" << licence.errorString();
        return;
    }
    setLicence(licence.readAll(), centerWithHtml);
    licence.close();
}

void DialogUeber::setLicence(QString licence, bool centerWithHtml)
{
    if(centerWithHtml) {
        // Zentriert als HTML darstellen; Sonderzeichen escapen, Zeilenumbrüche in Absätze wandeln
        const QString escaped = licence
            .replace("<",  "&lt;")
            .replace(">",  "&gt;")
            .replace("\n", "</span></p>"
                          "<p align=\"center\"><span style=\" font-size:8.25pt;\">");
        ui->textEdit_licence->setHtml(
            "<html><head/><body>"
            "<p align=\"center\"><span style=\" font-size:8.25pt;\">"
            + escaped +
            "</span></p></body></html>");
    } else {
        ui->textEdit_licence->setPlainText(licence);
    }

    ui->tabWidget->setTabVisible(2, true);
    ui->tabWidget->setCurrentIndex(0);
}

void DialogUeber::setIssueWebsite(const QString &url)
{
    ui->label_issue_website->setText(
        "<a style=\"text-decoration:none; color: " + m_color.name() +
        ";\" href=\"" + url + "\">hier</a>");
    ui->label_issue_website->show();
    ui->label_5issuetext->show();
    ui->label_4issue2->show();
}

void DialogUeber::setUpdaterFinishedMsgBoxFilePath(const QString &path)
{
    m_msgBoxFilePath = path;
}


DialogUeber::~DialogUeber()
{
    // QDialog übernimmt keine Ownership über m_updater / m_styleHandler
    delete m_updater;
    delete m_styleHandler;
    delete ui;
    qDebug() << "DialogUeber::~DialogUeber()";
}

MUpdater *DialogUeber::updater()
{
    return m_updater;
}

StyleHandler *DialogUeber::styleHandler()
{
    return m_styleHandler;
}

void DialogUeber::updaterStatusChanged()
{
    // Statustext immer aktualisieren und in den Log schreiben
    ui->pushButtonUpdaterButton->hide();
    ui->labelUpdateStatus->setText(updater()->getStatusStr());
    ui->plainTextEdit_updaer_error_log->appendPlainText("\n --> " + updater()->getStatusStr());
    ui->labelIi->setText("");
    ui->labelIi->setStyleSheet("");

    switch (updater()->getStatus()) {

    case MUpdater::NOT_CHECKED:
        ui->pushButtonUpdaterButton->show();
        ui->pushButtonUpdaterButton->setText(tr("Nach Updates suchen"));
        ui->labelIi->setText("↓");
        break;

    case MUpdater::UPDTAE_NEEDED:
        ui->pushButtonUpdaterButton->show();
        ui->pushButtonUpdaterButton->setText(tr("Aktualisieren"));
        ui->labelIi->setText("!");
        ui->tabWidget->setTabVisible(4, false);
        break;

    case MUpdater::UP_TO_DATE:
        ui->pushButtonUpdaterButton->show();
        ui->pushButtonUpdaterButton->setText(tr("Erneut nach Updates suchen"));
        ui->labelIi->setText("✓");
        ui->labelIi->setStyleSheet("color: green;");
        ui->tabWidget->setTabVisible(4, false);
        break;

    case MUpdater::CHECKING:
        // Kein Button während der Prüfung
        ui->tabWidget->setTabVisible(4, false);
        break;

    case MUpdater::UPDATING:
        // Kein Button während der Installation
        ui->tabWidget->setTabVisible(4, false);
        break;

    case MUpdater::UPDATE_ERROR:
        ui->pushButtonUpdaterButton->show();
        ui->pushButtonUpdaterButton->setText(tr("Erneut nach Updates suchen"));
        ui->labelIi->setText("⨯");
        ui->labelIi->setStyleSheet("color: red;");
        setUpdateErrorLog(m_updater->getExtraErrorInfo());
        break;

    case MUpdater::NO_UPDATER:
        // Wenn kein Tool vorhanden: Updater-UI optional ausblenden
        if(m_autoHideUpdaterPart) {
            ui->pushButtonMenueAutoUpdate->hide();
            ui->pushButtonUpdaterButton->hide();
        }
        break;

    case MUpdater::UPDATE_FINISHED:
        ui->pushButtonUpdaterButton->show();
        ui->pushButtonUpdaterButton->setText(tr("Programm neustarten"));
        ui->labelIi->setText("⟳");
        // Optionale Update-Infodatei anzeigen
        if(!m_msgBoxFilePath.isEmpty() && QFile(m_msgBoxFilePath).exists()) {
            QFile msgFile(m_msgBoxFilePath);
            if(msgFile.open(QIODevice::ReadOnly))
                QMessageBox::information(this, tr("Update Info"), msgFile.readAll());
        }
        break;
    }
}

void DialogUeber::setUpdateErrorLog(const QString &msg)
{
    // Fehlerlog-Tab einblenden
    ui->tabWidget->setTabVisible(4, true);
    if(!msg.isEmpty())
        ui->plainTextEdit_updaer_error_log->appendPlainText("\n Error Info: " + msg);
}

void DialogUeber::on_pushButtonUpdaterButton_clicked()
{
    const auto status = updater()->getStatus();

    if(status == MUpdater::NOT_CHECKED
    || status == MUpdater::UP_TO_DATE
    || status == MUpdater::UPDATE_ERROR)
    {
        // Neuer Prüflauf: Log leeren und Update-Check starten
        ui->plainTextEdit_updaer_error_log->clear();
        updater()->checkForUpdates();
    }
    else if(status == MUpdater::UPDTAE_NEEDED) {
        updater()->startUpdate();
    }
    else if(status == MUpdater::UPDATE_FINISHED) {
        updater()->restartApp();
    }
}


void DialogUeber::on_pushButtonClose_clicked()
{
    this->done(0);
}


void DialogUeber::on_pushButtonMenueAutoUpdate_clicked()
{
    // Dropdownmenü direkt an der Mausposition öffnen
    m_autoUpdaterMenu->exec(QCursor::pos());
}

void DialogUeber::onActionAutoUpdateSearchClicked()
{
    m_updater->setAutoSearchForUpdate(m_actionAutoUpdate->isChecked());
}

void DialogUeber::onActionStartMaintenanceToolClicked()
{
    const QString path = m_updater->getMaintananceToolPath();
    if(QFile(path).exists())
        QProcess::startDetached(path, {});
}

