#ifndef WAITINGDIALOG_H
#define WAITINGDIALOG_H

#include <QDialog>
#include <QString>
#include <QTimer>
#include "../program/globals.h"

namespace Ui {
class WaitingDialog;
}

class WaitingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WaitingDialog(QWidget * parent, QString message, bool showLandscapeThumbnailsCounter);
    ~WaitingDialog();

private:
    Ui::WaitingDialog * ui;
    QTimer m_updateTimer;

private slots:
    void updateLabels();
};

#endif // WAITINGDIALOG_H
