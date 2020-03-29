#include "../sars_cov2_sk/InputData.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;
using namespace sars_cov2_sk;

InputData::InputData(const std::string &municipality_info_text_file)    {
    m_text_file_municipalities = municipality_info_text_file;
    ReadMunicipalityFile();
    ReadMigrations();
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

    if (elements.size() < 3) {
        throw("I was unable to read the following line of input : " + line);
    }   

    const int id          = std::stoi(elements.at(0));
    const int population  = std::stoi(elements.at(1));
    const string name     = elements.at(2);

    m_municipality_id                   .push_back(id);
    m_municipality_number_of_inhabitants.push_back(population);
    m_municipality_name                 .push_back(name);
}

void InputData::ReadMigrations()    {
    string line;
    ifstream input_file ("data/sensitive/migrations.txt");
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