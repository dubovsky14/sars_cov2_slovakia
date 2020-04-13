#include "../sars_cov2_sk/Logging.h"
#include "../sars_cov2_sk/PopulationCenter.h"
#include "../sars_cov2_sk/ConfigParser.h"

#include<string>
#include<iostream>
#include<fstream>
#include<map>

using namespace std;
using namespace sars_cov2_sk;

Logging::Logging(const std::string &out_address) {
    m_log_file_address = out_address;
    m_log_file.open(m_log_file_address);
    m_log_file << "{\n";
};

Logging::~Logging() {
    m_log_file << "}";
    m_log_file.close();
};

void Logging::DumpVector(const std::string &indent, const std::string &variable_name, const std::vector<unsigned int> &numbers, bool comma_terminated)  {
    m_log_file << indent << "\"" << variable_name << "\" : [";
    for (unsigned int i = 0; i < numbers.size(); i++)   {
        m_log_file << numbers.at(i);
        if (i + 1 != numbers.size())    {
            m_log_file << ", ";
        }
    }
    m_log_file << "]";
    if (comma_terminated) m_log_file << ",";
    m_log_file << "\n";
};

void Logging::DumpCityHistory(const sars_cov2_sk::PopulationCenter &city, bool comma_terminated)   {
    std::string indent = "    ";
    m_log_file << indent << "\"" << city.m_name << "\" : {\n";
    m_log_file << indent << indent << "\"size\" : "         << city.m_number_of_inhabitants << ",\n";
    m_log_file << indent << indent << "\"longitude\" : "    << city.m_longitude << ",\n";
    m_log_file << indent << indent << "\"latitude\" : "     << city.m_latitude << ",\n";

    DumpVector(indent+indent, "simulation_days",    city.m_logging_days,        true);
    DumpVector(indent+indent, "unaffected",         city.m_logging_unaffected,  true);
    DumpVector(indent+indent, "infected",           city.m_logging_infected,    true);
    DumpVector(indent+indent, "symptomatic",        city.m_logging_symptomatic, true);
    DumpVector(indent+indent, "asymptomatic",       city.m_logging_asymptomatic,true);
    DumpVector(indent+indent, "infective",          city.m_logging_infective,   true);
    DumpVector(indent+indent, "immune",             city.m_logging_immune,      true);
    DumpVector(indent+indent, "dead",               city.m_logging_dead,        true);
    DumpVector(indent+indent, "new_cases",          city.m_logging_new_cases,   true);
    DumpVector(indent+indent, "hospitalized",       city.m_logging_hospitalized,true);
    DumpVector(indent+indent, "critical",           city.m_logging_critical,    false);

    if (comma_terminated)   m_log_file << indent << "},\n";
    else                    m_log_file << indent << "}\n";
};

void Logging::DumpHistoryToJson(const std::vector<PopulationCenter> &cities) {
    for (unsigned int i = 0; i < cities.size(); i++)    {
        if (i + 1 == cities.size())     DumpCityHistory(cities.at(i), false);
        else                            DumpCityHistory(cities.at(i), true);
    }
}

void Logging::DumpConfigToJson(bool comma_terminated) {
    ConfigParser::Check();
    map<string, string> *config_map = &ConfigParser::s_singleton_instance->m_map;

    string indent = "    ";
    m_log_file << indent << "\"config\" : {\n";


	map<string, string>::iterator it = config_map->begin();

	while (it != config_map->end())	{
		const string key    = it->first;
		const string value  = it->second;

		it++;
        if (it != config_map->end()) {
		    m_log_file << indent << indent << "\"" << key << "\" : \"" << value << "\"," << endl;
        }
        else {
		    m_log_file << indent << indent << "\"" << key << "\" : \"" << value << "\"" << endl;
        }
	}

    if (comma_terminated)   m_log_file << indent << "},\n";
    else                    m_log_file << indent << "}\n";
};
