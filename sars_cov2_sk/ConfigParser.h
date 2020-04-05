#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include <string>
#include <vector>
#include <map>

namespace sars_cov2_sk  {
    enum TrackingOption {disabled, all, infected_only};
    // singleton
    class ConfigParser  {
        private:
            ConfigParser(const std::string &config_address);
            ~ConfigParser();

            float m_transmission_prob_household;
            float m_transmission_prob_city;
            float m_average_number_of_city_interactions;
            float m_probability_to_remember_city_interactions;

            int   m_infected_initial;


            sars_cov2_sk::TrackingOption m_tracking_option;

            std::string m_migration_matrix_address;
            std::string m_municipalities_file;
            std::string m_result_file;
            std::string m_infected_file;

            std::map<std::string, std::string> m_map;   // Note for python developers -> this is the same as dictionary in python

            static sars_cov2_sk::ConfigParser *s_singleton_instance;

            void ReadConfig();

            void ReadLineOfConfig(std::string line);

            std::string GetStringValue(const std::string &key);
            int         GetIntValue   (const std::string &key);
            float       GetFloatValue (const std::string &key);
            void        SetTrackingOption();

            static std::vector<std::string> SplitAndStripString(std::string input_string, const std::string &separator);
            static std::vector<std::string> SplitString(std::string input_string, const std::string &separator);
            static void StripString(std::string *input_string, const std::string &chars_to_remove = " \n\t\r");
            static bool ValidLine(std::string line);
            static bool RemoveCommentFromLine(std::string *line);

            void InitializeValues();

            // Throw exception if config is not initialized
            static void Check();


            // Disease evolution parameters

            // Time from exposure to the virus until the person starts to infectious
            float m_infectious_start_mean;
            float m_infectious_start_std;

            // Length of being infectious
            float m_infectious_days_mean;
            float m_infectious_days_std;

            // Time from being exposed to virus until hospitalization is needed
            float m_hospitalization_start_mean;
            float m_hospitalization_start_std;

            // Length of hospitalization for people who are going to recover without a need for critical care
            float m_hospitalization_length_mean;
            float m_hospitalization_length_std;

            // Time from being hospitalized until the person needs critical care (if he gonna need it)
            float m_critical_care_start_mean;
            float m_critical_care_start_std;

            // Time from being hospitalized until the person recovers if it's in critical state (if does not die ...)
            float m_critical_care_length_mean;
            float m_critical_care_length_std;

        public:
            static void InitializeConfig(const std::string &config_address);

            // The lines bellow could be replaced by one Get function accessing a dictionary
            // But it would be slower and we need to call it few milion times per simulated day.

            static float GetTransmissionProbHousehold()         {Check(); return s_singleton_instance->m_transmission_prob_household;};
            static float GetTransmissionProbCity()              {Check(); return s_singleton_instance->m_transmission_prob_city;};
            static float GetAverageNumberOfCityInteractions()   {Check(); return s_singleton_instance->m_average_number_of_city_interactions;};
            static float GetProbToRememberCityInteractions()    {Check(); return s_singleton_instance->m_probability_to_remember_city_interactions;};


            static float InfectiousStartMean()               {Check(); return s_singleton_instance->m_infectious_start_mean;};
            static float InfectiousStartStd()                {Check(); return s_singleton_instance->m_infectious_start_std;};
            static float InfectiousDaysMean()                {Check(); return s_singleton_instance->m_infectious_days_mean;};
            static float InfectiousDaysStd()                 {Check(); return s_singleton_instance->m_infectious_days_std;};
            static float HospitalizationStartMean()          {Check(); return s_singleton_instance->m_hospitalization_start_mean;};
            static float HospitalizationStartStd()           {Check(); return s_singleton_instance->m_hospitalization_start_std;};
            static float HospitalizationLengthMean()         {Check(); return s_singleton_instance->m_hospitalization_length_mean;};
            static float HospitalizationLengthStd()          {Check(); return s_singleton_instance->m_hospitalization_length_std;};
            static float CriticalStartMean()                 {Check(); return s_singleton_instance->m_critical_care_start_mean;};
            static float CriticalStartStd()                  {Check(); return s_singleton_instance->m_critical_care_start_std;};
            static float CriticalLengthMean()                {Check(); return s_singleton_instance->m_critical_care_length_mean;};
            static float CriticalLengthStd()                 {Check(); return s_singleton_instance->m_critical_care_length_std;};

            static std::string GetMigrationMatrixAddress()    {Check(); return s_singleton_instance->m_migration_matrix_address;};
            static std::string GetMunicipalitiesFileAddress() {Check(); return s_singleton_instance->m_municipalities_file;};
            static std::string GetResultFileAddress()         {Check(); return s_singleton_instance->m_result_file;};
            static std::string GetInfectedFileAddress()       {Check(); return s_singleton_instance->m_infected_file;};
            static int GetInfectedInitial()                   {Check(); return s_singleton_instance->m_infected_initial;};

            static sars_cov2_sk::TrackingOption GetTrackingOption()   {Check(); return s_singleton_instance->m_tracking_option;};

            // If you need to access it only once, this is easier way (but more CPU intensive ...)
            static std::string ReadStringValue(const std::string &key) {Check(); return s_singleton_instance->GetStringValue(key);};;
            static int         ReadIntValue   (const std::string &key) {Check(); return s_singleton_instance->GetIntValue(key);};;
            static float       ReadFloatValue (const std::string &key) {Check(); return s_singleton_instance->GetFloatValue(key);};;

    };
}

#endif