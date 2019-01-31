#ifndef LANDSCAPE_H
#define LANDSCAPE_H

#include <vector>
#include <list>
#include <map>
#include <utility> //std::pair
#include <QPixmap>
#include <QColor>
#include <QGraphicsScene>

#include "../libnoise/noise.h"
#include "../bug/organism.h"
#include "globals.h"
#include "../bug/species.h"
#include "landscapeparameters.h"

#ifndef Q_MOC_RUN
#include "boost/serialization/list.hpp"
#include "boost/serialization/vector.hpp"
#include "boost/archive/text_iarchive.hpp"
#include "boost/archive/text_oarchive.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/serialization/shared_ptr.hpp"
#endif // Q_MOC_RUN
namespace boost {namespace serialization {class access;}}

class Landscape
{
public:
    Landscape() : m_elapsedTime(0), m_startingSpecies(0), m_scene(0) {}
    Landscape(QGraphicsScene * scene);
    ~Landscape();

    long long m_elapsedTime;
    boost::shared_ptr<Species> m_startingSpecies;
    std::list<Organism *> m_organisms; //The main pool of organisms
    std::vector<Organism *> m_newOrganisms; //This holds organisms that are made in a day.  After the day is done, they'll be added to m_organisms.
    QGraphicsScene * m_scene;

    QPixmap makePixmap();
    void updateLandscape();
    void updatePerlinNoiseAndWaterLevel();
    void cleanUp();
    void reset();
    void checkSpecies();
    void addNewOrganism(Organism * newOrganism) {m_newOrganisms.push_back(newOrganism);}
    void advanceOneTick();
    void createOrganisms();
    void setAllOrganismPositions();
    bool populationIsExtinct();
    bool populationIsNotExtinct() {return !populationIsExtinct();}
    bool pointIsHabitable(double x, double y) {return !pointIsNotHabitable(x, y);}
    bool pointIsNotHabitable(double x, double y);
    bool pointIsOutOfBounds(double x, double y);
    GroundType getGroundType(double x, double y);
    Organism * addToCollisionVector(int x, int y, Organism * organismToAdd);
    void clearCollisionVector(int x, int y);
    double getWaterLevelFromPerlinFunction(double mean, int magnitude, double z);
    QRgb getPixelColor(double x, double y);
    QString getNewickTree();
    QPixmap getLandscapeThumbnail(int width, int height);
    QPixmap getLandscapeThumbnail(int width, int height, LandscapeParameters parameters);
    std::map<int, boost::shared_ptr<Genome>> getRandomGenomeForEachSpecies();
    std::map<int, std::vector< std::pair<float, float> > > getOrganismLocationsForEachSpecies();
    void rebuildAfterLoad();
    void addOrganismsAfterLoad();
    LandscapeParameters getCurrentParameters();

private:
    double m_currentWaterLevel;
    noise::module::Perlin m_perlinAltitude;
    noise::module::Perlin m_perlinBrightness;
    noise::module::Perlin m_perlinWaterLevel;
    std::vector< std::vector<double> > m_altitude;
    std::vector< std::vector<int> > m_brightness;
    std::vector< std::vector<Organism *> > m_collisions;

    double getAltitudeFromPerlinFunction(double x, double y);
    double getAltitudeFromPerlinFunction(double x, double y, double z, noise::module::Perlin perlinAltitude);
    int getBrightnessFromPerlinFunction(double x, double y);
    double getWaterLevelFromPerlinFunction();
    double getAltitudeFromVector(int x, int y);
    int getBrightnessFromVector(int x, int y);
    QRgb getPixelColor(int x, int y);
    QRgb getPixelColor(double x, double y, double z, noise::module::Perlin perlinAltitude, double waterLevel);
    QRgb getPixelColor(double altitude, int brightness);
    void setAltitudeVector();
    void setBrightnessVector();
    void prepareCollisionVector();
    void moveAllOrganisms();
    void removeDeadOrganisms();
    void checkForCollisions();
    void addNewOrganisms();
    void setUpPerlinNoise();
    void getColorIndices(double altitude, int * i1, int * i2, double * fraction);
    QColor blendColors(QColor color1, QColor color2, double fraction);
    double blendDoubles(double d1, double d2, double fraction);
    std::map< int, std::list<Organism *> > groupOrganismsIntoSpeciesList();
    std::map< int, std::vector<Organism *> > groupOrganismsIntoSpeciesVector();
    void checkOneSpeciesForSplit(std::list<Organism *> list1, bool calledBySelf);
    void checkForExtinctSpecies(boost::shared_ptr<Species> species, std::map< int, std::list<Organism *> > * speciesGroups);

    friend class boost::serialization::access;
    template<typename Archive>
    void serialize(Archive & ar, const unsigned)
    {
        ar & m_elapsedTime;
        ar & m_startingSpecies;
        ar & m_organisms;
        ar & m_currentWaterLevel;
    }
};

#endif // LANDSCAPE_H
