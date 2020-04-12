#include "../sars_cov2_sk/Person.h"
#include "../sars_cov2_sk/HelperFunctions.h"
#include "../sars_cov2_sk/ConfigParser.h"
#include "../sars_cov2_sk/InputData.h"

#include <vector>
#include <stdlib.h>
#include <iostream>
#include <cmath>

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
    m_visited_cities_today = 1;
}

void Person::Infect()   {
    if (m_seir_status == enum_susceptible)    {
        m_seir_status               = enum_exposed;
        m_day_of_infection          = s_day_index;
        m_date_of_next_status_change= s_day_index + int(RandomGaussWithProbabilisticRounding(ConfigParser::InfectiousStartMean(), ConfigParser::InfectiousStartStd()));

        // Person starts to be infectious on the same day (quite unlikely, but might happen)
        if (m_date_of_next_status_change == s_day_index) {
            Evolve();
        }
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

    ReleaseFromQuarantine();

    // Temporary fix in order to save memory and speed up the code
    ForgetContacts();
}

// #TODO: Check if there is an available bed in hospital
bool Person::Hospitalize() {
    m_is_hospitalized = true;
    PutToQuarantine();

    return true; // true of there is a free hospital bed
}

bool Person::HasSymptoms() const    {
    // being dead is also quite strong symptom, but we are not going to test these people ...
    return (m_seir_status == enum_infective_symptomatic ||
            m_seir_status == enum_needs_hospitalization ||
            m_seir_status == enum_critical);
}

void Person::PutToQuarantine()  {
    m_in_quarantine = true;
}

void Person::ReleaseFromQuarantine()   {
    m_in_quarantine = false;
};

void Person::Evolve()   {
    if (!IsIll()) {
        return;
    }

    if (m_seir_status == enum_exposed)  {
        if (s_day_index >= m_date_of_next_status_change)    {
            // Throwing a dice for symptoms
            const float p = RandomUniform();
            // person has symptoms
            if (p < InputData::GetAgeSymptomatic()->at(m_age_category))   {
                // person has symptoms and after some time it will need hospitalization
                if (m_health_state < InputData::GetAgeHospitalized()->at(m_age_category))   {
                    m_seir_status = enum_infective_symptomatic;
                    m_date_of_next_status_change = s_day_index + RandomGaussWithProbabilisticRounding(ConfigParser::HospitalizationStartMean(),ConfigParser::HospitalizationStartStd());
                }
                // person has symptoms, but it will not need hospitalization
                else  {
                    m_seir_status = enum_infective_symptomatic;
                    m_date_of_next_status_change = s_day_index + RandomGaussWithProbabilisticRounding(ConfigParser::InfectiousDaysMean(),ConfigParser::InfectiousDaysStd());
                }
            }
            // person will have no symptoms
            else {
                m_seir_status = enum_infective_asymptomatic;
                m_date_of_next_status_change = s_day_index + RandomGaussWithProbabilisticRounding(ConfigParser::InfectiousDaysMean(),ConfigParser::InfectiousDaysStd());
            }
            Evolve();
        }
    }

    if (m_seir_status == enum_infective_asymptomatic)    {
        if (s_day_index >= m_date_of_next_status_change)    {
            Heal();

            Evolve();
        }
    }

    if (m_seir_status == enum_infective_symptomatic)    {
        if (s_day_index >= m_date_of_next_status_change)    {
            // The person will recover without a need to be hospitalized
            if (m_health_state > InputData::GetAgeHospitalized()->at(m_age_category))   {
                Heal();
            }
            else {
                m_seir_status = enum_needs_hospitalization;
                Hospitalize();

                // person will need critical care after some time
                if (m_health_state < InputData::GetAgeCritical()->at(m_age_category))   {
                    m_date_of_next_status_change = s_day_index + RandomGaussWithProbabilisticRounding(ConfigParser::CriticalStartMean(),ConfigParser::CriticalStartStd());
                }
                // person will recover without a need for critical care
                else {
                    m_date_of_next_status_change = s_day_index + RandomGaussWithProbabilisticRounding(ConfigParser::HospitalizationLengthMean(),ConfigParser::HospitalizationLengthStd());
                }
            }
            Evolve();
        }
    }

    if (m_seir_status == enum_needs_hospitalization)    {
        if (s_day_index >= m_date_of_next_status_change)    {
            // The person will recover without a need for critical care
            if (m_health_state > InputData::GetAgeCritical()->at(m_age_category))   {
                Heal();
            }
            // The person will need critical health care unit
            else {
                m_seir_status = enum_critical;

                if (m_health_state < InputData::GetAgeFatal()->at(m_age_category))   {
                    m_date_of_next_status_change = s_day_index + RandomGaussWithProbabilisticRounding(ConfigParser::CriticalLengthMean(),ConfigParser::CriticalLengthStd());
                }
                else    {
                    m_date_of_next_status_change = s_day_index + RandomGaussWithProbabilisticRounding(ConfigParser::CriticalLengthMean(),ConfigParser::CriticalLengthStd());
                }
            }
            Evolve();
        }
    }

    if (m_seir_status == enum_critical)  {
        if (s_day_index >= m_date_of_next_status_change)    {
            // person died
            if (m_health_state < InputData::GetAgeFatal()->at(m_age_category))   {
                Kill();
            }
            // person survived
            else {
                Heal();
            }
            Evolve();
        }
    }
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

int Person::CountInPopulation(const vector<Person> &population, seir_status status) {
    int result = 0;
    for (const Person &person : population) {
        if (person.m_seir_status == status)    {
            result++;
        }
    }
    return result;
}

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
