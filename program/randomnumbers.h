#ifndef RANDOMNUMBERS_H
#define RANDOMNUMBERS_H

#ifndef Q_MOC_RUN
#include "boost/random/mersenne_twister.hpp"
#include "boost/random/uniform_int_distribution.hpp"
#include "boost/random/uniform_real_distribution.hpp"
#include "boost/random/uniform_smallint.hpp"
#include "boost/random/poisson_distribution.hpp"
#include "boost/random/exponential_distribution.hpp"
#endif // Q_MOC_RUN

#include <QChar>

class RandomNumbers
{
public:
    RandomNumbers();
    ~RandomNumbers();

    double getRandomDouble(double min, double max);
    int getRandomInt(int min, int max);
    double getRandomZeroToOne() {return (*randomZeroToOne)(random);}
    int getRandomZeroOrOne() {return (*randomZeroOrOne)(random);}
    bool chanceOfTrue(double chance) {return getRandomZeroToOne() < chance;}
    int changeDoubleToProbabilisticInt(double input);
    bool fiftyPercentChance() {return getRandomZeroOrOne() == 0;}
    double getRandomAngleRadians() {return (*randomAngleRadians)(random);}
    double getRandomOrganismLifespan() {return (*randomOrganismLifespan)(random);}
    QChar getRandomNucleotide();

    void remakeOrganismLifespanGenerator();

private:
    boost::random::mt19937 random;
    boost::random::uniform_01<> * randomZeroToOne;
    boost::random::uniform_smallint<> * randomZeroOrOne;
    boost::random::uniform_smallint<> * randomZeroToThree;
    boost::random::uniform_real_distribution<> * randomAngleRadians;
    boost::random::exponential_distribution<> * randomOrganismLifespan;
};

#endif // RANDOMNUMBERS_H
