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

PopulationCenter::~PopulationCenter()   {
    delete m_migrations;
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

    // Meetings initiated by inhabitants
    for (Person *person : m_inhabitants)    {
        // sampling from exponential distribution
        // /2 is there in order to not double count meetings (in average 50% of the meetings is intiated by this person, 50% by the other person)
        const unsigned int interactions = RandomPoisson(average_interactions_per_person/2.);

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

    // Meetings initiated by temporary occupants
    for (Person *person : m_temporary_occupants)    {
        // sampling from exponential distribution
        // /2 is there in order to not double count meetings (in average 50% of the meetings is intiated by this person, 50% by the other person)
        const unsigned int interactions = RandomPoisson(average_interactions_per_person/2.);

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

void PopulationCenter::AddTemporaryOccupant(sars_cov2_sk::Person *person)   {
    m_temporary_occupants.push_back(person);
}

void PopulationCenter::RemoveAllTemporaryOccupants()   {
    m_temporary_occupants.clear();
}

void PopulationCenter::SendTravelersToCity(PopulationCenter *destination_city, unsigned int number_of_travelers) const    {
    for (unsigned int i_traveler = 0; i_traveler < number_of_travelers; i_traveler++)   {
        unsigned int index_in_origin_city = m_number_of_inhabitants*RandomUniform();
        destination_city->AddTemporaryOccupant(m_inhabitants.at(index_in_origin_city));
    }
};

void PopulationCenter::SendTravelersToAllCities(vector<PopulationCenter> *cities) const    {
    for (unsigned int i_city = 0; i_city < cities->size(); i_city++)   {
        // Can't send traveler to the city where he lives. 
        if (&cities->at(i_city) == this)    {
            continue;
        }
        // Sampling with poisson would be incredibly slow here ... in average this should be also fine enough
        this->SendTravelersToCity(&cities->at(i_city), m_migrations->at(i_city));
    }
};

void PopulationCenter::SaveTheDayToHistory()    {
    unsigned int unaffected         = 0;
    unsigned int infected           = 0;
    unsigned int immune             = 0;
    unsigned int dead               = 0;
    unsigned int new_cases          = 0;
    unsigned int hospitalized       = 0;

    for (const Person *person : m_inhabitants)  {
        if (person->IsUnaffected())     unaffected++;
        if (person->IsIll())            infected++;
        if (person->IsImmune())         immune++;
        if (person->IsDead())           dead++;
        if (person->IsNewCase())        new_cases++;
        if (person->IsHospitalized())   hospitalized++;
    }

    m_logging_days          .push_back(Person::GetDay());
    m_logging_unaffected    .push_back(unaffected  );
    m_logging_infected      .push_back(infected    );
    m_logging_immune        .push_back(immune      );
    m_logging_dead          .push_back(dead        );
    m_logging_new_cases     .push_back(new_cases   );
    m_logging_hospitalized  .push_back(hospitalized);
};

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
        population->push_back(Person());
    }

    // Move people to cities
    int current_person_index = 0;
    for (unsigned int i = 0; i < number_of_inhabitants.size(); i++) {
        cities->push_back(PopulationCenter(population, current_person_index, number_of_inhabitants.at(i), 3.));
        cities->back().SetName(names.at(i));
        current_person_index += number_of_inhabitants.at(i);
    }
};

void PopulationCenter::SetMigrations(const vector <vector <unsigned int >> *migrations, vector<PopulationCenter> *cities)   {
    for (unsigned int i_city = 0; i_city < migrations->size(); i_city++)   {
        cities->at(i_city).SetTravelMigrations(&migrations->at(i_city));
    }
}