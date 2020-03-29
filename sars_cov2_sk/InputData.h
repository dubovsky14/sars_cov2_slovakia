#ifndef INPUT_DATA_H
#define INPUT_DATA_H

#include<vector>
#include<string>

namespace sars_cov2_sk	{
    class InputData  {
        private:
            std::string                 m_text_file_municipalities;
            std::vector<unsigned int>   m_municipality_id;
            std::vector<unsigned int>   m_municipality_number_of_inhabitants;
            std::vector<unsigned int>   m_municipality_number_of_infected;
            std::vector<std::string>    m_municipality_name;

            std::vector< std::vector <unsigned int> > m_migrations;

            unsigned int                m_number_of_municipalities;

        public:
            InputData(const std::string &municipality_info_text_file);

            void ReadMunicipalityFile();

            void ReadLineOfConfig(std::string line);

            void ReadMigrations();

            void ReadLineOfMigrations(std::string line);

            static std::vector<std::string> SplitAndStripString(std::string input_string, const std::string &separator);

            static std::vector<std::string> SplitString(std::string input_string, const std::string &separator);

            static void StripString(std::string *input_string, const std::string &chars_to_remove = " \n\t\r");

            // I know that this is not optimal, but we run it only once, so I don't care ...
            std::vector<unsigned int>   GetMunicipIds() const           { return m_municipality_id;};
            std::vector<unsigned int>   GetMunicipPopulations() const   { return m_municipality_number_of_inhabitants;};
            std::vector<unsigned int>   GetMunicipInfected() const      { return m_municipality_number_of_infected;};
            std::vector<std::string>    GetMunicipNames() const         { return m_municipality_name;};
            const std::vector< std::vector <unsigned int> > *GetMigrations() const {return &m_migrations;};

            static bool ValidLine(std::string line);
    };
};
#endif