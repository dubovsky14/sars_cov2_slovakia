#include "../sars_cov2_sk/InputInfo.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;
using namespace sars_cov2_sk;

InputInfo::InputInfo(const std::string &municipality_info_text_file)    {
    m_text_file_municipalities = municipality_info_text_file;
    ReadMunicipalityFile();
};

void InputInfo::ReadMunicipalityFile()  {
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
};

void InputInfo::ReadLineOfConfig(string line)  {
    StripString(&line);
    if (line[0] == '#') return; // Enable comments
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

vector<string> InputInfo::SplitAndStripString(string input_string, const string &separator) {
    vector<string> result = SplitString(input_string, separator);
    for (string &x : result)    {
        StripString(&x, " \n\t\r");
    }
    return move(result);
}

vector<string> InputInfo::SplitString(string input_string, const string &separator)    {
    vector<string> result;
    size_t pos = 0;
    std::string token;
    while ((pos = input_string.find(separator)) != std::string::npos) {
        result.push_back(input_string.substr(0, pos));
        input_string.erase(0, pos + separator.length());
    }
    return move(result);
};

void InputInfo::StripString(string *input_string, const string &chars_to_remove)    {
    input_string->erase(0,input_string->find_first_not_of(chars_to_remove));
    input_string->erase(input_string->find_last_not_of(chars_to_remove)+1);
}
