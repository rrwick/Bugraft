#include "genome.h"
#include "../program/globals.h"
#include "../program/randomnumbers.h"
#include "../program/settings.h"
#include "math.h"

Genome::Genome()
{
    //EFFICIENCY PROBLEM - I THINK THIS CONSTRUCTOR WILL BE CALLED WHEN LOADING
    //A SIMULATION.  THIS IS WASTEFUL, AS THERE IS NO NEED TO COME UP WITH A
    //RANDOM GENOME IF IT WILL JUST BE WRITTEN OVER ANYWAY.
    //I SHOULD HAVE A DIFFERENT CONSTRUCTOR DO THE RANDOM GENOME.
    randomise();
}

//This constructor blends the two parent genomes and then mutates it
Genome::Genome(boost::shared_ptr<Genome> parent1, boost::shared_ptr<Genome> parent2)
{
    for (int i = 0; i < 16; ++i)
    {
        if (g_randomNumbers->fiftyPercentChance())
            m_genes.push_back(parent1->m_genes[i]);
        else
            m_genes.push_back(parent2->m_genes[i]);
    }

    perhapsMutate();
}



//This function creates a random genome, but not a totally random
//one.  Rather, each gene is constrained to have a moderate value.
void Genome::randomise()
{
    m_genes.clear();

    for (int i = 0; i < 16; ++i)
        m_genes.push_back(g_randomNumbers->getRandomDouble(45.0, 55.0));
}


//This function may insert up to one point mutation into the genome.
void Genome::perhapsMutate()
{
    if (g_randomNumbers->chanceOfTrue(g_settings->chanceOfMutation))
    {
        int i = g_randomNumbers->getRandomInt(0, 15);
        m_genes[i] += g_randomNumbers->getRandomDouble(-g_settings->maxMutationSize,
                                                      g_settings->maxMutationSize);
        if (m_genes[i] < 0.0)
            m_genes[i] = 0.0;
        else if (m_genes[i] > 99.99)
            m_genes[i] = 99.99;
    }
}

float Genome::calculateDistance(boost::shared_ptr<Genome> other)
{
    double distanceSquared = 0.0;
    for (int i = 0; i < 16; ++i)
    {
        double geneDistance = m_genes[i] - other->m_genes[i];
        distanceSquared += geneDistance * geneDistance;
    }
    return sqrt(distanceSquared);
}


QString Genome::getString()
{
    QString returnString = "";
    for (size_t i = 0; i < m_genes.size(); ++i)
    {
        returnString += QString::number(m_genes[i], 'f', 2);
        if (i == 3 || i == 7 || i == 11)
            returnString += ",\n";
        else if (i != m_genes.size() - 1)
            returnString += ", ";
    }
    return returnString;
}
