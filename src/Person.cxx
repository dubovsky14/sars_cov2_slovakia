#include "../sars_cov2_sk/Person.h"

#include <vector>
#include <stdlib.h>

using namespace std;
using namespace sars_cov2_sk;

int Person::s_day_index = 0;

Person::Person() {
    m_health_state      = rand() / double(RAND_MAX);
    m_is_ill            = false;
    m_has_symptoms      = false;
    m_is_imune          = false;
    m_is_infectable     = false;
    m_in_quarantine     = false;
    m_is_dead           = false;
    m_day_of_infection  = -1;
}

void Person::Infect()   {
    m_is_ill            = true;
    m_day_of_infection  = s_day_index;
}

void Person::Kill()     {
    m_has_symptoms      = true;
    m_is_imune          = true;
    m_is_infectable     = false;
    m_in_quarantine     = true;
    m_is_dead           = true;
    m_day_of_infection  = -1;    
}

void Person::Heal() {
    m_is_ill            = false;
    m_has_symptoms      = false;
    m_is_imune          = true;
    m_is_infectable     = false;
    m_day_of_infection  = -1;
}

void Person::Hospitalize() {
    m_is_hospitalized = false;
}

void Person::PutToQuarantine()  {
    m_in_quarantine = true;
}

// #TODO: Improve the method (for now everything is deterministic)
void Person::Evolve()   {
    if (!m_is_ill) {
        return;
    }

    const int length_of_infection = s_day_index - m_day_of_infection;
    switch (length_of_infection)    {
        case 3:
            m_is_infectable = true;
            break;
        case 5:
            m_has_symptoms = true;
            break;        
        case 12:
            if (m_health_state < 0.1) 
                m_needs_hospitalization = true;
                Hospitalize();
            break;    
        case 17:
            if (m_needs_hospitalization && !m_is_hospitalized)
                Kill();
            break;

        case 20:
            Heal();
            break;

        default:
            break;
    }
};

void Person::SetDay(int day) {
    s_day_index = day;
};