#include "../sars_cov2_sk/Logging.h"
#include "../sars_cov2_sk/PopulationCenter.h"
#include "../sars_cov2_sk/Person.h"
#include "../sars_cov2_sk/ConfigParser.h"
#include "../sars_cov2_sk/HelperFunctions.h"
#include "../sars_cov2_sk/ROutToReffConvertor.h"

#include<string>
#include<iostream>
#include<fstream>
#include<map>
#include<algorithm>

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

void Logging::DumpCityHistory(const sars_cov2_sk::PopulationCenter &city, bool comma_terminated)   {
    std::string indent = "    ";
    m_log_file << indent << "\"" << city.m_name << "\" : {\n";
    m_log_file << indent << indent << "\"size\" : "         << city.m_number_of_inhabitants << ",\n";
    m_log_file << indent << indent << "\"longitude\" : "    << city.m_longitude << ",\n";
    m_log_file << indent << indent << "\"latitude\" : "     << city.m_latitude << ",\n";

    DumpVector(indent+indent, "simulation_days",        city.m_logging_days,                true);
    DumpVector(indent+indent, "unaffected",             city.m_logging_unaffected,          true);
    DumpVector(indent+indent, "exposed",                city.m_logging_exposed,             true);
    DumpVector(indent+indent, "infected",               city.m_logging_infected,            true);
    DumpVector(indent+indent, "symptomatic",            city.m_logging_symptomatic,         true);
    DumpVector(indent+indent, "asymptomatic",           city.m_logging_asymptomatic,        true);
    DumpVector(indent+indent, "infective",              city.m_logging_infective,           true);
    DumpVector(indent+indent, "immune",                 city.m_logging_immune,              true);
    DumpVector(indent+indent, "dead",                   city.m_logging_dead,                true);
    DumpVector(indent+indent, "new_cases",              city.m_logging_new_cases,           true);
    DumpVector(indent+indent, "hospitalized",           city.m_logging_hospitalized,        true);
    DumpVector(indent+indent, "critical",               city.m_logging_critical,            true);
    DumpVector(indent+indent, "positive_tests_today",   city.m_logging_positive_tests_today,true);
    DumpVector(indent+indent, "negative_tests_today",   city.m_logging_negative_tests_today,false);


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

void Logging::DumpGlobalVariablesToJson(const vector<PopulationCenter> &cities, const vector<Person> &population, bool comma_terminated)    {
    string indent = "    ";
    m_log_file << indent << "\"global\" : {\n";

    // Generations
    vector<int> generations;
    for (const Person &person:population)   {
        generations.push_back(person.GetGeneration());
    }
    vector<int> generations_counts;
    generations_counts.resize(MaxInVector(generations) + 1);
    for (int gen : generations) {
        if (gen > 0)    {
            generations_counts.at(gen)++;
        }
    }
    DumpVector(indent+indent, "generations",    generations_counts,     true);

    const float R0 = Get_R_eff();

    if (cities.size() > 0)  {
        vector<unsigned int> logging_days = cities.at(0).m_logging_days;
        vector<unsigned int> logging_unaffected(logging_days.size(), 0);
        vector<unsigned int> logging_exposed(logging_days.size(), 0);
        vector<unsigned int> logging_infected(logging_days.size(), 0);
        vector<unsigned int> logging_symptomatic(logging_days.size(), 0);
        vector<unsigned int> logging_asymptomatic(logging_days.size(), 0);
        vector<unsigned int> logging_infective(logging_days.size(), 0);
        vector<unsigned int> logging_immune(logging_days.size(), 0);
        vector<unsigned int> logging_dead(logging_days.size(), 0);
        vector<unsigned int> logging_new_cases(logging_days.size(), 0);
        vector<unsigned int> logging_hospitalized(logging_days.size(), 0);
        vector<unsigned int> logging_critical(logging_days.size(), 0);
        vector<unsigned int> logging_positive_tests_today(logging_days.size(), 0);
        vector<unsigned int> logging_negative_tests_today(logging_days.size(), 0);
        vector<float>        logging_R_eff(logging_days.size(), 0);

        for (const PopulationCenter &city : cities) {
            std::transform(logging_unaffected.begin(),          logging_unaffected.end(),          city.m_logging_unaffected.begin(),          logging_unaffected.begin(),          std::plus<unsigned int>()) ;
            std::transform(logging_infected.begin(),            logging_infected.end(),            city.m_logging_infected.begin(),            logging_infected.begin(),            std::plus<unsigned int>()) ;
            std::transform(logging_exposed.begin(),             logging_exposed.end(),             city.m_logging_exposed.begin(),             logging_exposed.begin(),             std::plus<unsigned int>()) ;
            std::transform(logging_symptomatic.begin(),         logging_symptomatic.end(),         city.m_logging_symptomatic.begin(),         logging_symptomatic.begin(),         std::plus<unsigned int>()) ;
            std::transform(logging_asymptomatic.begin(),        logging_asymptomatic.end(),        city.m_logging_asymptomatic.begin(),        logging_asymptomatic.begin(),        std::plus<unsigned int>()) ;
            std::transform(logging_infective.begin(),           logging_infective.end(),           city.m_logging_infective.begin(),           logging_infective.begin(),           std::plus<unsigned int>()) ;
            std::transform(logging_immune.begin(),              logging_immune.end(),              city.m_logging_immune.begin(),              logging_immune.begin(),              std::plus<unsigned int>()) ;
            std::transform(logging_dead.begin(),                logging_dead.end(),                city.m_logging_dead.begin(),                logging_dead.begin(),                std::plus<unsigned int>()) ;
            std::transform(logging_new_cases.begin(),           logging_new_cases.end(),           city.m_logging_new_cases.begin(),           logging_new_cases.begin(),           std::plus<unsigned int>()) ;
            std::transform(logging_hospitalized.begin(),        logging_hospitalized.end(),        city.m_logging_hospitalized.begin(),        logging_hospitalized.begin(),        std::plus<unsigned int>()) ;
            std::transform(logging_critical.begin(),            logging_critical.end(),            city.m_logging_critical.begin(),            logging_critical.begin(),            std::plus<unsigned int>()) ;
            std::transform(logging_positive_tests_today.begin(),logging_positive_tests_today.end(),city.m_logging_positive_tests_today.begin(),logging_positive_tests_today.begin(),std::plus<unsigned int>()) ;
            std::transform(logging_negative_tests_today.begin(),logging_negative_tests_today.end(),city.m_logging_negative_tests_today.begin(),logging_negative_tests_today.begin(),std::plus<unsigned int>()) ;
        }

        for (unsigned int i_day = 0; i_day < logging_R_eff.size(); i_day++) {
            logging_R_eff.at(i_day) = R0*logging_unaffected.at(i_day)/float(population.size());
        }

        DumpVector(indent+indent, "simulation_days",        logging_days,                true);
        DumpVector(indent+indent, "unaffected",             logging_unaffected,          true);
        DumpVector(indent+indent, "exposed",                logging_exposed,             true);
        DumpVector(indent+indent, "infected",               logging_infected,            true);
        DumpVector(indent+indent, "symptomatic",            logging_symptomatic,         true);
        DumpVector(indent+indent, "asymptomatic",           logging_asymptomatic,        true);
        DumpVector(indent+indent, "infective",              logging_infective,           true);
        DumpVector(indent+indent, "immune",                 logging_immune,              true);
        DumpVector(indent+indent, "dead",                   logging_dead,                true);
        DumpVector(indent+indent, "new_cases",              logging_new_cases,           true);
        DumpVector(indent+indent, "hospitalized",           logging_hospitalized,        true);
        DumpVector(indent+indent, "critical",               logging_critical,            true);
        DumpVector(indent+indent, "positive_tests_today",   logging_positive_tests_today,true);
        DumpVector(indent+indent, "negative_tests_today",   logging_negative_tests_today,true);
        DumpVector(indent+indent, "R_eff",                  logging_R_eff,               false);
    }

    if (comma_terminated)   m_log_file << indent << "},\n";
    else                    m_log_file << indent << "}\n";
};
