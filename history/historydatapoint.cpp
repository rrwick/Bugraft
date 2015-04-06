#include "historydatapoint.h"

#include "../bug/genome.h"
#include "../bug/organism.h"
#include "../program/globals.h"
#include "../program/landscape.h"

HistoryDataPoint::HistoryDataPoint(long long date,
                                   std::map<int, boost::shared_ptr<Genome>> randomGenomeForEachSpecies,
                                   LandscapeParameters landscapeParameters,
                                   std::map<int, std::vector<std::pair<float, float> > > organismLocationsForEachSpecies) :
    m_date(date), m_landscapeParameters(landscapeParameters),
    m_organismLocationsForEachSpecies(organismLocationsForEachSpecies)
{
    //Make an organism for each genome.
    for (std::map<int, boost::shared_ptr<Genome>>::iterator i = randomGenomeForEachSpecies.begin();
         i != randomGenomeForEachSpecies.end(); ++i)
    {
        int speciesNumber = i->first;
        m_randomOrganismForEachSpecies[speciesNumber] = new Organism(i->second);
    }

    makeThumbnail();
}

HistoryDataPoint::~HistoryDataPoint()
{
    for (std::map<int, Organism *>::iterator i = m_randomOrganismForEachSpecies.begin();
         i != m_randomOrganismForEachSpecies.end(); ++i)
    {
        delete i->second;
    }
}

void HistoryDataPoint::makeThumbnail()
{
    m_landscapeThumbnail = g_landscape->getLandscapeThumbnail(160, 90, m_landscapeParameters);
}
