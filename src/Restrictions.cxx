#include "../sars_cov2_sk/Restrictions.h"

#include "../sars_cov2_sk/ConfigParser.h"
#include "../sars_cov2_sk/InputData.h"
#include "../sars_cov2_sk/HelperFunctions.h"
#include "../sars_cov2_sk/Person.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

using namespace std;
using namespace sars_cov2_sk;

Restrictions *Restrictions::s_singleton_instance = nullptr;

Restrictions::Restrictions(const std::string &resctrictions_file)    {
    const unsigned int n_simulation_days = ConfigParser::ReadIntValue("simulation_days");
    float current_limit_mobility                        = 1.;
    float current_limit_elderly_stochastic_interactions = 1.;
    float current_limit_stochastic_interactions         = 1.;
    m_smart_restrictions_type = enum_smart_restrictions_none;
    m_smart_restrictions_active = false;
    m_population = nullptr;

    m_smart_limit_mobility                          = -1.;
    m_smart_limit_elderly_stochastic_interactions   = -1.;
    m_smart_limit_stochastic_interactions           = -1.;

    // Read daily restrictions
    for (unsigned int i = 0; i < n_simulation_days; i++)    {
        if (resctrictions_file != "")   {
            std::map<string, float> *current_day_dictionary = ReadStringFloatDictionary(resctrictions_file, std::to_string(i));

            // if there is a dictionary for that line, update the restrictions which are defined for that day
            // if there are no resctrictions for the day, use previous values. Do the same if a particular restriction is not defined.
            if (current_day_dictionary) {
                if (current_day_dictionary->find("limit_mobility") != current_day_dictionary->end())  {
                    current_limit_mobility =  current_day_dictionary->at("limit_mobility");
                }

                if (current_day_dictionary->find("limit_elderly_stochastic_interactions") != current_day_dictionary->end())  {
                    current_limit_elderly_stochastic_interactions =  current_day_dictionary->at("limit_elderly_stochastic_interactions");
                }

                if (current_day_dictionary->find("limit_stochastic_interactions") != current_day_dictionary->end())  {
                    current_limit_stochastic_interactions =  current_day_dictionary->at("limit_stochastic_interactions");
                }
            }
            delete current_day_dictionary;
            current_day_dictionary = nullptr;
        }

        m_limit_mobility.push_back(current_limit_mobility);
        m_limit_elderly_stochastic_interactions.push_back(current_limit_elderly_stochastic_interactions);
        m_limit_stochastic_interactions.push_back(current_limit_stochastic_interactions);
    }

    InitializeInteligentRestrictions(resctrictions_file);
};

void Restrictions::InitializeInteligentRestrictions(const std::string &resctrictions_file)   {
    std::map<string, float> *critical_dictionary            = ReadStringFloatDictionary(resctrictions_file, "critical");
    std::map<string, float> *hospitalized_all_dictionary    = ReadStringFloatDictionary(resctrictions_file, "hospitalized_all");
    std::map<string, float> *positive_tests_dictionary      = ReadStringFloatDictionary(resctrictions_file, "positive_tests");

    int smart_restrictionss_count = 0;

    if (critical_dictionary)    {
        m_smart_restrictions_type = enum_smart_restrictions_critical;
        SetValuesForInteligentRestrictions(critical_dictionary);
        smart_restrictionss_count++;
    }

    if (hospitalized_all_dictionary)    {
        m_smart_restrictions_type = enum_smart_restrictions_hospitalized_all;
        SetValuesForInteligentRestrictions(hospitalized_all_dictionary);
        smart_restrictionss_count++;
    }

    if (positive_tests_dictionary)    {
        m_smart_restrictions_type = enum_smart_restrictions_positive_tests;
        SetValuesForInteligentRestrictions(positive_tests_dictionary);
        smart_restrictionss_count++;
    }

    delete critical_dictionary;
    delete hospitalized_all_dictionary;
    delete positive_tests_dictionary;

    if (smart_restrictionss_count > 1)  {
        throw "You have defined multiple smart restrictions, but only one is allowed!";
    }

}

void Restrictions::SetValuesForInteligentRestrictions(std::map<string, float> *restrictions_dictionary) {
    if (restrictions_dictionary->find("turn_on")  == restrictions_dictionary->end() ||
        restrictions_dictionary->find("turn_off") == restrictions_dictionary->end())  {
            throw "Please set trigger counts (turn_on and turn_off) for smart restrictions!";
    }
    bool restrictions_set = false;

    m_trigger_count_on  = restrictions_dictionary->at("turn_on");
    m_trigger_count_off = restrictions_dictionary->at("turn_off");

    if (restrictions_dictionary->find("limit_stochastic_interactions") != restrictions_dictionary->end()) {
        m_smart_limit_stochastic_interactions = restrictions_dictionary->at("limit_stochastic_interactions");
        restrictions_set = true;
    }

    if (restrictions_dictionary->find("limit_elderly_stochastic_interactions") != restrictions_dictionary->end()) {
        m_smart_limit_elderly_stochastic_interactions = restrictions_dictionary->at("limit_elderly_stochastic_interactions");
        restrictions_set = true;
    }

    if (restrictions_dictionary->find("limit_mobility") != restrictions_dictionary->end()) {
        m_smart_limit_mobility = restrictions_dictionary->at("limit_mobility");
        restrictions_set = true;
    }

    if (!restrictions_set)   {
        throw "You have defined smart restrictions with trigger counts, but you haven't defined their effect. Please check!";
    }
}

void Restrictions::CheckNeedForSmartRestrictions()  {
    Check();
    if (m_smart_restrictions_type == enum_smart_restrictions_none)   {
        m_smart_restrictions_active = false;
        return;
    }

    if (m_population == nullptr)    {
        throw "You asked me to apply smart restrictions, but you haven't provided pointer to population.";
    }

    if (m_smart_restrictions_type == enum_smart_restrictions_critical)   {
        const int n_count = Person::CountInPopulation(*m_population, enum_critical);
        if (!m_smart_restrictions_active && n_count > m_trigger_count_on)   {
            m_smart_restrictions_active = true;
            return;
        }
        if (m_smart_restrictions_active && n_count < m_trigger_count_off)   {
            m_smart_restrictions_active = false;
            return;
        }
        return;
    }

    if (m_smart_restrictions_type == enum_smart_restrictions_hospitalized_all)   {
        const int n_count = Person::CountInPopulation(*m_population, enum_critical) + Person::CountInPopulation(*m_population, enum_needs_hospitalization);
        if (!m_smart_restrictions_active && n_count > m_trigger_count_on)   {
            m_smart_restrictions_active = true;
            return;
        }
        if (m_smart_restrictions_active && n_count < m_trigger_count_off)   {
            m_smart_restrictions_active = false;
            return;
        }
        return;
    }

    if (m_smart_restrictions_type == enum_smart_restrictions_positive_tests)   {
        int n_count = 0;
        for (const Person &person: *m_population)    {
            if ((person.GetDateOfTest()+1 == Person::GetDay()) && person.PositivelyTesed()) {
                n_count++;
            }
        }

        if (!m_smart_restrictions_active && n_count > m_trigger_count_on)   {
            m_smart_restrictions_active = true;
            return;
        }
        if (m_smart_restrictions_active && n_count < m_trigger_count_off)   {
            m_smart_restrictions_active = false;
            return;
        }
        return;
    }
};

void Restrictions::Initialize()    {
    if (s_singleton_instance)   {
        throw "You have tried to initialize Resctrictions singleton multiple times!";
    }
    s_singleton_instance = new Restrictions(ConfigParser::GetRestrictionsFile());
};

void Restrictions::Check()  {
    if (s_singleton_instance == nullptr)    {
        throw "You have asked me to use Resctrictions class, but you did not initialized it! Please use Initialize method first.";
    }
}


void Restrictions::UpdateDay(int day)  {
    Check();
    s_singleton_instance->CheckNeedForSmartRestrictions();
    if (day >= int(s_singleton_instance->m_limit_mobility.size()))   {
        throw "You asked me to update restrictions for day " + to_string(day) + ", but the number of simuation days is only " + to_string(s_singleton_instance->m_limit_mobility.size());
    }
    s_singleton_instance->m_today_limit_mobility = s_singleton_instance->m_limit_mobility.at(day);
    s_singleton_instance->m_today_limit_elderly_stochastic_interactions = s_singleton_instance->m_limit_elderly_stochastic_interactions.at(day);
    s_singleton_instance->m_today_limit_stochastic_interactions = s_singleton_instance->m_limit_stochastic_interactions.at(day);

    if (s_singleton_instance->m_smart_restrictions_active)    {
        if (s_singleton_instance->m_smart_limit_mobility                         > 0) {
            s_singleton_instance->m_today_limit_mobility = s_singleton_instance->m_smart_limit_mobility;
        }
        if (s_singleton_instance->m_smart_limit_elderly_stochastic_interactions  > 0) {
            s_singleton_instance->m_today_limit_elderly_stochastic_interactions = s_singleton_instance->m_smart_limit_elderly_stochastic_interactions;
        }
        if (s_singleton_instance->m_smart_limit_stochastic_interactions          > 0) {
            s_singleton_instance->m_today_limit_stochastic_interactions = s_singleton_instance->m_smart_limit_stochastic_interactions;
        }
    }
}

std::map<std::string, float> *Restrictions::ReadStringFloatDictionary(const std::string &json_file, const std::string &dict_name) {
    string line;
    ifstream input_file (json_file);

    std::map<string, float> *result = nullptr;

    bool reading_requested_value = false;
    if (input_file.is_open())    {
        while ( getline (input_file,line) )        {
            vector<string> elements = SplitAndStripString(line, ":");

            // stop reading at the end of dictionary
            if (elements.at(0).length() > 0 && reading_requested_value)  {
                if (elements.at(0)[0] == '}')   {
                    reading_requested_value = false;
                    break;
                }
            }

            if (elements.size() != 2) {
                continue;
            }

            string key = elements.at(0);
            StripString(&key, "\"");

            // start reading
            if (key == dict_name)  {
                result = new std::map<std::string, float>;
                reading_requested_value = true;
                continue;
            }

            if (!reading_requested_value)   {
                continue;
            }

            StripString(&elements.at(1), ", \t");
            if (!StringIsFloat(elements.at(1)))  {
                throw "I could not read the following line in json \"" + json_file + "\", :" + line;
            }
            const float value = stod(elements.at(1));

            // #TODO: Check for multiple defintions
            if (reading_requested_value)    {
                // Check whether the value is already in dictiory. If yes, rise exception
                std::map<string, float>::iterator it = result->find(key);
                if (it != result->end())  {
                    throw "Expression \"" + key + "\" defined multiple two times in restrictions file in dictionary \"" + dict_name + "\". Please check!";
                }

                (*result)[key]=value;
            }
        }
        input_file.close();
    }
    else    {
        return nullptr;
    }

    return result;
};

