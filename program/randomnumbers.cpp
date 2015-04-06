#include "randomnumbers.h"

#include <QTime>

#include "globals.h"
#include "settings.h"

RandomNumbers::RandomNumbers()
{
    QTime midnight(0, 0, 0);
    int seed = midnight.msecsTo(QTime::currentTime());
    random.seed(seed);
//    random.seed(0);  //Use this for deterministic testing

    randomZeroToOne = new boost::random::uniform_01<>();
    randomZeroOrOne = new boost::random::uniform_smallint<>(0, 1);
    randomZeroToThree = new boost::random::uniform_smallint<>(0, 3);
    randomAngleRadians = new boost::random::uniform_real_distribution<>(0, 6.283185307);
    randomOrganismLifespan = new boost::random::exponential_distribution<>(1.0 / g_settings->meanOrganismLifespan);
}

RandomNumbers::~RandomNumbers()
{
    delete randomZeroToOne;
    delete randomZeroOrOne;
    delete randomZeroToThree;
    delete randomAngleRadians;
    delete randomOrganismLifespan;
}

void RandomNumbers::remakeOrganismLifespanGenerator()
{
    delete randomOrganismLifespan;
    randomOrganismLifespan = new boost::random::exponential_distribution<>(1.0 / g_settings->meanOrganismLifespan);
}

double RandomNumbers::getRandomDouble(double min, double max)
{
    boost::random::uniform_real_distribution<> randomDoubleDistribution(min, max);
    return randomDoubleDistribution(random);
}

int RandomNumbers::getRandomInt(int min, int max)
{
    boost::random::uniform_int_distribution<> randomIntDistribution(min, max);
    return randomIntDistribution(random);
}

//This function returns an int from a double.
//The integer componenet of is always included.  Any fractional component represents the
//probability for another.
//E.g. 2.2 will have a 20% chance of returning 3 and an 80% chance of returning 2.
int RandomNumbers::changeDoubleToProbabilisticInt(double input)
{
    int count = int(input);

    double chanceOfAnother = input - count;
    if (chanceOfTrue(chanceOfAnother))
        ++count;

    return count;
}


QChar RandomNumbers::getRandomNucleotide()
{
    switch ((*randomZeroToThree)(random))
    {
    case 0:
        return 'A';
        break;
    case 1:
        return 'B';
        break;
    case 2:
        return 'C';
        break;
    default:
        return 'D';
        break;
    }
}
