#include "../sars_cov2_sk/Household.h"
#include "../sars_cov2_sk/Person.h"
#include "../sars_cov2_sk/HelperFunctions.h"
#include "../sars_cov2_sk/ConfigParser.h"

#include <vector>
#include <stdlib.h>
#include <iostream>

using namespace std;
using namespace sars_cov2_sk;

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
        // Can't infect other people if not-infected, hospitalized or dead ...
        if (!(person1->IsInfective() && !person1->IsHospitalized() && !person1->IsDead()))    {
            continue;
        }

        for (Person *person2 : m_inhabitants) {
             // each meeting happens only once (p1 <==> p2 and p2 <==> p1 is the same, we shouldn't repeat it)
            if (person2 == person1) {
                break;
            }

            Person::Meet(person1, person2, probability, 1.);
        }
    }
};

unsigned int Household::GetRandomHouseholdNumber(float average_people_in_household) {
    const float p = RandomUniform();

    // #TODO: For now just random numbers there. It needs to be improved.
    if (p < 0.2)    return 1;
    if (p < 0.45)   return 2;
    if (p < 0.7)    return 3;
    if (p < 0.9)    return 4;
    if (p < 0.95)   return 5;
    if (p < 0.98)   return 6;
    return 7;
};