#ifndef POPULATION_CENTER_H
#define POPULATION_CENTER_H

#include "../sars_cov2_sk/Person.h"
#include "../sars_cov2_sk/Household.h"

#include "../sars_cov2_sk/Constants.h"

#include <vector>
#include <string>
namespace sars_cov2_sk	{
    class PopulationCenter  {
        private:
            std::string m_name;
            std::vector<Person *> m_inhabitants;            // vector of people living (or already buried ...) in the city
            std::vector<Person *> m_temporary_occupants;    // nope, not Russians ... just people being in the city temporarily (comuters to work, visitors ...)
            std::vector<Household> m_households;

            unsigned int m_number_of_inhabitants;

            // Called in the contructor. Takes people and move them randomly to households
            void BuildAndFillHouseholds(float average_people_in_household);

        public:
            // Creates population center (town) with "number_of_inhabitants" of people living here.
            // Takes people from country_population between indexes (first_citizen_index) and (first_citizen_index+number_of_inhabitants) and move them to the city
            // The average_people_in_household is average ocuppance of a household in the city
            PopulationCenter(std::vector<Person> *country_population, unsigned int first_citizen_index, unsigned  int number_of_inhabitants, float average_people_in_household = 3.);

            const std::vector<Household> *GetHouseholdVector()  const {return &m_households;};

            void SimulateDailySpread(float transmission_probability_household = TRANSMISSION_PROB_HOUSE, float transmission_probability_other = TRANSMISSION_PROB_OUT);

            void SpreadInfectionInHouseholds(float probablity = TRANSMISSION_PROB_HOUSE);

            void SimulateDailySpreadAmongInhabitantsAndTempOccupants(float average_interactions_per_person, float probability = TRANSMISSION_PROB_OUT);

            void EvolveInhabitants();

            void SetName(std::string &name) {m_name = name;};

            std::string GetName()   const {return m_name;};

            const std::vector<sars_cov2_sk::Person *> *GetInhabitants()   const {return &m_inhabitants; };

            const unsigned int GetNumberOfInhabitants() const {return m_number_of_inhabitants;};

            // Factory method for both population (vector<Person>) and cities (vector<PopulationCenter>)
            // Requires number of inhabitants and names of cities as input
            // City and population vector are firstly cleared out, then the are filled again
            static void CityAndPersonsFactory(  const std::vector<unsigned int> &number_of_inhabitants, const std::vector<std::string> &names,
                                                std::vector<sars_cov2_sk::Person> *population,
                                                std::vector<sars_cov2_sk::PopulationCenter> *cities);
    };
}
#endif