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
            std::vector<float>          m_municipality_longitude;
            std::vector<float>          m_municipality_latitude;

            std::vector< std::vector <unsigned int> > m_migrations;

            // fraction of poeple in the age group or younger
            std::vector<float>          m_age_distribution;

            std::vector<float>          m_age_symptomatic;
            std::vector<float>          m_age_hospitalized;
            std::vector<float>          m_age_critical;
            std::vector<float>          m_age_fatal;

            unsigned int                m_number_of_municipalities;

            void ReadMunicipalityFile();

            void ReadLineOfConfig(std::string line);

            void ReadMigrations();

            void ReadLineOfMigrations(std::string line);

            void ReadNumberOfInfected();

            void ReadLineOfInfected(std::string line);

            void ReadAgeDistribution();

            void ReadLineOfAgeDistributuon(std::string line);

            void ReadAgeSymptomsFile();

            std::vector<float> ReadAgeSymptomsProperty(const std::string &property);

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
            static std::vector<float>          GetMunicipLongitudes()   { Check(); return s_singleton_instance->m_municipality_longitude;};
            static std::vector<float>          GetMunicipLatitudes()    { Check(); return s_singleton_instance->m_municipality_latitude;};
            static const std::vector< std::vector <unsigned int> > *GetMigrations()   {Check(); return &s_singleton_instance->m_migrations;};
            static const std::vector<float>    *GetAgeDistribution()    { Check(); return &s_singleton_instance->m_age_distribution; };


            static const std::vector<float>    *GetAgeSymptomatic()     { Check(); return &s_singleton_instance->m_age_symptomatic;};
            static const std::vector<float>    *GetAgeHospitalized()    { Check(); return &s_singleton_instance->m_age_hospitalized;};
            static const std::vector<float>    *GetAgeCritical()        { Check(); return &s_singleton_instance->m_age_critical;};
            static const std::vector<float>    *GetAgeFatal()           { Check(); return &s_singleton_instance->m_age_fatal;};
    };
};
#endif
