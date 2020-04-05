#include "../sars_cov2_sk/ConfigParser.h"

#include <string>
#include <vector>
#include <fstream>
#include <iostream>


using namespace std;
using namespace sars_cov2_sk;

ConfigParser *ConfigParser::s_singleton_instance = nullptr;

ConfigParser::ConfigParser(const std::string &config_address)    {
    string line;
    ifstream input_file (config_address);
    if (input_file.is_open())    {
        while ( getline (input_file,line) )        {
            ReadLineOfConfig(line);
        }
        input_file.close();
    }
    else    {
        throw "Unable to open config file \"" + config_address + "\"";
    }
    InitializeValues();
}

void ConfigParser::ReadLineOfConfig(string line)  {
    StripString(&line);
    if (!ValidLine(line)) return;
    vector<string> elements = SplitAndStripString(line, ":");

    if (elements.size() != 2) {
        return;
    }

    // Check whether the value is already in dictiory. If yes, rise exception
    std::map<string, string>::iterator it = m_map.find(elements.at(0));
    if (it != m_map.end())  {
        throw "Expression \"" + elements.at(0) + "\" defined multiple two times in config file. Please fix it!";
    }

    // Save key and value to dictionary
    m_map[elements.at(0)] = elements.at(1);
}

string ConfigParser::GetStringValue(const std::string &key)  {
    std::map<string, string>::iterator it = m_map.find(key);
    if (it == m_map.end())  {
        throw "Expression \"" + key + "\" not defined in config file. Please fix it!";
    }
    return m_map[key];
};

int   ConfigParser::GetIntValue   (const std::string &key)  {
    const string value = GetStringValue(key);
    try {
        return std::stoi(value);
    }
    catch (...) {
        throw ("Expression \"" + key + "\" defined in config file must be numberical integer value. Please fix it!");
    }
};

float ConfigParser::GetFloatValue (const std::string &key)  {
    const string value = GetStringValue(key);
    try {
        return std::stod(value);
    }
    catch (...) {
        throw ("Expression \"" + key + "\" defined in config file must be floating point numerical value. Please fix it!");
    }
};

void ConfigParser::SetTrackingOption() {
    const string tracking_string = GetStringValue("tracking");
    if (tracking_string == "disabled")          {
        m_tracking_option = disabled;
    }
    else if (tracking_string == "infected_only") {
        m_tracking_option = infected_only;
    }
    else if (tracking_string == "all")  {
        m_tracking_option = all;
    }
    else {
        throw("Value for tracking has to be one of the following: disabled, all, infected_only. Please fix it in the config!");
    }
}

vector<string> ConfigParser::SplitAndStripString(string input_string, const string &separator) {
    vector<string> result = SplitString(input_string, separator);
    for (string &x : result)    {
        StripString(&x, " \n\t\r");
    }
    return move(result);
}

vector<string> ConfigParser::SplitString(string input_string, const string &separator)    {
    vector<string> result;
    size_t pos = 0;
    std::string token;
    while ((pos = input_string.find(separator)) != std::string::npos) {
        result.push_back(input_string.substr(0, pos));
        input_string.erase(0, pos + separator.length());
    }
    if (input_string.length() > 0) result.push_back(input_string);
    return move(result);
};

void ConfigParser::StripString(string *input_string, const string &chars_to_remove)    {
    input_string->erase(0,input_string->find_first_not_of(chars_to_remove));
    input_string->erase(input_string->find_last_not_of(chars_to_remove)+1);
}

bool ConfigParser::ValidLine(string line)   {
    StripString(&line);
    if (line.length() == 0) return false;
    if (line[0] == '#')     return false;
    return true;
};

void ConfigParser::InitializeValues()   {
    SetTrackingOption();
    m_transmission_prob_household               = GetFloatValue("transmission_probability_household");
    m_transmission_prob_city                    = GetFloatValue("transmission_probability");
    m_average_number_of_city_interactions       = GetFloatValue("mean_stochastic_interactions");
    m_probability_to_remember_city_interactions = GetFloatValue("probability_to_remember_random_meeting");
    m_migration_matrix_address                  = GetStringValue("migration_matrix_txt");
    m_municipalities_file                       = GetStringValue("populations_file_txt");
    m_result_file                               = GetStringValue("result_file");

    m_infectious_start_mean         = GetFloatValue("infectious_start_mean");
    m_infectious_start_std          = GetFloatValue("infectious_start_std");
    m_infectious_days_mean          = GetFloatValue("infectious_days_mean");
    m_infectious_days_std           = GetFloatValue("infectious_days_std");
    m_hospitalization_start_mean    = GetFloatValue("hospitalization_start_mean");
    m_hospitalization_start_std     = GetFloatValue("hospitalization_start_std");
    m_hospitalization_length_mean   = GetFloatValue("hospitalization_length_mean");
    m_hospitalization_length_std    = GetFloatValue("hospitalization_length_std");
    m_critical_care_start_mean      = GetFloatValue("critical_care_start_mean");
    m_critical_care_start_std       = GetFloatValue("critical_care_start_std");
    m_critical_care_length_mean     = GetFloatValue("critical_care_length_mean");
    m_critical_care_length_std      = GetFloatValue("critical_care_length_std");

    // Number of infected can be either defined in the config centraly for whole country, or in the text file with number of infected in municipalities
    try {
        m_infected_initial = GetIntValue("initial_number_of_infected");
    }
    catch (string e) {
        m_infected_initial = -1;
        m_infected_file                             = GetStringValue("infected_distribution_file");
    }
};

void ConfigParser::Check()    {
    if (!s_singleton_instance)   {
        throw ("You have not initialized config file, but you tried to use one of the Get methods. Please initialize it first!");
    }
}

void ConfigParser::InitializeConfig(const std::string &config_address)   {
    if (s_singleton_instance)   {
        cout << "Config already initialized!\n";
        return;
    }
    s_singleton_instance = new ConfigParser(config_address);
};