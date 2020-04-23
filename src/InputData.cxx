#include "../sars_cov2_sk/InputData.h"
#include "../sars_cov2_sk/ConfigParser.h"
#include "../sars_cov2_sk/HelperFunctions.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;
using namespace sars_cov2_sk;

InputData *InputData::s_singleton_instance = nullptr;

InputData::InputData()    {
    m_text_file_municipalities = ConfigParser::GetMunicipalitiesFileAddress();
    ReadMunicipalityFile();
    ReadMigrations();
    ReadAgeDistribution();
    ReadAgeSymptomsFile();
    ReadHouseholdFile();
    if (ConfigParser::GetInfectedInitial() < 0) {
        ReadNumberOfInfected();
    }
};

void InputData::Check() {
    if (s_singleton_instance == nullptr)    {
        throw "You have asked me to use InputData, but you did not initialized it! Please use Initialize method first.";
    }
};

void InputData::Initialize()   {
    delete s_singleton_instance;
    s_singleton_instance = new InputData();
};

unsigned int InputData::GetMigrationsTotal() {
    Check();
    unsigned int result = 0;
    for (unsigned int i = 0; i < s_singleton_instance->m_migrations.size(); i++)    {
        for (unsigned int j = 0; j < s_singleton_instance->m_migrations.at(i).size(); j++)    {
            if (j!=i)   {
                result += s_singleton_instance->m_migrations.at(i).at(j);
            }
        }
    }
    return result;
}

void InputData::ReadMunicipalityFile()  {
    string line;
    ifstream input_file (m_text_file_municipalities);
    if (input_file.is_open())    {
        while ( getline (input_file,line) )        {
            ReadLineOfConfig(line);
        }
        input_file.close();
    }
    else    {
        throw "Unable to open file municipality text file!";
    }
    m_number_of_municipalities = m_municipality_id.size();
};

void InputData::ReadLineOfConfig(string line)  {
    StripString(&line);
    if (!ValidLine(line)) return; // Enable comments
    vector<string> elements = SplitAndStripString(line, ";");

    if (elements.size() < 5) {
        throw("I was unable to read the following line of input : " + line);
    }

    const int id          = std::stoi(elements.at(0));
    const string name     = elements.at(1);
    const int population  = std::stoi(elements.at(2));
    const float longitude = std::stof(elements.at(3));
    const float latitude  = std::stof(elements.at(4));

    m_municipality_id                   .push_back(id);
    m_municipality_name                 .push_back(name);
    m_municipality_number_of_inhabitants.push_back(population);
    m_municipality_longitude            .push_back(longitude);
    m_municipality_latitude             .push_back(latitude);
}

void InputData::ReadMigrations()    {
    string line;
    ifstream input_file (ConfigParser::GetMigrationMatrixAddress());
    if (input_file.is_open())    {
        while ( getline (input_file,line) )        {
            ReadLineOfMigrations(line);
        }
        input_file.close();
    }
    else    {
        throw "Unable to open file municipality text file!";
    }
    if (m_migrations.size() != m_number_of_municipalities)   {
        throw ("Incompatible inputs! The size of the migration matrix does not match the size of municipalities info input!\n");
    }
}

void InputData::ReadLineOfMigrations(string line)  {
    StripString(&line);
    if (!ValidLine(line)) return; // Enable comments
    vector<string> elements = SplitAndStripString(line, ";");

    if (elements.size() != m_number_of_municipalities)   {
        cout << "elements.size = " << elements.size() << "\t\t#municip. = " << m_number_of_municipalities << endl;
        throw ("Incompatible inputs! The size of the migration matrix does not match the size of municipalities info input!\n");
    }

    vector<unsigned int> numbers;
    for (const string &x : elements)    {
        numbers.push_back(stoi(x));
    }
    m_migrations.push_back(numbers);
}

void InputData::ReadNumberOfInfected()    {
    // Set number of infected in each municip. to zero at the begining
    m_municipality_number_of_infected.clear();
    for (unsigned int i = 0; i < m_municipality_number_of_inhabitants.size(); i++) {
        m_municipality_number_of_infected.push_back(0);
    }

    string line;
    ifstream input_file (ConfigParser::GetInfectedFileAddress());
    if (input_file.is_open())    {
        while ( getline (input_file,line) )        {
            ReadLineOfInfected(line);
        }
        input_file.close();
    }
    else    {
        throw "Unable to open file text file with number of infected people in municipalities!";
    }
}

void InputData::ReadLineOfInfected(string line)  {
    StripString(&line);
    if (!ValidLine(line)) return; // Enable comments
    vector<string> elements = SplitAndStripString(line, ":");


    if (elements.size() != 2)   {
        throw ("Invalid content of file with number of infected people. Each line must have firstly municip. name or ID followed by : and then by number of infected people!\n");
    }

    // Check if the number after : is really positive int
    if ((elements.at(1).find_first_not_of( "0123456789" ) != std::string::npos))   {
        throw "Invalid content of the following line in number of infected file: \"" + line + "\"";
    }
    const bool first_element_is_id = (elements.at(0).find_first_not_of( "0123456789" ) == std::string::npos);
    if (first_element_is_id)    {
        const unsigned int munic_id = stoi(elements.at(0));
        for (unsigned int i = 0; i < m_municipality_id.size(); i++) {
            if (m_municipality_id.at(i) == munic_id)    {
                m_municipality_number_of_infected.at(i) = stoi(elements.at(1));
                return;
            }
        }
    }
    else    {
        for (unsigned int i = 0; i < m_municipality_id.size(); i++) {
            if (m_municipality_name.at(i) == elements.at(0))    {
                m_municipality_number_of_infected.at(i) = stoi(elements.at(1));
                return;
            }
        }
    }
    throw "I wasn't able to find municipality with name/ID : \"" + elements.at(0) + "\"";
}

void InputData::ReadAgeDistribution()   {
    m_age_distribution.clear();
    for (int i = 0; i < 9; i++) {
        m_age_distribution.push_back(-1.);
    }

    string line;
    ifstream input_file (ConfigParser::ReadStringValue("age_distribution_file"));
    if (input_file.is_open())    {
        while ( getline (input_file,line) )        {
            ReadLineOfAgeDistributuon(line);
        }
        input_file.close();
    }
    else    {
        throw "Unable to open file text file with the population age distribution!";
    }

    float sum = 0;
    for (float age : m_age_distribution)    {
        if (age < 0)    {
            throw ("At least one age category is not defined! Please defined number of people for each of the following categories:\n0, 10, 20, 30, 40, 50, 60, 70, 80");
        }
        sum += age;
    }

    // Normalize
    for (float &age : m_age_distribution)   {
        age /= sum;
    }
}

void InputData::ReadLineOfAgeDistributuon(std::string line) {
    StripString(&line, " \n\t\r{}");
    vector<string> elements = SplitAndStripString(line, ":");
    if (elements.size() != 2)   {
        return;
    }
    if (!StringIsFloat(elements.at(1))) {
        throw ("Cannot read line " + line + " of the age distribution file");
    }
    StripString(&elements.at(0), "\"");
    if (elements.at(0) == "0")    m_age_distribution.at(0) = stod(elements.at(1));
    if (elements.at(0) == "10")   m_age_distribution.at(1) = stod(elements.at(1));
    if (elements.at(0) == "20")   m_age_distribution.at(2) = stod(elements.at(1));
    if (elements.at(0) == "30")   m_age_distribution.at(3) = stod(elements.at(1));
    if (elements.at(0) == "40")   m_age_distribution.at(4) = stod(elements.at(1));
    if (elements.at(0) == "50")   m_age_distribution.at(5) = stod(elements.at(1));
    if (elements.at(0) == "60")   m_age_distribution.at(6) = stod(elements.at(1));
    if (elements.at(0) == "70")   m_age_distribution.at(7) = stod(elements.at(1));
    if (elements.at(0) == "80")   m_age_distribution.at(8) = stod(elements.at(1));
};


void InputData::ReadAgeSymptomsFile()   {
    const string file = ConfigParser::ReadStringValue("age_symptoms_file");
    vector<string> age_categories = {"0", "10", "20", "30", "40", "50", "60", "70", "80"};

    ReadDictionaryFromJSON(file, "symptomatic",     age_categories, &m_age_symptomatic);
    ReadDictionaryFromJSON(file, "hospitalized",    age_categories, &m_age_hospitalized);
    ReadDictionaryFromJSON(file, "critical_care",   age_categories, &m_age_critical);
    ReadDictionaryFromJSON(file, "fatal",           age_categories, &m_age_fatal);
};

void InputData::ReadDictionaryFromJSON( const std::string &json_address, const std::string &dict_name,
                                        const std::vector<std::string> &keys, std::vector<float> *values)    {

    values->clear();
    vector<bool> key_found_vector;
    for (unsigned int i = 0; i < keys.size(); i++)  {
        values->push_back(-1.);
        key_found_vector.push_back(false);
    }

    string line;
    ifstream input_file (json_address);


    bool reading_requested_value = false;
    vector<float> result;
    for (int i = 0; i < 9; i++) {
        result.push_back(-1.);
    }

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
                reading_requested_value = true;
                continue;
            }

            if (!reading_requested_value)   {
                continue;
            }

            StripString(&elements.at(1), ", \t");
            if (!StringIsFloat(elements.at(1)))  {
                throw "I could not read the following line in json \"" + json_address + "\", :" + line;
            }
            const float value = stod(elements.at(1));
            if (reading_requested_value)    {
                bool key_found = false;
                for (unsigned int i = 0; i < keys.size(); i++)  {
                    if (keys.at(i) == key) {
                        values->at(i) = value;
                        key_found_vector.at(i) = true;
                        key_found = true;
                    }
                }

                if (key_found == false) {
                    throw "Unknown key appeared in age symptoms json while reading property \"" + dict_name + "\", the key: \"" + key + "\"";
                }
            }
        }
        input_file.close();
    }
    else    {
        throw "Unable to open file text file with the age symptoms!";
    }

    for (unsigned int i = 0; i < key_found_vector.size(); i++)    {
        if (key_found_vector.at(i) == false)  {
            throw "I could not read value for key \"" + keys.at(i) + "\" in file " + json_address + " dictionary \"" + dict_name + "\"";
        }
    }
};


void InputData::ReadHouseholdFile() {
    ReadHouseholdsElderly();
    ReadHouseholdsYoung();
};

void InputData::ReadHouseholdsElderly() {
    const vector<string> keys = {"with_young", "1", "2"};
    m_elderly_houses.resize(keys.size());
    vector<float> values;
    ReadDictionaryFromJSON(ConfigParser::ReadStringValue("households_file"), "elderly", keys, &values);
    for (unsigned int i = 0; i < keys.size(); i++)   {
        m_elderly_houses.at(i) = values.at(i);
    }
};

void InputData::ReadHouseholdsYoung()   {
    const vector<string> keys = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};
    m_young_houses.resize(keys.size() + 1);
    vector<float> values;
    ReadDictionaryFromJSON(ConfigParser::ReadStringValue("households_file"), "young", keys, &values);
    for (unsigned int i = 0; i < keys.size(); i++)   {
        m_young_houses.at(i+1) = values.at(i);  // we do not have households with 0 inhabitants, that's why "i+1"
    }
};

bool InputData::ValidLine(string line)   {
    StripString(&line);
    if (line.length() == 0) return false;
    if (line[0] == '#')     return false;
    return true;
};
