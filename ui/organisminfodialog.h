#ifndef ORGANISMINFODIALOG_H
#define ORGANISMINFODIALOG_H

#include "../bug/organism.h"

#include <QDialog>
#include "singleorganismwidget.h"

namespace Ui {
class OrganismInfoDialog;
}

class OrganismInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OrganismInfoDialog(Organism * organism, QWidget *parent = 0);
    ~OrganismInfoDialog();

private:
    Ui::OrganismInfoDialog *ui;

    Organism * m_organism;
    SingleOrganismWidget * singleOrganismWidget;
};

#endif // ORGANISMINFODIALOG_H
