#ifndef GLOBALS_H
#define GLOBALS_H

#include <QFont>


enum GroundType {WATER, HABITABLE, HIGH_ALTITUDE, OUT_OF_BOUNDS};
enum SpeciesState {CURRENT, EXTINCT, SPLIT};
enum HighlightState {WHOLE_LINE, PARTIAL_LINE, NO_HIGHLIGHT};
enum ZoomSource {MOUSE_WHEEL, SPIN_BOX, KEYBOARD, GESTURE};

extern double g_zoom;

extern QFont g_largeFont;
extern QFont g_extraLargeFont;

class Settings;
extern Settings * g_settings;

class RandomNumbers;
extern RandomNumbers * g_randomNumbers;

class Landscape;
extern Landscape * g_landscape;

class History;
extern History * g_history;

class MainWindow;
extern MainWindow * g_mainWindow;

extern int g_organismsSavedOrLoaded;
extern int g_historyOrganismsSavedOrLoaded;
extern int g_landscapeThumbnailsMade;


#endif // GLOBALS_H
