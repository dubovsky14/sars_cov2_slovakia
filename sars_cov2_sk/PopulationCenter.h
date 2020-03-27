#ifndef POPULATION_CENTER_H
#define POPULATION_CENTER_H

#include "../sars_cov2_sk/Person.h"
#include "../sars_cov2_sk/Household.h"

#include <vector>
#include <string>
namespace sars_cov2_sk	{
    class PopolationCenter  {
        private:
            std::string name;
            std::vector<Person *> m_inhabitants;            // vector of people living (or already buried ...) in the city
            std::vector<Person *> m_temporary_ocupants;     // vector of people being in the city temporarily (comuters to work, visitors ...)
            std::vector<Household> m_households;

            unsigned int number_of_inhabitants;

            // Called in the contructor. Takes people and move them randomly to households
            void BuildAndFillHouseholds(float average_people_in_household);

        public:
            // Creates population center (town) with "number_of_inhabitants" of people living here.
            // Takes people from country_population between indexes (first_citizen_index) and (first_citizen_index+number_of_inhabitants) and move them to the city
            // The average_people_in_household is average ocuppance of a household in the city
            PopolationCenter(std::vector<Person> *country_population, unsigned int first_citizen_index, unsigned  int number_of_inhabitants, float average_people_in_household = 3.);

            void SpreadInfectionInHouseholds(float probablity);
    };
}
#endif