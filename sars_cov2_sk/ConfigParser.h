#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include "../sars_cov2_sk/Person.h"

#include <string>
#include <vector>
#include <map>

namespace sars_cov2_sk  {
    class Logging;

    enum TrackingOption {disabled, all, infected_only};
    // singleton
    class ConfigParser  {
        private:
            ConfigParser(const std::string &config_address);
            ~ConfigParser();

            static sars_cov2_sk::ConfigParser *s_singleton_instance;


            float m_transmission_prob_household;
            float m_transmission_prob_city;
            float m_average_number_of_city_interactions;
            float m_probability_to_remember_city_interactions;

            int   m_infected_initial;

            // true = number of travellers meetings is the same as for other people, they are just distributed among cities (travelling does not increase the number of total meetings)
            // false = traveller will have in each visited city the same number of meetings as normal person (travelling increases the number of total meetings)
            bool  m_travellers_meetings_constant;
            float m_mobility;

            sars_cov2_sk::TrackingOption m_tracking_option;

            std::string m_migration_matrix_address;
            std::string m_municipalities_file;
            std::string m_result_file;
            std::string m_infected_file;

            std::map<std::string, std::string> m_map;   // Note for python developers -> this is the same as dictionary in python

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

            void InitializeTravellersMeetingsConstant();

            seir_status m_initial_seir_status;
            void InitializeInitialSEIRStatus();

            // Throw exception if config is not initialized
            static void Check();


            // Disease evolution parameters

            // Time from exposure to the virus until the person starts to infectious
            float m_infectious_start_mean;
            float m_infectious_start_std;

            // Length of being infectious
            float m_infectious_days_mean;
            float m_infectious_days_std;

            // Time from the first symptoms until hospitalization is needed
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

            friend class sars_cov2_sk::Logging;
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

            // true = number of traveller's meetings is the same as for other people, they are just distributed among cities (travelling does not increase the number of total meetings)
            // false = traveller will have in each visited city the same number of meetings as normal person (travelling increases the number of total meetings)
            static bool  TravellerMeetingsConstant()          {Check(); return s_singleton_instance->m_travellers_meetings_constant;};

            // Fraction of people travelling in current state (with respect to the normal state)
            static float GetMobility()                        {Check(); return s_singleton_instance->m_mobility;}

            static seir_status  GetInitialSeirStatus()        {Check(); return s_singleton_instance->m_initial_seir_status;};

            static sars_cov2_sk::TrackingOption GetTrackingOption()   {Check(); return s_singleton_instance->m_tracking_option;};

            static std::string GetRestrictionsFile();

            // If you need to access it only once, this is easier way (but more CPU intensive ...)
            static std::string ReadStringValue(const std::string &key) {Check(); return s_singleton_instance->GetStringValue(key);};;
            static int         ReadIntValue   (const std::string &key) {Check(); return s_singleton_instance->GetIntValue(key);};;
            static float       ReadFloatValue (const std::string &key) {Check(); return s_singleton_instance->GetFloatValue(key);};;
    };
}

#endif