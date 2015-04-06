#include "organisminfodialog.h"
#include "ui_organisminfodialog.h"
#include <QPalette>


OrganismInfoDialog::OrganismInfoDialog(Organism * organism, QWidget * parent) :
    QDialog(parent), ui(new Ui::OrganismInfoDialog), m_organism(organism)
{
    ui->setupUi(this);

    QPalette palette = ui->genomeFrame->palette();
    palette.setColor(backgroundRole(), QColor(255, 255, 255));
    ui->genomeFrame->setPalette(palette);
    ui->genomeFrame->setAutoFillBackground(true);

    singleOrganismWidget = new SingleOrganismWidget(m_organism, this);
    ui->frame->layout()->addWidget(singleOrganismWidget);

    ui->organismAgeLabel->setText(this->locale().toString(m_organism->getAge()));
    ui->genomeTextLabel->setText(m_organism->m_genome->getString());
    ui->speciesNumberLabel->setText(this->locale().toString(m_organism->m_species->m_speciesNumber));
}

OrganismInfoDialog::~OrganismInfoDialog()
{
    delete ui;
}
