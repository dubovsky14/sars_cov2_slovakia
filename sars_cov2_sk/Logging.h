#ifndef LOGGING_H
#define LOGGING_H

#include "../sars_cov2_sk/PopulationCenter.h"

#include<string>
#include<iostream>
#include<fstream>

namespace sars_cov2_sk	{

    class PopulationCenter;

    class Logging  {
        private:
            std::string     m_log_file_address;
            std::ofstream   m_log_file;

            template <class T>
            void DumpVector(const std::string &indent, const std::string &variable_name, const std::vector<T> &numbers, bool comma_terminated)  {
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

            void DumpCityHistory(const sars_cov2_sk::PopulationCenter &city, bool comma_terminated);

        public:
            Logging(const std::string &out_adress);
            ~Logging();

            void DumpHistoryToJson(const std::vector<sars_cov2_sk::PopulationCenter> &cities);

            void DumpConfigToJson(bool comma_terminated = true);
    };
};
#endif