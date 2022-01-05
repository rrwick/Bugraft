#include "landscape.h"

#include "globals.h"
#include "randomnumbers.h"
#include "settings.h"
#include <utility> //std::pair
#include "../ui/mainwindow.h"
#include <algorithm> // std::shuffle
#include <random> // std::mt19937


Landscape::Landscape(QGraphicsScene * scene) :
    m_elapsedTime(0), m_scene(scene)
{
    m_perlinBrightness.SetOctaveCount(1);
    m_perlinBrightness.SetFrequency(157);
    m_perlinWaterLevel.SetOctaveCount(1);

    setUpPerlinNoise();

    setBrightnessVector();
    prepareCollisionVector();
}

Landscape::~Landscape()
{
    cleanUp();
}


void Landscape::reset()
{
    m_elapsedTime = 0;
    cleanUp();
    setUpPerlinNoise();
    createOrganisms();
}

void Landscape::cleanUp()
{
    //This clears out the m_organisms vector, but the actual
    //deleting of the organisms is done when the EnvironmentGraphicsScene
    //is cleared.

    m_organisms.clear();
    m_startingSpecies.reset();
}


void Landscape::setUpPerlinNoise()
{
    g_settings->perlinZValue = g_randomNumbers->getRandomDouble(-1000000.0, 1000000.0);

    g_settings->waterLevelPerlinXValue = g_randomNumbers->getRandomDouble(-1000.0, 1000.0);
    g_settings->waterLevelPerlinYValue = g_randomNumbers->getRandomDouble(-1000.0, 1000.0);

    //It is desirable to start with a water level Perlin noise value that is close to zero,
    //so keep trying until one is found.
    double waterLevelPerlinValue;
    do
    {
        g_settings->waterLevelPerlinZValue = g_randomNumbers->getRandomDouble(-1000000.0, 1000000.0);
        waterLevelPerlinValue = m_perlinWaterLevel.GetValue(g_settings->waterLevelPerlinXValue,
                                                            g_settings->waterLevelPerlinYValue,
                                                            g_settings->waterLevelPerlinZValue);
    } while (waterLevelPerlinValue > 0.01 ||
             waterLevelPerlinValue < -0.01);

    updateLandscape();
}


void Landscape::rebuildAfterLoad()
{
    m_scene = &g_mainWindow->m_graphicsScene;

    m_perlinBrightness.SetOctaveCount(1);
    m_perlinBrightness.SetFrequency(157);
    m_perlinWaterLevel.SetOctaveCount(1);

    updatePerlinNoiseAndWaterLevel();

    setAltitudeVector();
    setBrightnessVector();
}


void Landscape::addOrganismsAfterLoad()
{
    for (std::list<Organism *>::iterator i = m_organisms.begin(); i != m_organisms.end(); ++i)
    {
        m_scene->addItem(*i);
        (*i)->setFlag(QGraphicsItem::ItemIsMovable);
    }
    setAllOrganismPositions();
}


void Landscape::advanceOneTick()
{
    moveAllOrganisms();
    removeDeadOrganisms();
    checkForCollisions();
    addNewOrganisms();
}

void Landscape::moveAllOrganisms()
{
    for (std::list<Organism *>::iterator i = m_organisms.begin(); i != m_organisms.end(); ++i)
        (*i)->move();
}

void Landscape::removeDeadOrganisms()
{
    for (std::list<Organism *>::iterator i = m_organisms.begin(); i != m_organisms.end();)
    {
        if ((*i)->hasDied())
        {
            delete *i;
            i = m_organisms.erase(i);
        }
        else
            ++i;
    }
}

void Landscape::setAllOrganismPositions()
{
    for (std::list<Organism *>::iterator i = m_organisms.begin(); i != m_organisms.end(); ++i)
        (*i)->setPosition();
}

void Landscape::checkForCollisions()
{
    for (std::list<Organism *>::iterator i = m_organisms.begin(); i != m_organisms.end(); ++i)
        (*i)->checkForCollisions();
    for (std::list<Organism *>::iterator i = m_organisms.begin(); i != m_organisms.end(); ++i)
        (*i)->clearCollisionVector();
}

void Landscape::addNewOrganisms()
{
    for (size_t i = 0; i < m_newOrganisms.size(); ++i)
    {
        m_organisms.push_back(m_newOrganisms[i]);
        m_scene->addItem(m_newOrganisms[i]);
    }
    m_newOrganisms.clear();
}

void Landscape::updateLandscape()
{
    updatePerlinNoiseAndWaterLevel();
    setAltitudeVector();
}

void Landscape::updatePerlinNoiseAndWaterLevel()
{
    m_perlinAltitude.SetOctaveCount(g_settings->perlinOctaves);
    m_perlinAltitude.SetFrequency(g_settings->perlinFrequency);
    m_perlinAltitude.SetPersistence(g_settings->perlinPersistence);

    m_currentWaterLevel = getWaterLevelFromPerlinFunction();
}


void Landscape::createOrganisms()
{
    //Find a location for the starting organisms.
    //This is done by looking for a circular area that is all
    //inhabitable.  If none is found, then the area with the
    //largest inhabitable proportion is used.

    //Create a vector of all points in a circular region, centred
    //on (0, 0).
    std::vector< std::pair<int, int> > circularRegionPositions;
    int radius = g_settings->startingOrganismCircleRadius;
    for (int i = -radius; i <= radius; ++i)
    {
        for (int j = -radius; j <= radius; ++j)
        {
            if (i * i + j * j < radius * radius)
                circularRegionPositions.push_back( std::pair<int, int>(i, j) );
        }
    }

    //Make many attempts to find a good starting location.
    int bestX = 0;
    int bestY = 0;
    double bestFractionHabitable = 0.0;
    for (int i = 0; i < g_settings->startingOrganismPositionAttempts; ++i)
    {
        int xCentre = g_randomNumbers->getRandomInt(radius, g_settings->environmentWidth - radius);
        int yCentre = g_randomNumbers->getRandomInt(radius, g_settings->environmentHeight - radius);

        int habitablePositionCount = 0;
        for (size_t j = 0; j < circularRegionPositions.size(); ++j)
        {
            int x = xCentre + circularRegionPositions[j].first;
            int y = yCentre + circularRegionPositions[j].second;

            if (pointIsHabitable(x, y))
                ++habitablePositionCount;
        }

        double fractionHabitable = double(habitablePositionCount) / circularRegionPositions.size();
        if (fractionHabitable > bestFractionHabitable)
        {
            bestX = xCentre;
            bestY = yCentre;
            bestFractionHabitable = fractionHabitable;
        }
        if (habitablePositionCount == int(circularRegionPositions.size()))
            break;
    }

    boost::shared_ptr<Genome> randomStartingGenome(new Genome());
    m_startingSpecies.reset(new Species(0, m_elapsedTime));

    //Stop now if the function failed to find a habitable region.
    //This might happen if the landscape has little to no habitable
    //area.
    if (bestFractionHabitable == 0.0)
        return;

    for (int i = 0; i < g_settings->startingOrganismCount; ++i)
    {
        int newOrganismX, newOrganismY;
        do
        {
            int randomLocation = g_randomNumbers->getRandomInt(0, int(circularRegionPositions.size()) - 1);
            newOrganismX = bestX + circularRegionPositions[randomLocation].first;
            newOrganismY = bestY + circularRegionPositions[randomLocation].second;
        } while (pointIsNotHabitable(newOrganismX, newOrganismY));

        Organism * newOrganism = new Organism(newOrganismX, newOrganismY, randomStartingGenome, m_startingSpecies);
        m_organisms.push_back(newOrganism);
        m_scene->addItem(newOrganism);
    }

    setAllOrganismPositions();
}


void Landscape::setAltitudeVector()
{
    m_altitude.resize(g_settings->environmentWidth);

    for (int i = 0; i < g_settings->environmentWidth; ++i)
    {
        m_altitude[i].resize(g_settings->environmentHeight);

        for (int j = 0; j < g_settings->environmentHeight; ++j)
            m_altitude[i][j] = getAltitudeFromPerlinFunction(i, j);

    }
}

void Landscape::setBrightnessVector()
{
    m_brightness.resize(g_settings->environmentWidth);

    for (int i = 0; i < g_settings->environmentWidth; ++i)
    {
        m_brightness[i].resize(g_settings->environmentHeight);

        for (int j = 0; j < g_settings->environmentHeight; ++j)
            m_brightness[i][j] = getBrightnessFromPerlinFunction(i, j);

    }
}

void Landscape::prepareCollisionVector()
{
    m_collisions.resize(g_settings->environmentWidth);

    for (int i = 0; i < g_settings->environmentWidth; ++i)
    {
        m_collisions[i].resize(g_settings->environmentHeight);

        for (int j = 0; j < g_settings->environmentHeight; ++j)
            m_collisions[i][j] = 0;
    }
}


//This function can be called with just x and y, in which case the current perlin module
//will be used with the current z value.  Or it can be called specifying a perlin module
//and a z value.
double Landscape::getAltitudeFromPerlinFunction(double x, double y)
{
    //This should give altitudes that range mostly from 0 to 100
    return 50.0 + 50.0 * m_perlinAltitude.GetValue(x / 250.0, y / 250.0, g_settings->perlinZValue);
}
double Landscape::getAltitudeFromPerlinFunction(double x, double y, double z, noise::module::Perlin perlinAltitude)
{
    //This should give altitudes that range mostly from 0 to 100
    return 50.0 + 50.0 * perlinAltitude.GetValue(x / 250.0, y / 250.0, z);
}

int Landscape::getBrightnessFromPerlinFunction(double x, double y)
{
    return int(20.0 * m_perlinBrightness.GetValue(x / 250.0, y / 250.0, g_settings->perlinZValue));
}


double Landscape::getWaterLevelFromPerlinFunction()
{
    return getWaterLevelFromPerlinFunction(g_settings->baseWaterLevel,
                                           g_settings->waterLevelVariationMagnitude,
                                           g_settings->waterLevelPerlinZValue);
}

double Landscape::getWaterLevelFromPerlinFunction(double mean, int magnitude, double z)
{
    double waterLevel =  mean +
                         magnitude * 0.3 *
                         m_perlinWaterLevel.GetValue(g_settings->waterLevelPerlinXValue,
                                                     g_settings->waterLevelPerlinYValue,
                                                     z);
    if (waterLevel > 100.0)
        return 100.0;
    else if (waterLevel < 0.0)
        return 0.0;
    else
        return waterLevel;
}

double Landscape::getAltitudeFromVector(int x, int y)
{
    return m_altitude[x][y];
}

int Landscape::getBrightnessFromVector(int x, int y)
{
    return m_brightness[x][y];
}

QPixmap Landscape::makePixmap()
{
    QImage image(g_settings->environmentWidth, g_settings->environmentHeight, QImage::Format_RGB32);

    //Set each pixel
    for (int i = 0; i < g_settings->environmentWidth; ++i)
    {
        for (int j = 0; j < g_settings->environmentHeight; ++j)
            image.setPixel(i, j, getPixelColor(i, j));
    }

    return QPixmap::fromImage(image);
}



//This function gets the pixel colour using the precomputed vectors
QRgb Landscape::getPixelColor(int x, int y)
{
    double altitudeAboveWater = m_altitude[x][y] - m_currentWaterLevel;
    return getPixelColor(altitudeAboveWater, m_brightness[x][y]);
}

//This function gets the pixel colour using the perlin function.
//Since this is used for the preview, no brightness adjustment is used.
QRgb Landscape::getPixelColor(double x, double y, double z,
                              noise::module::Perlin perlinAltitude, double waterLevel)
{
    double altitudeAboveWater = getAltitudeFromPerlinFunction(x, y, z, perlinAltitude) - waterLevel;
    return getPixelColor(altitudeAboveWater, 0);
}

QRgb Landscape::getPixelColor(double altitudeAboveWater, int brightness)
{
    int i1, i2;
    double fraction;
    getColorIndices(altitudeAboveWater, &i1, &i2, &fraction);

    QColor pixelColor = blendColors(g_settings->landscapeColors[i1],
                                    g_settings->landscapeColors[i2],
                                    fraction);
    double colorNoise = blendDoubles(g_settings->landscapeColorNoise[i1],
                                     g_settings->landscapeColorNoise[i2],
                                     fraction);

    if (brightness != 0)
    {
        int lighterFactor = 100.0 + colorNoise * brightness;
        return pixelColor.lighter(lighterFactor).rgb();
    }
    else
        return pixelColor.rgb();
}

void Landscape::getColorIndices(double altitude, int * i1, int * i2, double * fraction)
{
    int colorCount = int(g_settings->landscapeColors.size());

    if (altitude < g_settings->landscapeColorAltitudes[0])
    {
        *i1 = 0;
        *i2 = 0;
        *fraction = 0.0;
        return;
    }

    for (size_t i = 1; i < g_settings->landscapeColors.size(); ++i)
    {
        if (altitude < g_settings->landscapeColorAltitudes[i])
        {
            //The altitude is between that of index i-1 and i
            double gapSize = g_settings->landscapeColorAltitudes[i] - g_settings->landscapeColorAltitudes[i-1];
            double progress = altitude - g_settings->landscapeColorAltitudes[i-1];
            *i1 = int(i) - 1;
            *i2 = int(i);
            *fraction = progress / gapSize;
            return;
        }
    }

    //If the code gets here, then the altitude is greater than the largerst in
    //landscapeColorAltitudes.
    *i1 = colorCount - 1;
    *i2 = colorCount - 1;
    *fraction = 0.0;
    return;
}


QColor Landscape::blendColors(QColor color1, QColor color2, double fraction)
{
    double otherFraction = 1.0 - fraction;
    return QColor(color1.red() * otherFraction + color2.red() * fraction,
                  color1.green() * otherFraction + color2.green() * fraction,
                  color1.blue() * otherFraction + color2.blue() * fraction);
}


double Landscape::blendDoubles(double d1, double d2, double fraction)
{
    double otherFraction = 1.0 - fraction;
    return d1 * otherFraction + d2 * fraction;
}


bool Landscape::pointIsNotHabitable(double x, double y)
{
    int xInt = int(x);
    int yInt = int(y);

    if (xInt < 0 || yInt < 0 || xInt >= g_settings->environmentWidth || yInt >= g_settings->environmentHeight)
        return true;

    double altitude = getAltitudeFromVector(xInt, yInt);
    return altitude < m_currentWaterLevel || altitude > g_settings->maxHabitableAltitudeAboveWaterLevel + m_currentWaterLevel;
}

bool Landscape::pointIsOutOfBounds(double x, double y)
{
    int xInt = int(x);
    int yInt = int(y);

    return (xInt < 0 || yInt < 0 || xInt >= g_settings->environmentWidth || yInt >= g_settings->environmentHeight);
}

GroundType Landscape::getGroundType(double x, double y)
{
    int xInt = int(x);
    int yInt = int(y);

    if (xInt < 0 || yInt < 0 || xInt >= g_settings->environmentWidth || yInt >= g_settings->environmentHeight)
        return OUT_OF_BOUNDS;

    double altitude = getAltitudeFromVector(xInt, yInt);
    if (altitude < m_currentWaterLevel)
        return WATER;
    else if (altitude > g_settings->maxHabitableAltitudeAboveWaterLevel + m_currentWaterLevel)
        return HIGH_ALTITUDE;
    else
        return HABITABLE;
}

//Organisms will call this function to add themselves to the collision vector.
//If there is already an organism at that location in the vector, then this
//function returns a pointer to it - and a collision has occurred!
Organism * Landscape::addToCollisionVector(int x, int y, Organism * organismToAdd)
{
    Organism * returnOrganism = 0;
    if (m_collisions[x][y] != 0)
        returnOrganism = m_collisions[x][y];

    m_collisions[x][y] = organismToAdd;
    return returnOrganism;
}

//Organisms call this after collision stuff is done to reset the collision
//vector to all null pointers - ready for the next step.
void Landscape::clearCollisionVector(int x, int y)
{
    m_collisions[x][y] = 0;
}

bool Landscape::populationIsExtinct()
{
    return m_organisms.size() == 0;
}




void Landscape::checkSpecies()
{
    //Group the organisms into one list per species
    std::map< int, std::list<Organism *> > speciesGroups = groupOrganismsIntoSpeciesList();

    //Look for any species with a CURRENT status that have no organisms.
    //If found, change these species to EXTINCT.
    checkForExtinctSpecies(m_startingSpecies, &speciesGroups);

    //For each species, check to see if it has split into more than one species.
    for(std::map< int, std::list<Organism *> >::iterator i = speciesGroups.begin(); i != speciesGroups.end(); ++i)
        checkOneSpeciesForSplit(i->second, false);
}


std::map< int, std::list<Organism *> > Landscape::groupOrganismsIntoSpeciesList()
{
    std::map< int, std::list<Organism *> > speciesGroups;

    for (std::list<Organism *>::iterator i = m_organisms.begin(); i != m_organisms.end(); ++i)
        speciesGroups[(*i)->m_species->m_speciesNumber].push_back(*i);

    return speciesGroups;
}
std::map< int, std::vector<Organism *> > Landscape::groupOrganismsIntoSpeciesVector()
{
    std::map< int, std::vector<Organism *> > speciesGroups;

    for (std::list<Organism *>::iterator i = m_organisms.begin(); i != m_organisms.end(); ++i)
        speciesGroups[(*i)->m_species->m_speciesNumber].push_back(*i);

    return speciesGroups;
}

void Landscape::checkOneSpeciesForSplit(std::list<Organism *> list1, bool calledBySelf)
{
    //Move one organism (doesn't matter which) into a new list.
    std::list<Organism *> list2;
    list2.push_back(list1.front());
    list1.pop_front();


    //For each organism in the first list, see if it is close enough to
    //each species in the second list.  If so, move it to the second list.
    //This is repeated until either:
    //  -All organisms have moved into the second list.  This means that
    //   the organisms are all in the same species.
    //  -A round is completed with no additional organisms being moved
    //   into the second list.  This means that the species has split.

    int organismsMoved;
    do
    {
        organismsMoved = 0;
        for (std::list<Organism *>::iterator i = list1.begin(); i != list1.end();)
        {
            bool closeEnough = false;
            for (std::list<Organism *>::iterator j = list2.begin(); j != list2.end(); ++j)
            {
                if ((*i)->calculateDistance(*j) <= g_settings->maximumDistanceForSameSpecies)
                {
                    closeEnough = true;
                    break;
                }
            }
            if (closeEnough)
            {
                ++organismsMoved;
                list2.push_back(*i);
                list1.erase(i++);
            }
            else
                ++i;
        }
    } while (organismsMoved > 0 && list1.size() > 0);


    //If the species has split, create a new species for the organisms in
    //the second list, as they are all close enough to each other to be a
    //single species.
    //Then call this function again for the organisms left in the first
    //list.  They are probably a single, coherent species, but it may be
    //that they too are comprised of multiple species, and so this same
    //procedure must be done on them.
    if (list1.size() > 0)
    {
        boost::shared_ptr<Species> parentSpecies = list2.front()->m_species;
        boost::shared_ptr<Species> newSpecies(new Species(parentSpecies, m_elapsedTime));
        parentSpecies->m_childSpecies.push_back(newSpecies);

        for (std::list<Organism *>::iterator j = list2.begin(); j != list2.end(); ++j)
            (*j)->m_species = newSpecies;

        checkOneSpeciesForSplit(list1, true);
    }



    //If the species has not split, and this function did not call itself,
    //then no further action is required.  This means that the species has
    //not split since the last check.
    if (list1.empty() && !calledBySelf)
        return;


    //If the species has not split, but this function was called by itself,
    //create a new species for each organism (all of which will be in the
    //second list).
    //This is the case when a species has split and the last of the
    //remaining organisms groups well.  Since most splits will be into just
    //two species, this will most often represent the second group of a split.
    if (list1.empty() && calledBySelf)
    {
        boost::shared_ptr<Species> parentSpecies = list2.front()->m_species;
        boost::shared_ptr<Species> newSpecies(new Species(parentSpecies, m_elapsedTime));
        parentSpecies->m_childSpecies.push_back(newSpecies);

        for (std::list<Organism *>::iterator j = list2.begin(); j != list2.end(); ++j)
            (*j)->m_species = newSpecies;

        //The parent species should now have two or more child species.  Since
        //the larger group is more likely to be the first group, because the
        //organism chosen at the start of this function is more likely to be
        //from the larger group.  This will make the tree unbalanced, as left
        //branches will typically contain more organisms than right branches.
        //To combat this, we now shuffle the order of the child species.
        std::mt19937 rng(std::time(nullptr));
        std::shuffle(parentSpecies->m_childSpecies.begin(), parentSpecies->m_childSpecies.end(), rng);
    }
}


void Landscape::checkForExtinctSpecies(boost::shared_ptr<Species> species, std::map< int, std::list<Organism *> > * speciesGroups)
{
    if (species->m_state == CURRENT &&
            speciesGroups->count(species->m_speciesNumber) == 0)
    {
        species->m_state = EXTINCT;
        species->m_endDate = m_elapsedTime;
    }

    //Now call this function on all child species.
    for (size_t i = 0; i < species->m_childSpecies.size(); ++i)
        checkForExtinctSpecies(species->m_childSpecies[i], speciesGroups);
}



QString Landscape::getNewickTree()
{
    QString startingSpeciesTree = m_startingSpecies->getNewickTree(m_elapsedTime);
    return startingSpeciesTree + ";";
}




//If no parameters are given for the thumbnail, then the current ones are used.
QPixmap Landscape::getLandscapeThumbnail(int width, int height)
{
    return getLandscapeThumbnail(width, height, getCurrentParameters());
}

QPixmap Landscape::getLandscapeThumbnail(int width, int height, LandscapeParameters parameters)
{
    //The thumbnail is made with a height that matches the environment.  If the
    //requested thumbnail has the same aspect ratio as the environment, then the
    //width will match too.  But if the aspect ratio is different, there will be
    //more or less width shown.
    double ratio = double(g_settings->environmentHeight) / height;


    noise::module::Perlin perlinAltitude;
    perlinAltitude.SetOctaveCount(g_settings->perlinOctaves);
    perlinAltitude.SetFrequency(parameters.m_perlinFrequency);
    perlinAltitude.SetPersistence(parameters.m_perlinPersistance);

    QImage image(width, height, QImage::Format_RGB32);

    for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < height; ++j)
            image.setPixel(i, j, getPixelColor(i * ratio, j * ratio, parameters.m_perlinZ,
                                               perlinAltitude, parameters.m_waterLevel));
    }

    return QPixmap::fromImage(image);
}




std::map<int, boost::shared_ptr<Genome>> Landscape::getRandomGenomeForEachSpecies()
{
    std::map<int, boost::shared_ptr<Genome>> returnMap;

    std::map< int, std::vector<Organism *> > organismsGroupedBySpecies = groupOrganismsIntoSpeciesVector();

    for(std::map< int, std::vector<Organism *> >::iterator i = organismsGroupedBySpecies.begin();
        i != organismsGroupedBySpecies.end(); ++i)
    {
        int speciesNumber = i->first;
        int organismCount = int(i->second.size());
        int randomIndex = g_randomNumbers->getRandomInt(0, organismCount - 1);
        Organism * randomOrganism = i->second[randomIndex];

        returnMap[speciesNumber] = randomOrganism->m_genome;
    }

    return returnMap;
}

std::map<int, std::vector< std::pair<float, float> > > Landscape::getOrganismLocationsForEachSpecies()
{
    std::map<int, std::vector< std::pair<float, float> > > returnMap;

    std::map< int, std::vector<Organism *> > organismsGroupedBySpecies = groupOrganismsIntoSpeciesVector();

    for(std::map< int, std::vector<Organism *> >::iterator i = organismsGroupedBySpecies.begin();
        i != organismsGroupedBySpecies.end(); ++i)
    {
        int speciesNumber = i->first;
        std::vector< std::pair<float, float> > speciesLocations;

        for (size_t j = 0; j < i->second.size(); ++j)
        {
            float x = i->second[j]->m_x;
            float y = i->second[j]->m_y;
            speciesLocations.push_back(std::pair<float, float>(x, y));
        }

        returnMap[speciesNumber] = speciesLocations;
    }

    return returnMap;
}


LandscapeParameters Landscape::getCurrentParameters()
{
    return LandscapeParameters(g_settings->perlinFrequency, g_settings->perlinPersistence,
                               g_settings->perlinZValue, m_currentWaterLevel);
}
