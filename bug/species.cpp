#include "species.h"

int Species::m_highestSpeciesNumber = 0;

Species::Species(boost::shared_ptr<Species> parentSpecies,
                 long long date) :
    m_state(CURRENT),
    m_startDate(date),
    m_endDate(-1),
    m_parentSpecies(parentSpecies),
    m_squareTreeSpeciesLine(0),
    m_curvedTreeLine(0)
{
    //If this species doesn't have a parent, then it's the first one.
    if (parentSpecies == 0)
    {
        m_speciesNumber = 1;
        m_highestSpeciesNumber = 1;
    }

    //If the species does have a parent, then give it the next available
    //species number.
    else
    {
        m_speciesNumber = ++m_highestSpeciesNumber;
        m_parentSpecies->m_state = SPLIT;
        m_parentSpecies->m_endDate = date;
    }
}


QString Species::getNewickTree(long long date)
{
    QString returnString;

    //If there are child species, they go in parentheses, separated by commas
    if (m_childSpecies.size() > 0)
    {
        returnString += "(";
        for (size_t i = 0; i < m_childSpecies.size(); ++i)
        {
            returnString += m_childSpecies[i]->getNewickTree(date);
            if (i != m_childSpecies.size() - 1)
                returnString += ",";
        }
        returnString += ")";
    }

    //Now add the node name
    returnString += QString::number(m_speciesNumber);
    returnString += ":";

    //Now add the node length
    int age;
    if (m_state == CURRENT)
        age = date - m_startDate;
    else
        age = m_endDate - m_startDate;
    returnString += QString::number(age);

    return returnString;
}


SpeciesState Species::getStateAtDate(long long date)
{
    if (m_endDate == -1)
        return CURRENT;
    if (date < m_endDate)
        return CURRENT;
    return m_state;
}
