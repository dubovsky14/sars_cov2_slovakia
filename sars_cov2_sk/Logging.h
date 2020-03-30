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

            void DumpVector(const std::string &indent, const std::string &variable_name, const std::vector<unsigned int> &numbers, bool comma_terminated);
            
        public:
            Logging(const std::string &out_adress);
            ~Logging();

            void DumpCityHistory(const sars_cov2_sk::PopulationCenter &city);
    };
};
#endif