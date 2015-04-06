#include "landscapeeditdialog.h"
#include "ui_landscapeeditdialog.h"

#include "../program/globals.h"
#include "../program/settings.h"
#include <math.h>
#include "../program/landscape.h"
#include "infotextwidget.h"

LandscapeEditDialog::LandscapeEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LandscapeEditDialog)
{
    ui->setupUi(this);

    setInfoTexts();

    m_waterLevelVisualAid = new WaterLevelVisualAid(g_settings->baseWaterLevel, g_settings->waterLevelVariationMagnitude,
                                                    g_settings->waterLevelVariationRate);
    ui->visualAidFrame->layout()->addWidget(m_waterLevelVisualAid);

    m_previewHeight = ui->landscapeSettingSpinBoxesWidget->sizeHint().height();
    m_previewWidth = m_previewHeight * double(g_settings->environmentWidth) / double(g_settings->environmentHeight);
    ui->landscapePreviewLabel->setFixedHeight(m_previewHeight);
    ui->landscapePreviewLabel->setFixedWidth(m_previewWidth);

    ui->visualAidFrame->setFixedHeight(m_previewHeight);
    ui->visualAidFrame->setFixedWidth(m_previewWidth);


    //Make the sliders have a range that matches with their corresponding spin boxes
    ui->waterLevelSlider->setMinimum(ui->waterLevelSpinBox->minimum() * pow(10, ui->waterLevelSpinBox->decimals()));
    ui->waterLevelSlider->setMaximum(ui->waterLevelSpinBox->maximum() * pow(10, ui->waterLevelSpinBox->decimals()));
    ui->featureSizeSlider->setMinimum(ui->featureSizeSpinBox->minimum() * pow(10, ui->featureSizeSpinBox->decimals()));
    ui->featureSizeSlider->setMaximum(ui->featureSizeSpinBox->maximum() * pow(10, ui->featureSizeSpinBox->decimals()));
    ui->roughnessSlider->setMinimum(ui->roughnessSpinBox->minimum() * pow(10, ui->roughnessSpinBox->decimals()));
    ui->roughnessSlider->setMaximum(ui->roughnessSpinBox->maximum() * pow(10, ui->roughnessSpinBox->decimals()));
    ui->waterLevelVariationMagnitudeSlider->setMinimum(ui->waterLevelVariationMagnitudeSpinBox->minimum());
    ui->waterLevelVariationMagnitudeSlider->setMaximum(ui->waterLevelVariationMagnitudeSpinBox->maximum());
    ui->waterLevelVariationRateSlider->setMinimum(ui->waterLevelVariationRateSpinBox->minimum());
    ui->waterLevelVariationRateSlider->setMaximum(ui->waterLevelVariationRateSpinBox->maximum());
    ui->landscapeShapeVariationRateSlider->setMinimum(ui->landscapeShapeVariationRateSpinBox->minimum());
    ui->landscapeShapeVariationRateSlider->setMaximum(ui->landscapeShapeVariationRateSpinBox->maximum());

    setSpinBoxesFromSettings();
    setSlidersFromSpinBoxes();

    connect(ui->waterLevelSlider, SIGNAL(valueChanged(int)), this, SLOT(setSpinBoxesFromSliders()));
    connect(ui->waterLevelSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setSlidersFromSpinBoxes()));
    connect(ui->featureSizeSlider, SIGNAL(valueChanged(int)), this, SLOT(setSpinBoxesFromSliders()));
    connect(ui->featureSizeSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setSlidersFromSpinBoxes()));
    connect(ui->roughnessSlider, SIGNAL(valueChanged(int)), this, SLOT(setSpinBoxesFromSliders()));
    connect(ui->roughnessSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setSlidersFromSpinBoxes()));
    connect(ui->waterLevelVariationMagnitudeSlider, SIGNAL(valueChanged(int)), this, SLOT(setSpinBoxesFromSliders()));
    connect(ui->waterLevelVariationMagnitudeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setSlidersFromSpinBoxes()));
    connect(ui->waterLevelVariationRateSlider, SIGNAL(valueChanged(int)), this, SLOT(setSpinBoxesFromSliders()));
    connect(ui->waterLevelVariationRateSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setSlidersFromSpinBoxes()));
    connect(ui->landscapeShapeVariationRateSlider, SIGNAL(valueChanged(int)), this, SLOT(setSpinBoxesFromSliders()));
    connect(ui->landscapeShapeVariationRateSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setSlidersFromSpinBoxes()));
    connect(ui->waterLevelSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateWaterLevelVisualAid()));
    connect(ui->waterLevelVariationMagnitudeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateWaterLevelVisualAid()));
    connect(ui->waterLevelVariationRateSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateWaterLevelVisualAid()));
    connect(ui->roughnessSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateLandscapePreview()));
    connect(ui->featureSizeSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateLandscapePreview()));
    connect(ui->waterLevelSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateLandscapePreview()));

    updateLandscapePreview();
}

LandscapeEditDialog::~LandscapeEditDialog()
{
    delete ui;
}


void LandscapeEditDialog::setSettingsFromSpinBoxes()
{
    double newPerlinFrequency = 1.0 / ui->featureSizeSpinBox->value();
    double scaleRatio = g_settings->perlinFrequency / newPerlinFrequency;

    g_settings->baseWaterLevel = ui->waterLevelSpinBox->value();
    g_settings->perlinFrequency = newPerlinFrequency;
    g_settings->perlinPersistence = ui->roughnessSpinBox->value() / 2.0 + 0.2;
    g_settings->waterLevelVariationMagnitude = ui->waterLevelVariationMagnitudeSpinBox->value();
    g_settings->waterLevelVariationRate = ui->waterLevelVariationRateSpinBox->value();
    g_settings->landscapeShapeVariationRate = ui->landscapeShapeVariationRateSpinBox->value();

    g_settings->perlinZValue *= scaleRatio;
}

void LandscapeEditDialog::setSpinBoxesFromSettings()
{
    ui->waterLevelSpinBox->setValue(g_settings->baseWaterLevel);
    ui->featureSizeSpinBox->setValue(1.0 / g_settings->perlinFrequency);
    ui->roughnessSpinBox->setValue(2.0 * (g_settings->perlinPersistence - 0.2));
    ui->waterLevelVariationMagnitudeSpinBox->setValue(g_settings->waterLevelVariationMagnitude);
    ui->waterLevelVariationRateSpinBox->setValue(g_settings->waterLevelVariationRate);
    ui->landscapeShapeVariationRateSpinBox->setValue(g_settings->landscapeShapeVariationRate);
}

void LandscapeEditDialog::setSlidersFromSpinBoxes()
{
    ui->waterLevelSlider->setValue(ui->waterLevelSpinBox->value() * pow(10, ui->waterLevelSpinBox->decimals()));
    ui->featureSizeSlider->setValue(ui->featureSizeSpinBox->value() * pow(10, ui->featureSizeSpinBox->decimals()));
    ui->roughnessSlider->setValue(ui->roughnessSpinBox->value() * pow(10, ui->roughnessSpinBox->decimals()));
    ui->waterLevelVariationMagnitudeSlider->setValue(ui->waterLevelVariationMagnitudeSpinBox->value());
    ui->waterLevelVariationRateSlider->setValue(ui->waterLevelVariationRateSpinBox->value());
    ui->landscapeShapeVariationRateSlider->setValue(ui->landscapeShapeVariationRateSpinBox->value());
}

void LandscapeEditDialog::setSpinBoxesFromSliders()
{
    ui->waterLevelSpinBox->setValue(ui->waterLevelSlider->value() / pow(10, ui->waterLevelSpinBox->decimals()));
    ui->featureSizeSpinBox->setValue(ui->featureSizeSlider->value() / pow(10, ui->featureSizeSpinBox->decimals()));
    ui->roughnessSpinBox->setValue(ui->roughnessSlider->value() / pow(10, ui->roughnessSpinBox->decimals()));
    ui->waterLevelVariationMagnitudeSpinBox->setValue(ui->waterLevelVariationMagnitudeSlider->value());
    ui->waterLevelVariationRateSpinBox->setValue(ui->waterLevelVariationRateSlider->value());
    ui->landscapeShapeVariationRateSpinBox->setValue(ui->landscapeShapeVariationRateSlider->value());
}

void LandscapeEditDialog::updateWaterLevelVisualAid()
{
    m_waterLevelVisualAid->m_baseWaterLevel = ui->waterLevelSpinBox->value();
    m_waterLevelVisualAid->m_waterLevelVariationMagnitude = ui->waterLevelVariationMagnitudeSpinBox->value();
    m_waterLevelVisualAid->m_waterLevelVariationRate = ui->waterLevelVariationRateSpinBox->value();
    m_waterLevelVisualAid->update();
}

void LandscapeEditDialog::updateLandscapePreview()
{
    double newPerlinFrequency = 1.0 / ui->featureSizeSpinBox->value();
    double scaleRatio = g_settings->perlinFrequency / newPerlinFrequency;
    double newPerlinPersistence = ui->roughnessSpinBox->value() / 2.0 + 0.2;
    double newPerlinZ = g_settings->perlinZValue * scaleRatio;
    double newWaterLevel = ui->waterLevelSpinBox->value();

    ui->landscapePreviewLabel->setPixmap(g_landscape->getLandscapeThumbnail(m_previewWidth, m_previewHeight,
                                                                            LandscapeParameters(newPerlinFrequency, newPerlinPersistence,
                                                                                                newPerlinZ, newWaterLevel)));
}

void LandscapeEditDialog::setInfoTexts()
{
    ui->waterLevelInfoText->setInfoText("This is the average water level in the landscape.<br><br>"
                                        "If either of the following two settings are zero, then the "
                                        "water level will be constant at this value.<br><br>"
                                        "If the following two settings are both above zero, then the "
                                        "water level will vary around this level.");
    ui->waterLevelVariationMagnitudeInfoText->setInfoText("This is the degree to which the water level "
                                                          "will vary. It corresponds to the difference "
                                                          "between the minimum and maximum water levels.<br><br>"
                                                          "The following setting must have a non-zero level for "
                                                          "this setting to have any effect.");
    ui->waterLevelVariationRateInfoText->setInfoText("This is the speed at which the water level "
                                                     "will vary. A low value results in gradual changes of "
                                                     "water level while a high value results in rapid "
                                                     "changes.<br><br>"
                                                     "The previous setting must have a non-zero level for "
                                                     "this setting to have any effect.");
    ui->featureSizeInfoText->setInfoText("This controls the size of the landscape's features, such as islands "
                                         "and seas.<br><br>"
                                         "A small value will result in many smaller features. A large "
                                         "value will give fewer larger features.");
    ui->roughnessInfoText->setInfoText("This controls the roughness of the landscape's shape.<br><br>"
                                       "A low value will result in smooth features.  A large value will "
                                       "result in more complex shapes.");
    ui->landscapeShapeVariationRateInfoText->setInfoText("This controls the rate at which the shape of the "
                                                         "landscape changes.<br><br>"
                                                         "When this value is zero, the shape of the landscape "
                                                         "will not change. When low, it will change slowly. "
                                                         "When high, it will change quickly.");
}
