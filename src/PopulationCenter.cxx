#include "../sars_cov2_sk/PopulationCenter.h"
#include "../sars_cov2_sk/Household.h"
#include "../sars_cov2_sk/Person.h"
#include "../sars_cov2_sk/HelperFunctions.h"

#include <vector>
#include <iostream>
#include <cmath>

using namespace std;
using namespace sars_cov2_sk;

PopulationCenter::PopulationCenter( vector<Person> *country_population, unsigned int first_citizen_index, 
                                    unsigned int number_of_inhabitants, float average_people_in_household) {
    const unsigned int last_citizen_index = first_citizen_index + number_of_inhabitants;
    if (country_population->size() < last_citizen_index)    {
        throw "We do not have enough people.";
    } 

    // Save pointers to ihabitants to member variable
    for (unsigned int i = first_citizen_index; i < last_citizen_index; i++) {
        m_inhabitants.push_back(&country_population->at(i));
    }
    m_number_of_inhabitants = m_inhabitants.size();
    BuildAndFillHouseholds(average_people_in_household);
}

void PopulationCenter::BuildAndFillHouseholds(float average_people_in_household)  {
    unsigned int current_person = 0;
    while (current_person < m_number_of_inhabitants)   {
        m_households.push_back(Household());
        // Randomly generate number of inhabitants and fill the household
        const unsigned int people_in_household = Household::GetRandomHouseholdNumber(average_people_in_household);
        for (unsigned int i = 0; (i < people_in_household) && (current_person < m_number_of_inhabitants); i++)  {
            m_households.back().AddInhabitant(m_inhabitants.at(current_person));
            current_person++;
        }
    }
};

void PopulationCenter::SimulateDailySpread(float transmission_probability_household, float transmission_probability_other)  {
    EvolveInhabitants();
    SpreadInfectionInHouseholds(transmission_probability_household);
    SimulateDailySpreadAmongInhabitantsAndTempOccupants(5, transmission_probability_other);
};

void PopulationCenter::SpreadInfectionInHouseholds(float probablity)    {
    for (Household &household : m_households)   {
        household.SpreadInfection(probablity);
    }
}

void PopulationCenter::SimulateDailySpreadAmongInhabitantsAndTempOccupants(float average_interactions_per_person, float probability)    {
    const unsigned int number_of_temporary_occupants = m_temporary_occupants.size();

    for (Person *person : m_inhabitants)    {
        // sampling from exponential distribution
        // /2 is there in order to not double count meetings, +1 are there to avoid 0 and 1 in logarithm, 0.5 is there instead of rounding,
        const unsigned int interactions = -(average_interactions_per_person/2)*log(RandomUniform()) + 0.5;

        for (unsigned int i = 0; i < interactions; i++) {
            unsigned int person2_index = RandomUniform()*(number_of_temporary_occupants + m_number_of_inhabitants);

            // Interaction with inhabitants of the city
            if (person2_index < m_number_of_inhabitants)  {
                Person::Meet(person, m_inhabitants.at(person2_index), probability, 0.8);
            }
            else    // Interaction with temporary occupants
            {
                person2_index -= m_number_of_inhabitants;
                if (person2_index >= number_of_temporary_occupants)
                    continue; // if this happens, somethins is really wrong ...

                Person::Meet(person, m_temporary_occupants.at(person2_index), probability, 0.8);
            }
        }
    }
    
};


void PopulationCenter::EvolveInhabitants()    {
    for (Person *person : m_inhabitants)   {
        person->Evolve();
    }
}

void PopulationCenter::CityAndPersonsFactory(   const vector<unsigned int> &number_of_inhabitants, const vector<string> &names,
                                                vector<Person> *population,
                                                vector<PopulationCenter> *cities)    {

    population->clear();
    cities->clear();

    // Firstly calculate population size
    int population_size = 0;
    for (unsigned int x : number_of_inhabitants)    {
        population_size += x;
    }

    // Create population
    for (int i = 0; i < population_size; i++)   {
        if (i % 1000000 == 0)
            cout << i << "/" << population_size << endl;
            population->push_back(Person());
    }

    // Move people to cities
    int current_person_index = 0;
    for (unsigned int i = 0; i < number_of_inhabitants.size(); i++) {
        cities->push_back(PopulationCenter(population, current_person_index, number_of_inhabitants.at(i), 3.));
        current_person_index += number_of_inhabitants.at(i);
    }
};