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

    // Calculate cumulative distribution function
    for (unsigned int i = 1; i < m_age_distribution.size(); i++)  {
        m_age_distribution.at(i) = m_age_distribution.at(i) + m_age_distribution.at(i-1);
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
    m_age_symptomatic   = ReadAgeSymptomsProperty("symptomatic");
    m_age_hospitalized  = ReadAgeSymptomsProperty("hospitalized");
    m_age_critical      = ReadAgeSymptomsProperty("critical_care");
    m_age_fatal         = ReadAgeSymptomsProperty("fatal");
};

vector<float> InputData::ReadAgeSymptomsProperty(const string &property)    {
    string line;
    ifstream input_file (ConfigParser::ReadStringValue("age_symptoms_file"));

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
            if (key == property)  {
                reading_requested_value = true;
                continue;
            }

            if (!reading_requested_value)   {
                continue;
            }

            StripString(&elements.at(1), ", \t");
            if (!StringIsFloat(elements.at(1)))  {
                throw "I could not read the following line in age_symptoms_file config\n" + line;
            }
            const float value = stod(elements.at(1));
            if (reading_requested_value)    {
                if      (key == "0")    result.at(0) = value;
                else if (key == "10")   result.at(1) = value;
                else if (key == "20")   result.at(2) = value;
                else if (key == "30")   result.at(3) = value;
                else if (key == "40")   result.at(4) = value;
                else if (key == "50")   result.at(5) = value;
                else if (key == "60")   result.at(6) = value;
                else if (key == "70")   result.at(7) = value;
                else if (key == "80")   result.at(8) = value;
                else {
                    throw "Unknown key appeared in age symptoms json while reading property \"" + property + "\", the key: \"" + key + "\"";
                }
            }
        }
        input_file.close();
    }
    else    {
        throw "Unable to open file text file with the age symptoms!";
    }

    for (unsigned int i = 0; i < result.size(); i++)    {
        if (result.at(i) < 0)  {
            throw "I coult not read age symptom property \"" + property + "\" for age group " + to_string(i*10); 
        }
    }

    return result;
};

vector<string> InputData::SplitAndStripString(string input_string, const string &separator) {
    vector<string> result = SplitString(input_string, separator);
    for (string &x : result)    {
        StripString(&x, " \n\t\r");
    }
    return move(result);
}

vector<string> InputData::SplitString(string input_string, const string &separator)    {
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

void InputData::StripString(string *input_string, const string &chars_to_remove)    {
    input_string->erase(0,input_string->find_first_not_of(chars_to_remove));
    input_string->erase(input_string->find_last_not_of(chars_to_remove)+1);
}

bool InputData::ValidLine(string line)   {
    StripString(&line);
    if (line.length() == 0) return false;
    if (line[0] == '#')     return false;
    return true;
};
