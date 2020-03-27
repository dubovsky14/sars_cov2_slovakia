#include "../sars_cov2_sk/Household.h"
#include "../sars_cov2_sk/Person.h"

#include <vector>
#include <stdlib.h>

using namespace std;
using namespace sars_cov2_sk;

bool Household::Infected() const    {
    for (const Person *person : m_inhabitants){
        if (person->IsInfectable() && !person->IsHospitalized()) {
            return true;
        }
    }
    return false;
};

void Household::SpreadInfection(float probability)   {
    // person1 is the source of infection, person2 get infected
    for (Person *person1 : m_inhabitants)    {
        // Can't infect other people if not-infected, hospitalized or dead ...
        if (!(person1->IsInfectable() && !person1->IsHospitalized() && !person1->IsDead()))    {
            continue;
        }

        for (Person *person2 : m_inhabitants) {
             // you can't infect yourself
            if (person2 == person1) {
                continue;
            }

            if ((rand() / double(RAND_MAX)) < probability)  {
                person1->Infect();
            }
        }
    }
};