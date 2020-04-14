#include "../sars_cov2_sk/PopulationCenter.h"
#include "../sars_cov2_sk/Household.h"
#include "../sars_cov2_sk/Person.h"
#include "../sars_cov2_sk/HelperFunctions.h"
#include "../sars_cov2_sk/ConfigParser.h"

#include <vector>
#include <iostream>
#include <cmath>

using namespace std;
using namespace sars_cov2_sk;

PopulationCenter::PopulationCenter( vector<Person> *country_population, unsigned int first_citizen_index,
                                    unsigned int number_of_inhabitants) {
    const unsigned int last_citizen_index = first_citizen_index + number_of_inhabitants;
    if (country_population->size() < last_citizen_index)    {
        throw "We do not have enough people.";
    }

    // Save pointers to ihabitants to member variable
    for (unsigned int i = first_citizen_index; i < last_citizen_index; i++) {
        m_inhabitants.push_back(&country_population->at(i));
    }
    m_number_of_inhabitants = m_inhabitants.size();
    BuildAndFillHouseholds();
}

PopulationCenter::~PopulationCenter()   {

}

void PopulationCenter::RandomlyInfectNInhabitants(unsigned int number_of_infected)    {
    if (number_of_infected > m_number_of_inhabitants)   {
        throw "You asked me to infected more people than the number of inhabitants in the city " + m_name;
    }

    unsigned int infected_already = 0;
    while (infected_already < number_of_infected)   {
        const unsigned int index = (this->GetNumberOfInhabitants()-1)*RandomUniform();
        if (m_inhabitants.at(index)->IsIll())   {
            continue;
        }
        else {
            m_inhabitants.at(index)->Infect();
            infected_already++;
        }
    }
}

void PopulationCenter::BuildAndFillHouseholds()  {
    unsigned int current_person = 0;
    while (current_person < m_number_of_inhabitants)   {
        m_households.push_back(Household());
        // Randomly generate number of inhabitants and fill the household
        unsigned int people_in_household;
        HouseholdCategory household_category;
        Household::GetRandomHouseholdNumbers(&people_in_household, &household_category);
        for (unsigned int i = 0; (i < people_in_household) && (current_person < m_number_of_inhabitants); i++)  {
            // Adjust age of ihabitants according to the age category of the household
            if (household_category == enum_elderly) {
                m_inhabitants.at(current_person)->SetAgeCategory(Person::GenerateRandomAgeCategoryElderlyHousehold());
            }
            if (household_category == enum_young) {
                m_inhabitants.at(current_person)->SetAgeCategory(Person::GenerateRandomAgeCategoryYoungHousehold());
            }

            m_households.back().AddInhabitant(m_inhabitants.at(current_person));
            current_person++;
        }
    }
};

void PopulationCenter::SimulateDailySpread(float transmission_probability_household, float transmission_probability_other)  {
    EvolveInhabitants();
    SpreadInfectionInHouseholds(transmission_probability_household);
    SimulateDailySpreadAmongInhabitantsAndTempOccupants(ConfigParser::GetAverageNumberOfCityInteractions(), transmission_probability_other);
};

void PopulationCenter::SpreadInfectionInHouseholds(float probablity)    {
    for (Household &household : m_households)   {
        household.SpreadInfection(probablity);
    }
}

void PopulationCenter::SimulateDailySpreadAmongInhabitantsAndTempOccupants(float average_interactions_per_person, float probability)    {
    const unsigned int number_of_temporary_occupants = m_temporary_occupants.size();

    const bool travellers_increase_number_of_meetings = !ConfigParser::TravellerMeetingsConstant();

    // Meetings initiated by inhabitants
    for (Person *person : m_inhabitants)    {
        // /2 is there in order to not double count meetings (in average 50% of the meetings is intiated by this person, 50% by the other person)
        const int interactions = RandomPoisson(average_interactions_per_person/2.);

        for (int i = 0; i < interactions; i++) {
            // Travelers who visited N cities (including their home city) will initialize 1/N part of their meetings in each city
            if (person->GetNumberOfVisitedMunicipalities() > 1 && !travellers_increase_number_of_meetings)    {
                if (RandomUniform() < 1./person->GetNumberOfVisitedMunicipalities())  {
                    continue;
                }
            }
            unsigned int person2_index = RandomUniform()*(number_of_temporary_occupants + m_number_of_inhabitants);
            Person *person2 = nullptr;

            // Interaction with inhabitants of the city
            if (person2_index < m_number_of_inhabitants)  {
               person2 = m_inhabitants.at(person2_index);
            }
            else    // Interaction with temporary occupants
            {
                person2_index -= m_number_of_inhabitants;
                if (person2_index >= number_of_temporary_occupants) {
                    continue;  // might happen very rarely because of a finite float precision
                }
                person2 = m_temporary_occupants.at(person2_index);
            }

            // Travelers who visited N cities (including their home city) will have 1/N part of their meetings in each city
            if (person2->GetNumberOfVisitedMunicipalities() > 1 && !travellers_increase_number_of_meetings)    {
                if (RandomUniform() < 1./person2->GetNumberOfVisitedMunicipalities())  {
                    i--;    // In this case (contrary to other "continue"s in this part of code), we need another meeting to happen instead
                    continue;
                }
            }
            Person::Meet(person, person2, probability, ConfigParser::GetProbToRememberCityInteractions());
        }
    }

    // Meetings initiated by temporary occupants
    for (Person *person : m_temporary_occupants)    {
        // /2 is there in order to not double count meetings (in average 50% of the meetings is intiated by this person, 50% by the other person)
        const unsigned int interactions = RandomPoisson(average_interactions_per_person/2.);

        for (unsigned int i = 0; i < interactions; i++) {
            // Travelers who visited N cities (including their home city) will initialize 1/N part of their meetings in each city
            if (RandomUniform() < 1./person->GetNumberOfVisitedMunicipalities() && !travellers_increase_number_of_meetings)  {
                continue;
            }
            unsigned int person2_index = RandomUniform()*(number_of_temporary_occupants + m_number_of_inhabitants);
            Person *person2 = nullptr;

            // Interaction with inhabitants of the city
            if (person2_index < m_number_of_inhabitants)  {
               person2 = m_inhabitants.at(person2_index);
            }
            else    // Interaction with temporary occupants
            {
                person2_index -= m_number_of_inhabitants;
                if (person2_index >= number_of_temporary_occupants) {
                    continue; // might happen very rarely because of a finite float precision
                }
                person2 = m_temporary_occupants.at(person2_index);
            }

            // Travelers who visited N cities (including their home city) will have 1/N part of their meetings in each city
            if (person2->GetNumberOfVisitedMunicipalities() > 1 && !travellers_increase_number_of_meetings)    {
                if (RandomUniform() < 1./person2->GetNumberOfVisitedMunicipalities())  {
                    i--;    // In this case (contrary to other "continue"s in this part of code), we need another meeting to happen instead
                    continue;
                }
            }
            Person::Meet(person, person2, probability, ConfigParser::GetProbToRememberCityInteractions());
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
    person->AddVisitedMunicipality();
}

void PopulationCenter::RemoveAllTemporaryOccupants()   {
    for (Person *temporary_occupant : m_temporary_occupants)    {
        temporary_occupant->RemoveVisitedMunicipality();
    }
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
        this->SendTravelersToCity(&cities->at(i_city), RoundProbabilistic(ConfigParser::GetMobility()*m_migrations->at(i_city)));
    }
};

void PopulationCenter::SaveTheDayToHistory()    {
    unsigned int unaffected         = 0;
    unsigned int infected           = 0;
    unsigned int symptomatic        = 0;
    unsigned int asymptomatic       = 0;
    unsigned int immune             = 0;
    unsigned int dead               = 0;
    unsigned int new_cases          = 0;
    unsigned int hospitalized       = 0;
    unsigned int infective          = 0;
    unsigned int critical           = 0;

    for (const Person *person : m_inhabitants)  {
        if (person->IsUnaffected())     unaffected++;
        if (person->IsIll())            infected++;
        if (person->IsInfectiveSymptomatic())    symptomatic++;
        if (person->IsInfectiveAsymptomatic())   asymptomatic++;
        if (person->IsImmune())         immune++;
        if (person->IsDead())           dead++;
        if (person->IsNewCase())        new_cases++;
        if (person->IsHospitalized())   hospitalized++;
        if (person->IsInfective())      infective++;
        if (person->IsCritical())       critical++;
    }

    if (symptomatic + asymptomatic + hospitalized != infective) {
      cout << "Population center: " << m_name << endl;
      cout << "symptomatic: " << symptomatic << endl;
      cout << "asymptomatic: " << asymptomatic << endl;
      cout << "hospitalized: " << hospitalized << endl;
      cout << "infective: " << infective << endl;
      throw "ERROR: Sum of symptomatic, asymptomatic and hospitalized cases does not match infective cases!";
    }

    m_logging_days          .push_back(Person::GetDay());
    m_logging_unaffected    .push_back(unaffected  );
    m_logging_infected      .push_back(infected    );
    m_logging_symptomatic   .push_back(symptomatic );
    m_logging_asymptomatic  .push_back(asymptomatic);
    m_logging_immune        .push_back(immune      );
    m_logging_dead          .push_back(dead        );
    m_logging_new_cases     .push_back(new_cases   );
    m_logging_hospitalized  .push_back(hospitalized);
    m_logging_infective     .push_back(infective);
    m_logging_critical      .push_back(critical);
};

void PopulationCenter::CityAndPersonsFactory(
        const vector<unsigned int> &number_of_inhabitants,
        const vector<string> &names,
        const vector<float> &longitudes,
        const vector<float> &latitudes,
        vector<Person> *population,
        vector<PopulationCenter> *cities) {

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
        cities->push_back(PopulationCenter(population, current_person_index, number_of_inhabitants.at(i)));
        cities->back().SetName(names.at(i));
        cities->back().SetLongitude(longitudes.at(i));
        cities->back().SetLatitude(latitudes.at(i));
        current_person_index += number_of_inhabitants.at(i);
    }
};

void PopulationCenter::SetMigrations(const vector <vector <unsigned int >> *migrations, vector<PopulationCenter> *cities)   {
    for (unsigned int i_city = 0; i_city < migrations->size(); i_city++)   {
        cities->at(i_city).SetTravelMigrations(&migrations->at(i_city));
    }
}
