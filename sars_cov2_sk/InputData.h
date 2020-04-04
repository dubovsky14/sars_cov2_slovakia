#ifndef INPUT_DATA_H
#define INPUT_DATA_H

#include<vector>
#include<string>

namespace sars_cov2_sk	{
    // singleton
    class InputData  {
        private:
            InputData();

            static sars_cov2_sk::InputData *s_singleton_instance;

            static void Check();

            std::string                 m_text_file_municipalities;
            std::vector<unsigned int>   m_municipality_id;
            std::vector<unsigned int>   m_municipality_number_of_inhabitants;
            std::vector<unsigned int>   m_municipality_number_of_infected;
            std::vector<std::string>    m_municipality_name;

            std::vector< std::vector <unsigned int> > m_migrations;

            // fraction of poeple in the age group or younger
            std::vector<float>          m_age_distribution;

            unsigned int                m_number_of_municipalities;

            void ReadMunicipalityFile();

            void ReadLineOfConfig(std::string line);

            void ReadMigrations();

            void ReadLineOfMigrations(std::string line);

            void ReadNumberOfInfected();

            void ReadLineOfInfected(std::string line);

            void ReadAgeDistribution();

            void ReadLineOfAgeDistributuon(std::string line);

            static std::vector<std::string> SplitAndStripString(std::string input_string, const std::string &separator);

            static std::vector<std::string> SplitString(std::string input_string, const std::string &separator);

            static void StripString(std::string *input_string, const std::string &chars_to_remove = " \n\t\r");

            static bool ValidLine(std::string line);

        public:

            static void Initialize();

            // I know that this is not optimal, but we run it only once, so I don't care ...
            static std::vector<unsigned int>   GetMunicipIds()          { Check(); return s_singleton_instance->m_municipality_id;};
            static std::vector<unsigned int>   GetMunicipPopulations()  { Check(); return s_singleton_instance->m_municipality_number_of_inhabitants;};
            static std::vector<unsigned int>   GetMunicipInfected()     { Check(); return s_singleton_instance->m_municipality_number_of_infected;};
            static std::vector<std::string>    GetMunicipNames()        { Check(); return s_singleton_instance->m_municipality_name;};
            static const std::vector< std::vector <unsigned int> > *GetMigrations()   {Check(); return &s_singleton_instance->m_migrations;};
            static const std::vector<float>    *GetAgeDistribution()    { Check(); return &s_singleton_instance->m_age_distribution; };
    };
};
#endif