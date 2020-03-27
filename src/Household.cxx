#include "../sars_cov2_sk/Household.h"
#include "../sars_cov2_sk/Person.h"

#include <vector>

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

void Household::SpreadInfection();