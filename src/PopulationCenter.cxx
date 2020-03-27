#include "../sars_cov2_sk/PopulationCenter.h"
#include "../sars_cov2_sk/Household.h"
#include "../sars_cov2_sk/Person.h"

#include <vector>
#include <stdlib.h>

using namespace std;
using namespace sars_cov2_sk;

PopolationCenter::PopolationCenter( vector<Person> *country_population, unsigned int first_citizen_index, 
                                    unsigned int number_of_inhabitants, float average_people_in_household) {
    const unsigned int last_citizen_index = first_citizen_index + number_of_inhabitants;
    if (country_population->size() < last_citizen_index)    {
        throw "We do not have enough people.";
    } 

    // Save pointers to ihabitants to member variable
    for (unsigned int i = first_citizen_index; i < last_citizen_index; i++) {
        m_inhabitants.push_back(&country_population->at(i));
    }
    number_of_inhabitants = m_inhabitants.size();

    BuildAndFillHouseholds(average_people_in_household);
}

void PopolationCenter::BuildAndFillHouseholds(float average_people_in_household)  {
    unsigned int current_person = 0;
    while (current_person < number_of_inhabitants)   {
        m_households.push_back(Household());

        // Randomly generate number of inhabitants and fill the household
        const unsigned int people_in_household = Household::GetRandomHouseholdNumber(average_people_in_household);
        for (unsigned int i = 0; (i < people_in_household) && (current_person < number_of_inhabitants); i++)  {
            m_households.back().AddInhabitant(m_inhabitants.at(current_person));
            current_person++;
        }
    }
};