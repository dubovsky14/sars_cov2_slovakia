#ifndef RESTRICTIONS_H
#define RESTRICTIONS_H

#include "../sars_cov2_sk/Person.h"

#include<vector>
#include<string>
#include<map>

namespace sars_cov2_sk	{
    // singleton
    enum SmartResctrictionsType {   enum_smart_restrictions_none,
                                    enum_smart_restrictions_critical,
                                    enum_smart_restrictions_positive_tests,
                                    enum_smart_restrictions_hospitalized_all};
    class Restrictions  {
        private:
            Restrictions(const std::string &resctrictions_file);

            static sars_cov2_sk::Restrictions *s_singleton_instance;

            float m_today_limit_mobility;
            float m_today_limit_elderly_stochastic_interactions;
            float m_today_limit_stochastic_interactions;

            std::vector<float> m_limit_mobility;
            std::vector<float> m_limit_elderly_stochastic_interactions;
            std::vector<float> m_limit_stochastic_interactions;

            // smart restrictions
            SmartResctrictionsType  m_smart_restrictions_type;
            bool                    m_smart_restrictions_active;
            int                     m_trigger_count_on;
            int                     m_trigger_count_off;
            float                   m_smart_limit_mobility;
            float                   m_smart_limit_elderly_stochastic_interactions;
            float                   m_smart_limit_stochastic_interactions;


            const std::vector<sars_cov2_sk::Person> *m_population;

            std::map<std::string, float> *ReadStringFloatDictionary(const std::string &json_file, const std::string &dict_name);

            void InitializeInteligentRestrictions(const std::string &resctrictions_file);

            void SetValuesForInteligentRestrictions(std::map<std::string, float> *restrictions_dictionary);

            // it counts the number of people in the state that triggers the smart restrictions
            // if the number of people is hight enough, it put restrictions in place
            // if the restrictions are in place and the number of people in the given state descreases bellow the given threshold, it cancels the restrictions
            void CheckNeedForSmartRestrictions();

            static void Check();

        public:

            static void Initialize();

            // Set pointer to vector of persons representing the whole country population. It will be used for smart restrictions
            static void SetPopulationPointer(const std::vector<sars_cov2_sk::Person> *population)   {Check(); s_singleton_instance->m_population = population;};

            // Update resctrictions based on day and in case of smart restrictions count number of people in the state triggering the smart restrictions
            // and (de)activate smart restrictions accordingly
            static void UpdateDay(int day);

            // Get multiplicative factor by which the mobility is decreased
            static float GetLimitMobility()                         {Check(); return s_singleton_instance->m_today_limit_mobility;};

            // Get multiplicative factor by which the interactions of elderly people are decreased
            // This is additional factor to "GetLimitStochasticInteractions()", so if both of them are 0.5, the elderly will have 0.5^2 = 0.25 of the normal interactions
            static float GetLimitElderlyStochasticInteractions()    {Check(); return s_singleton_instance->m_today_limit_elderly_stochastic_interactions;};

            // Get multiplicative factor by which the random interactions in city are decreased
            static float GetLimitStochasticInteractions()           {Check(); return s_singleton_instance->m_today_limit_stochastic_interactions;};

    };
};
#endif
