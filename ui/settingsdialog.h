#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include "../program/settings.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

    void setWidgetsFromSettings();
    void setSettingsFromWidgets();

private:
    Ui::SettingsDialog *ui;

    void setInfoTexts();

    void loadOrSaveSettingsToOrFromWidgets(bool setWidgets,
                                           Settings * settings);

private slots:
    void restoreDefaults();
};

#endif // SETTINGSDIALOG_H
