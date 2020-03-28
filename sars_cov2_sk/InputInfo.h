#ifndef INPUT_INFO_H
#define INPUT_INFO_H

#include<vector>
#include<string>

namespace sars_cov2_sk	{
    class InputInfo  {
        private:
            std::string                 m_text_file_municipalities;
            std::vector<int>            m_municipality_id;
            std::vector<int>            m_municipality_number_of_inhabitants;
            std::vector<int>            m_municipality_number_of_infected;
            std::vector<std::string>    m_municipality_name;

        public:
            InputInfo(const std::string &municipality_info_text_file);

            void ReadMunicipalityFile();

            static std::vector<std::string> SplitAndStripString(std::string input_string, const std::string &separator);

            static std::vector<std::string> SplitString(std::string input_string, const std::string &separator);

            static void StripString(std::string *input_string, const std::string &chars_to_remove = " \n\t\r");
    };
};
#endif