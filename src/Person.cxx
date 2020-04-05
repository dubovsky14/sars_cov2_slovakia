#include "../sars_cov2_sk/Person.h"
#include "../sars_cov2_sk/HelperFunctions.h"
#include "../sars_cov2_sk/ConfigParser.h"
#include "../sars_cov2_sk/InputData.h"

#include <vector>
#include <stdlib.h>
#include <iostream>

using namespace std;
using namespace sars_cov2_sk;

int Person::s_day_index = 0;

Person::Person() {
    m_age_category      = GenerateRandomAgeCategory();
    m_health_state      = RandomUniform();
    m_seir_status       = enum_susceptible;

    m_in_quarantine     = false;
    m_is_hospitalized   = false;
    m_day_of_infection  = -1;
    m_had_positive_test = false;
}

void Person::Infect()   {
    if (!IsIll())    {
        m_seir_status               = enum_exposed;
        m_day_of_infection          = s_day_index;
        m_date_of_next_status_change= s_day_index + int(RandomGauss(ConfigParser::InfectiousStartMean(), ConfigParser::InfectiousStartStd()));
    }
}
void Person::AddContact(sars_cov2_sk::Person *person)   {
    if (ConfigParser::GetTrackingOption() == disabled)  {
        return;
    }
    if (ConfigParser::GetTrackingOption() == infected_only  &&
        !(person->IsIll() && this->IsIll())) {
            return;
    }

    if (!IsInVector(m_list_of_contacts, person))    {
        m_list_of_contacts.push_back(person);
    }
}

void Person::Kill()     {
    m_seir_status       = enum_dead;
    m_in_quarantine     = true;
    m_is_hospitalized   = false;
}

void Person::Heal() {
    m_seir_status       = enum_immune;
    m_is_hospitalized   = false;

    ReleseFromToQuarantine();

    // Temporary fix in order to save memory and speed up the code
    ForgetContacts();
}

// #TODO: Check if there is an available bed in hospital
bool Person::Hospitalize() {
    m_is_hospitalized = true;
    PutToQuarantine();

    return true; // true of there is a free hospital bed
}

void Person::PutToQuarantine()  {
    m_in_quarantine = true;
}

void Person::ReleseFromToQuarantine()   {
    m_in_quarantine = false;
};

// #TODO: Improve the method (for now everything is deterministic)
void Person::Evolve()   {
    if (!IsIll()) {
        return;
    }

    if (m_seir_status == enum_exposed)  {
        if (m_date_of_next_status_change <= s_day_index)    {
            // Throwing a dice for symptoms
            const float p = RandomUniform();
            if (p < InputData::GetAgeSymptomatic()->at(m_age_category))   {
                // person has symptoms and after some time it will need hospitalization
                if (m_health_state < InputData::GetAgeHospitalized()->at(m_age_category))   {
                    m_seir_status = enum_infective_symptomatic;
                    m_date_of_next_status_change = m_day_of_infection + RandomGauss(ConfigParser::HospitalizationStartMean(),ConfigParser::HospitalizationStartStd());
                }
                // person has symptoms, but it will not need hospitalization
                else  {
                    m_seir_status = enum_infective_symptomatic;
                    m_date_of_next_status_change = s_day_index + RandomGauss(ConfigParser::InfectiousDaysMean(),ConfigParser::InfectiousDaysStd());
                }
            }
            else {
                m_seir_status = enum_infective_asymptomatic;
                m_date_of_next_status_change = s_day_index + RandomGauss(ConfigParser::InfectiousDaysMean(),ConfigParser::InfectiousDaysStd());
            }
        }
        return;
    }

    if (m_seir_status == enum_infective_asymptomatic)    {
        if (m_date_of_next_status_change <= s_day_index)    {
            Heal();
        }
        return;
    }

    if (m_seir_status == enum_infective_symptomatic)    {
        if (m_date_of_next_status_change <= s_day_index)    {
            // The person will recover without a need to be hospitalized
            if (m_health_state > InputData::GetAgeHospitalized()->at(m_age_category))   {
                Heal();
            }
            else {
                m_seir_status = enum_needs_hospitalization;
                Hospitalize();

                // person will need critical care after some time
                if (m_health_state < InputData::GetAgeCritical()->at(m_age_category))   {
                    m_date_of_next_status_change = s_day_index + RandomGauss(ConfigParser::CriticalStartMean(),ConfigParser::CriticalStartStd());
                }
                // person will recover without a need for critical care
                else {
                    m_date_of_next_status_change = s_day_index + RandomGauss(ConfigParser::HospitalizationLengthMean(),ConfigParser::HospitalizationLengthStd());
                }
            }
        }
        return;
    }

    if (m_seir_status == enum_needs_hospitalization)    {
        if (m_date_of_next_status_change <= s_day_index)    {
            // The person will recover without a need for critical care
            if (m_health_state > InputData::GetAgeCritical()->at(m_age_category))   {
                Heal();
            }
            else {
                m_seir_status = enum_critical;

                // person will die
                if (m_health_state < InputData::GetAgeFatal()->at(m_age_category))   {
                    m_date_of_next_status_change = s_day_index + RandomExponential(ConfigParser::CriticalLengthMean());
                }
                // person will survive
                else {
                    m_date_of_next_status_change = s_day_index + RandomGauss(ConfigParser::CriticalLengthMean(),ConfigParser::CriticalLengthStd());
                }
            }
        }
        return;
    }

    if (m_seir_status == enum_critical)  {
        if (m_date_of_next_status_change <= s_day_index)    {
            // person died
            if (m_health_state < InputData::GetAgeFatal()->at(m_age_category))   {
                Kill();
            }
            // person survived
            else {
                Heal();
            }
        }
        return;
    }
};

bool Person::IsIll()            const   {
    return !(m_seir_status == enum_susceptible || m_seir_status == enum_immune || m_seir_status == enum_dead);
};

bool Person::HasSymptoms()      const   {
    return (IsIll() && m_seir_status != enum_infective_asymptomatic);
}

bool Person::IsInfective()      const   {
    return (IsIll() && !(m_seir_status == enum_exposed));
};

bool Person::NeedsHospitalization() const   {
    return (m_seir_status == enum_needs_hospitalization || m_seir_status == enum_critical);
};

bool Person::IsNewCase()        const   {
    return (IsIll() && (s_day_index == m_day_of_infection));
};

void Person::ForgetContacts()   {
    m_list_of_contacts.clear();
    m_list_of_contacts.shrink_to_fit();
};

void Person::SetDay(int day) {
    s_day_index = day;
};

void Person::Meet(Person *person1, Person *person2, float transmission_probability, float probability_to_remember)   {
    // You can't meet yourself. But this situation might happen in the code, although it's very unlikely
    if (person1 == person2) {
        return;
    }

    // If one of the persons is in quarantine, the meeting cannot happen
    if (person1->InQuarantine() || person2->InQuarantine()) {
        return;
    }

    // Do nothing if nobody is infected
    if (!person1->IsIll() && !person2->IsIll()) {
        return;
    }

    // We do not remember all people we met ...
    const bool remember = RandomUniform() < probability_to_remember;

    // if both of them are infected, just keep track of the contact
    if (person1->IsIll() && person2->IsIll())   {
        if (remember) person1->AddContact(person2);
        if (remember) person2->AddContact(person1);
        return;
    }

    // If person is infected, we have to keep track of its contacts and spread the virus
    if (person1->IsIll() && !person2->IsIll())   {
        const bool spread_virus = RandomUniform() < transmission_probability;
        if (spread_virus)   {
            if (person1->IsInfective())    person2->Infect();
            if (remember) person2->AddContact(person1);
        }
        if (spread_virus || ConfigParser::GetTrackingOption() == all)   {
            if (remember) person1->AddContact(person2);
        }
        return;
    }

    // If person is infected, we have to keep track of its contacts and spread the virus
    if (!person1->IsIll() && person2->IsIll())   {
        const bool spread_virus = RandomUniform() < transmission_probability;
        if (spread_virus)   {
            if (person2->IsInfective())    person1->Infect();
            if (remember) person1->AddContact(person1);
        }

        // if tracking == all, save the contact, other options are considered by AddContact() method itself
        if (spread_virus || ConfigParser::GetTrackingOption() == all)   {
            if (remember) person2->AddContact(person2);
        }
        return;
    }
};


int Person::GetNumberOfInfectedPersonsInPopulation(const vector<Person *> &population) {
    int infected = 0;
    for (Person *person : population) {
        if (person->IsIll())    {
            infected++;
        }
    }
    return infected;
};

int Person::GetNumberOfInfectedPersonsInPopulation(const vector<Person> &population){
    int infected = 0;
    for (const Person &person : population) {
        if (person.IsIll())    {
            infected++;
        }
    }
    return infected;
};

int Person::GenerateRandomAgeCategory() {
    const double rand = RandomUniform();
    const vector<float> *age_distribution = InputData::GetAgeDistribution();
    for (unsigned int i = 0; i < age_distribution->size(); i++)    {
        if (rand < age_distribution->at(i))  {
            return i;
        }
    }
    // extremely unlikely, but might happen because of a finite double precision ...
    return age_distribution->size() - 1;
}