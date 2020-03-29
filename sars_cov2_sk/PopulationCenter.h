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

            // Number of people traveling from this city to another cities
            const std::vector<unsigned int> *m_migrations = nullptr;

            // Called in the contructor. Takes people and move them randomly to households
            void BuildAndFillHouseholds(float average_people_in_household);

        public:
            // Creates population center (town) with "number_of_inhabitants" of people living here.
            // Takes people from country_population between indexes (first_citizen_index) and (first_citizen_index+number_of_inhabitants) and move them to the city
            // The average_people_in_household is average ocuppance of a household in the city
            PopulationCenter(std::vector<Person> *country_population, unsigned int first_citizen_index, unsigned  int number_of_inhabitants, float average_people_in_household = 3.);

            ~PopulationCenter();

            const std::vector<Household> *GetHouseholdVector()  const {return &m_households;};

            void SimulateDailySpread(float transmission_probability_household = TRANSMISSION_PROB_HOUSE, float transmission_probability_other = TRANSMISSION_PROB_OUT);

            // Loop over all households, arrange meetings of all pairs of persons inside these.
            void SpreadInfectionInHouseholds(float probablity = TRANSMISSION_PROB_HOUSE);

            // Simulate random meetings of people in a city (both those who live there and also of the travellers)
            void SimulateDailySpreadAmongInhabitantsAndTempOccupants(float average_interactions_per_person, float probability = TRANSMISSION_PROB_OUT);

            // Simulate evolution of the disease of inhabitants (healing, getting hospitalized, dying ...). Does not include interactions with other people
            void EvolveInhabitants();

            void SetName(std::string &name) {m_name = name;};

            std::string GetName()   const {return m_name;};

            inline Person *GetInhabitant(unsigned int index)    {return m_inhabitants.at(index);};

            const std::vector<sars_cov2_sk::Person *> *GetInhabitants()   const {return &m_inhabitants; };

            // Add a traveller
            void AddTemporaryOccupant(sars_cov2_sk::Person *person);

            // Remove all people who do not live in the city
            void RemoveAllTemporaryOccupants();

            const unsigned int GetNumberOfInhabitants() const {return m_number_of_inhabitants;};
            
            void SetTravelMigrations(const std::vector<unsigned int> *migrations)   { m_migrations = migrations;};

            void SendTravelersToCity(sars_cov2_sk::PopulationCenter *destination_city, unsigned int number_of_travelers) const;

            void SendTravelersToAllCities(std::vector<sars_cov2_sk::PopulationCenter> *destination_city) const;

            // Factory method for both population (vector<Person>) and cities (vector<PopulationCenter>)
            // Requires number of inhabitants and names of cities as input
            // City and population vector are firstly cleared out, then the are filled again
            static void CityAndPersonsFactory(  const std::vector<unsigned int> &number_of_inhabitants, const std::vector<std::string> &names,
                                                std::vector<sars_cov2_sk::Person> *population,
                                                std::vector<sars_cov2_sk::PopulationCenter> *cities);

            static void SetMigrations(const std::vector <std::vector <unsigned int >> *migrations, std::vector<sars_cov2_sk::PopulationCenter> *cities);
    };
}
#endif