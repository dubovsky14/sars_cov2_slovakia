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

            // throw exception if singleton instance is nullptr
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

            // 0th element = number of elderly people living with youngs in an household
            // 1st element = number of elderly people living alone
            // 2nd element = number of elderly people living in pairs with another elderly person
            std::vector<float> m_elderly_houses;

            // Number of other people than elderly living in pairs, living in a household with given number of inhabitants
            std::vector<float> m_young_houses;

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

            // Opens file "json_address", looks for dictionary dic_name and read values for all keys in the same order
            static void ReadDictionaryFromJSON( const std::string &json_address, const std::string &dict_name,
                                                const std::vector<std::string> &keys, std::vector<float> *values);

            void ReadHouseholdFile();

            void ReadHouseholdsElderly();

            void ReadHouseholdsYoung();

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

            // Fraction of people in given age category. Sum of all elements == 1
            static const std::vector<float>    *GetAgeDistribution()    { Check(); return &s_singleton_instance->m_age_distribution; };


            static const std::vector<float>    *GetAgeSymptomatic()     { Check(); return &s_singleton_instance->m_age_symptomatic;};
            static const std::vector<float>    *GetAgeHospitalized()    { Check(); return &s_singleton_instance->m_age_hospitalized;};
            static const std::vector<float>    *GetAgeCritical()        { Check(); return &s_singleton_instance->m_age_critical;};
            static const std::vector<float>    *GetAgeFatal()           { Check(); return &s_singleton_instance->m_age_fatal;};

            // fraction of people living in non-elderly households with "index" number of inhabitants
            static const std::vector<float>    *GetYoungHouseholdsOccupancy()   { Check(); return &s_singleton_instance->m_young_houses;};

            static float GetElderlyFractionLivingWithYoungs()           { Check(); return s_singleton_instance->m_elderly_houses.at(0);};

            // 0th element = fraction of elderly people living with youngs in an household
            // 1st element = fraction of elderly people living alone
            // 2nd element = fraction of elderly people living in pairs with another elderly person
            static const std::vector<float>    *GetElderlyHouseholdsOccupancy() { Check(); return &s_singleton_instance->m_elderly_houses;};
    };
};
#endif
