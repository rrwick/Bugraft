#ifndef LANDSCAPEEDITDIALOG_H
#define LANDSCAPEEDITDIALOG_H

#include <QDialog>
#include "waterlevelvisualaid.h"

class Landscape;

namespace Ui {
class LandscapeEditDialog;
}

class LandscapeEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LandscapeEditDialog(QWidget *parent = 0);
    ~LandscapeEditDialog();
    void setSettingsFromSpinBoxes();

private:
    Ui::LandscapeEditDialog *ui;
    WaterLevelVisualAid * m_waterLevelVisualAid;
    void setSpinBoxesFromSettings();
    int m_previewHeight, m_previewWidth;
    void setInfoTexts();

private slots:
    void setSlidersFromSpinBoxes();
    void setSpinBoxesFromSliders();
    void updateWaterLevelVisualAid();
    void updateLandscapePreview();
};

#endif // LANDSCAPEEDITDIALOG_H
