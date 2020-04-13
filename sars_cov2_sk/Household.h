#ifndef HOUSEHOLD_H
#define HOUSEHOLD_H

#include"../sars_cov2_sk/Person.h"

#include<vector>

namespace sars_cov2_sk	{
    enum HouseholdCategory {enum_young, enum_elderly};
    class Household  {
        private:
            std::vector<Person *> m_inhabitants;

            static bool     s_info_initialized;
            static int      s_n_young;
            static int      s_n_elderly;
            static float    s_average_occupancy_young;   // Average number of persons living in "young" households
            static float    s_average_occupancy_elderly; // Average number of persons living in "elderly" households
            static int      s_n_households_young;   // Number of households with young people
            static int      s_n_households_elderly; // Number of households with elderly people
            static float    s_fraction_of_elderly_living_with_young;

        public:
            const std::vector<Person *> *GetInhabitants()   {return &m_inhabitants;};

            void AddInhabitant(Person *person)  {m_inhabitants.push_back(person);};

            // Is someone from the household infected?
            bool Infected() const;

            // Simulate daily interations in household.
            // Each pair of its inhabitants interacts once and with "probability" the infected person spreads the infection to a healthy person during this interaction
            void SpreadInfection(float probability);

            unsigned int GetNumberOfInhabitants() const {return m_inhabitants.size();};

            // Generate a random number with the distribution of household occupancy
            static void GetRandomHouseholdNumbers(unsigned int *number_of_inhabitants, HouseholdCategory *category);

            static void IntializeHouseholdNumbers();
    };
};
#endif