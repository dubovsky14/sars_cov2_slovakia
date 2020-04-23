#include "../sars_cov2_sk/Restrictions.h"

#include "../sars_cov2_sk/ConfigParser.h"
#include "../sars_cov2_sk/InputData.h"
#include "../sars_cov2_sk/HelperFunctions.h"

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
};

void Restrictions::Initialize(const std::string &resctrictions_file)    {
    if (s_singleton_instance)   {
        throw "You have tried to initialize Resctrictions singleton multiple times!";
    }
    s_singleton_instance = new Restrictions(resctrictions_file);
};

void Restrictions::Check()  {
    if (s_singleton_instance == nullptr)    {
        throw "You have asked me to use Resctrictions class, but you did not initialized it! Please use Initialize method first.";
    }
}


void Restrictions::UpdateDay(int day)  {
    Check();
    if (day >= int(s_singleton_instance->m_limit_mobility.size()))   {
        throw "You asked me to update restrictions for day " + to_string(day) + ", but the number of simuation days is only " + to_string(s_singleton_instance->m_limit_mobility.size());
    }
    s_singleton_instance->m_today_limit_mobility = s_singleton_instance->m_limit_mobility.at(day);
    s_singleton_instance->m_today_limit_elderly_stochastic_interactions = s_singleton_instance->m_limit_elderly_stochastic_interactions.at(day);
    s_singleton_instance->m_today_limit_stochastic_interactions = s_singleton_instance->m_limit_stochastic_interactions.at(day);

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
        throw "Unable to open file json file with restrictions!";
    }

    return result;
};

