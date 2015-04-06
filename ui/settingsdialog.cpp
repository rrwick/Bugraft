#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "../program/globals.h"
#include "infotextwidget.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    ui->mainTitleLabel->setFont(g_extraLargeFont);
    ui->populationLabel->setFont(g_largeFont);
    ui->speciationLabel->setFont(g_largeFont);
    ui->mobilityLabel->setFont(g_largeFont);

    setInfoTexts();

    connect(ui->okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(ui->restoreDefaultsButton, SIGNAL(clicked()), this, SLOT(restoreDefaults()));
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::setInfoTexts()
{
    ui->startingOrganismCountInfoText->setInfoText("This is the number of organisms initially put "
                                                   "into the environment when the simulation is reset.");
    ui->meanOrganismLifespanInfoText->setInfoText("Organisms will die after this amount of time has "
                                                  "passed, on average.");
    ui->bumpFreeTimeBeforeGivingBirthInfoText->setInfoText("Fertilised organisms will reproduce if "
                                                           "this much time passes without encountering "
                                                           "another organism.<br><br>"
                                                           "Low values lead to dense populations, high "
                                                           "values to sparse populations.");
    ui->chanceOfMutationInfoText->setInfoText("When two organisms produce an offspring, this is the "
                                              "chance of a random mutation occurring in one of the "
                                              "offspring's genes.");
    ui->maxMutationSizeInfoText->setInfoText("When a mutation does occur, it will have a random magnitude "
                                             "no larger than this value.");
    ui->maximumDistanceForMatingInfoText->setInfoText("If two organisms have a total genetic difference "
                                                      "of less than this value, then they can mate.<br><br>"
                                                      "A low value means that only organisms that are very "
                                                      "close genetically will be able to mate.   A high value "
                                                      "means that organisms with more genetic differences can "
                                                      "mate.<br><br>"
                                                      "This setting and the next define how much variation "
                                                      "can occur within a species.");
    ui->maximumDistanceForSameSpeciesInfoText->setInfoText("If within a species two groups of individuals are "
                                                           "separated by this much genetic difference, then "
                                                           "they will be reclassified as two separate "
                                                           "species.<br><br>"
                                                           "A low value means that speciation will occur earlier "
                                                           "when fewer genetic differences have accumulated.  A "
                                                           "high value will result in speciation occurring "
                                                           "later.<br><br>"
                                                           "This setting should be at least somewhat larger than "
                                                           "the previous setting (Maximum difference to mate), "
                                                           "or else individuals from different species may be "
                                                           "able to mate.<br><br>"
                                                           "This setting should be considerably larger than the "
                                                           "'Max mutation size' setting, or else new species will "
                                                           "be defined too frequently.");
    ui->chanceOfDriftingOverWaterInfoText->setInfoText("When an organism bumps up against the coast, this "
                                                       "is their chance of beginning to drift over the water.<br><br>"
                                                       "A low value results in infrequent colonisation of land "
                                                       "separated by water.  A high value results in frequent "
                                                       "colonisation.");
    ui->chanceOfDriftingOverAltitudeInfoText->setInfoText("When an organism bumps up against the maximum habitable "
                                                          "altitude, this is their chance of beginning to drift "
                                                          "over the high altitude region.<br><br>"
                                                          "A low value results in infrequent colonisation of land "
                                                          "separated by high altitude.  A high value results in "
                                                          "frequent colonisation.");
    ui->maxHabitableAltitudeAboveWaterLevelInfoText->setInfoText("This is the maximum altitude (measured as "
                                                                 "height above the water level) where organisms "
                                                                 "can live.<br><br>"
                                                                 "Regions of altitude higher than this will "
                                                                 "form barriers, except for the occasions where "
                                                                 "an organism drifts.");
}


void SettingsDialog::restoreDefaults()
{
    Settings defaultSettings;
    loadOrSaveSettingsToOrFromWidgets(true, &defaultSettings);
}


void SettingsDialog::setWidgetsFromSettings()
{
    loadOrSaveSettingsToOrFromWidgets(true, g_settings);
}

void SettingsDialog::setSettingsFromWidgets()
{
    loadOrSaveSettingsToOrFromWidgets(false, g_settings);
}


//These functions either set a spin box to a value or set the value to the spin box.  Pointers to
//these functions will be passed to loadOrSaveSettingsToOrFromWidgets, so that one function can
//take care of both save and load functionality.
void setOneSettingFromWidget(double * setting, QDoubleSpinBox * widget, bool percentage) {*setting = widget->value() / (percentage ? 100.0 : 1.0);}
void setOneSettingFromWidget(int * setting, QSpinBox * widget) {*setting = widget->value();}
void setOneWidgetFromSetting(double * setting, QDoubleSpinBox * widget, bool percentage) {widget->setValue(*setting * (percentage ? 100.0 : 1.0));}
void setOneWidgetFromSetting(int * setting, QSpinBox * widget) {widget->setValue(*setting);}


void SettingsDialog::loadOrSaveSettingsToOrFromWidgets(bool setWidgets, Settings * settings)
{
    void (*doubleFunctionPointer)(double *, QDoubleSpinBox *, bool);
    void (*intFunctionPointer)(int *, QSpinBox *);
    if (setWidgets)
    {
        doubleFunctionPointer = setOneWidgetFromSetting;
        intFunctionPointer = setOneWidgetFromSetting;
    }
    else
    {
        doubleFunctionPointer = setOneSettingFromWidget;
        intFunctionPointer = setOneSettingFromWidget;
    }

    intFunctionPointer(&settings->startingOrganismCount, ui->startingOrganismCountSpinBox);
    doubleFunctionPointer(&settings->meanOrganismLifespan, ui->meanOrganismLifespanSpinBox, false);
    intFunctionPointer(&settings->bumpFreeTimeBeforeGivingBirth, ui->bumpFreeTimeBeforeGivingBirthSpinBox);

    doubleFunctionPointer(&settings->chanceOfMutation, ui->chanceOfMutationSpinBox, true);
    doubleFunctionPointer(&settings->maxMutationSize, ui->maxMutationSizeSpinBox, false);
    doubleFunctionPointer(&settings->maximumDistanceForMating, ui->maximumDistanceForMatingSpinBox, false);
    doubleFunctionPointer(&settings->maximumDistanceForSameSpecies, ui->maximumDistanceForSameSpeciesSpinBox, false);

    doubleFunctionPointer(&settings->chanceOfDriftingOverWater, ui->chanceOfDriftingOverWaterSpinBox, true);
    doubleFunctionPointer(&settings->chanceOfDriftingOverAltitude, ui->chanceOfDriftingOverAltitudeSpinBox, true);
    doubleFunctionPointer(&settings->maxHabitableAltitudeAboveWaterLevel, ui->maxHabitableAltitudeAboveWaterLevelSpinBox, false);
}
