#include "waitingdialog.h"
#include "ui_waitingdialog.h"
#include "../program/globals.h"

WaitingDialog::WaitingDialog(QWidget * parent, QString message, bool showLandscapeThumbnailsCounter) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::WaitingDialog)
{
    ui->setupUi(this);

    if (!showLandscapeThumbnailsCounter)
    {
        ui->landscapeThumbnailsLabel1->setVisible(false);
        ui->landscapeThumbnailsLabel2->setVisible(false);
    }

    //Enlarge one of the number-holding labels so the UI
    //elements don't shift around as the numbers grow.
    ui->organismLabel2->setText("                ");
    ui->organismLabel2->setMinimumWidth(ui->organismLabel2->sizeHint().width());

    ui->messageLabel->setFont(g_extraLargeFont);
    ui->messageLabel->setText(message);
    setWindowTitle(message);
    setFixedHeight(sizeHint().height());
    setFixedWidth(sizeHint().width() * 1.3);

    g_organismsSavedOrLoaded = 0;
    g_historyOrganismsSavedOrLoaded = 0;
    g_landscapeThumbnailsMade = 0;

    connect(&m_updateTimer, SIGNAL(timeout()), this, SLOT(updateLabels()));

    m_updateTimer.start(40);
}

WaitingDialog::~WaitingDialog()
{
    delete ui;
}

void WaitingDialog::updateLabels()
{
    ui->organismLabel2->setText(this->locale().toString(g_organismsSavedOrLoaded));
    ui->historyLabel2->setText(this->locale().toString(g_historyOrganismsSavedOrLoaded));
    ui->landscapeThumbnailsLabel2->setText(this->locale().toString(g_landscapeThumbnailsMade));
}
