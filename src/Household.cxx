#include "../sars_cov2_sk/Household.h"
#include "../sars_cov2_sk/Person.h"
#include "../sars_cov2_sk/HelperFunctions.h"
#include "../sars_cov2_sk/ConfigParser.h"
#include "../sars_cov2_sk/InputData.h"

#include <vector>
#include <stdlib.h>
#include <iostream>

using namespace std;
using namespace sars_cov2_sk;

bool     Household::s_info_initialized = false;
int      Household::s_n_young            = -1;
int      Household::s_n_elderly          = -1;
int      Household::s_n_households_young    = -1;
int      Household::s_n_households_elderly  = -1;
float    Household::s_average_occupancy_young   = -1.;
float    Household::s_average_occupancy_elderly = -1.;
float    Household::s_fraction_of_elderly_living_with_young = 0.;

void Household::IntializeHouseholdNumbers() {
    if (s_info_initialized) {
        return;
    }
    s_n_elderly = 0;
    s_n_young   = 0;
    s_n_households_young    = 0;
    s_n_households_elderly  = 0;
    s_average_occupancy_young   = 0;
    s_average_occupancy_elderly = 0;

    const vector<float> *age_distribution = InputData::GetAgeDistribution();
    for (unsigned int i = 0; i < age_distribution->size(); i++)    {
        if (i >= 6)  {
            s_n_elderly += age_distribution->at(i);
        }
        else {
            s_n_young += age_distribution->at(i);
        }
    }

    // Calculate average number of persons living in "elderly" household
    float sum = 0;
    const vector<float> *elderly_households = InputData::GetElderlyHouseholdsOccupancy();
    for (unsigned int i = 1; i < elderly_households->size(); i++) {
        sum += elderly_households->at(i);
        s_average_occupancy_elderly += i*elderly_households->at(i);
    }
    s_average_occupancy_elderly *= (1./sum);

    // Calculate average number of persons living in "young" household
    sum = 0;
    const vector<float> *young_households = InputData::GetYoungHouseholdsOccupancy();
    for (unsigned int i = 1; i < young_households->size(); i++) {
        sum += young_households->at(i);
        s_average_occupancy_young += i*young_households->at(i);
    }
    s_average_occupancy_young *= (1./sum);

    // calculate number of households with elderly persons only and the number of "young" households
    s_n_households_young    = float(s_n_young + InputData::GetElderlyFractionLivingWithYoungs()*s_n_elderly)/s_average_occupancy_young;
    s_n_households_elderly  = float((1-InputData::GetElderlyFractionLivingWithYoungs())*s_n_elderly)/s_average_occupancy_elderly;
}


bool Household::Infected() const    {
    for (const Person *person : m_inhabitants){
        if (person->IsInfective() && !person->IsHospitalized()) {
            return true;
        }
    }
    return false;
};

void Household::SpreadInfection(float probability)   {
    // person1 is the source of infection, person2 get infected
    for (Person *person1 : m_inhabitants)    {
        // Can't infect other people if hospitalized or dead ...
        if (person1->IsHospitalized() || person1->IsDead())    {
            continue;
        }

        for (Person *person2 : m_inhabitants) {
             // each meeting happens only once (p1 <==> p2 and p2 <==> p1 is the same, we shouldn't repeat it)
            if (person2 == person1) {
                break;
            }

            // If both persons are healthy, do nothing
            if (!person1->IsIll() && !person2->IsIll())   {
                continue;
            }

            // Can't infect other people if hospitalized or dead ...
            if (person2->IsHospitalized() || person2->IsDead())    {
                continue;
            }

            Person::Meet(person1, person2, probability, 1.);
        }
    }
};

void Household::GetRandomHouseholdNumbers(unsigned int *number_of_inhabitants, HouseholdCategory *category) {
    if (RandomUniform() < (float(s_n_households_elderly)/float(s_n_households_elderly+s_n_households_young)) )  {
        *category = enum_elderly;
        const float max_prob = MaxInVector(*InputData::GetElderlyHouseholdsOccupancy());
        while (true)    {
            const float p           = max_prob*RandomUniform();
            *number_of_inhabitants  = 1+((InputData::GetElderlyHouseholdsOccupancy()->size() - 1)*RandomUniform());
            if (*number_of_inhabitants >= InputData::GetElderlyHouseholdsOccupancy()->size())    {
                continue; // may happen because of the float precision
            }
            if (p < InputData::GetElderlyHouseholdsOccupancy()->at(*number_of_inhabitants)) {
                return;
            }
        }
    }
    else {
        *category = enum_young;
        const float max_prob = MaxInVector(*InputData::GetYoungHouseholdsOccupancy());
        while (true)    {
            const float p           = max_prob*RandomUniform();
            *number_of_inhabitants  = 1+((InputData::GetYoungHouseholdsOccupancy()->size() - 1)*RandomUniform());
            if (*number_of_inhabitants >= InputData::GetYoungHouseholdsOccupancy()->size())    {
                continue; // may happen because of the float precision
            }
            if (p < InputData::GetYoungHouseholdsOccupancy()->at(*number_of_inhabitants)) {
                return;
            }
        }
    }
};