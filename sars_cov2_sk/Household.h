#ifndef HOUSEHOLD_H
#define HOUSEHOLD_H

#include"../sars_cov2_sk/Person.h"

#include<vector>

namespace sars_cov2_sk	{
    class Household  {
        private:
            std::vector<Person *> m_inhabitants;

        public:
            const std::vector<Person *> *GetInhabitants()   {return &m_inhabitants;};

            void AddInhabitant(Person *person)  {m_inhabitants.push_back(person);};

            // Is someone from the household infected?
            bool Infected() const;

            // Simulate daily interations in household.
            // Each pair of its inhabitants interacts once and with "probability" the infected person spreads the infection to a healthy person during this interaction
            void SpreadInfection(float probability = 0.4);

            unsigned int GetNumberOfInhabitants() const {return m_inhabitants.size();};

            static unsigned int GetRandomHouseholdNumber(float average_people_in_household);
    };
};
#endif