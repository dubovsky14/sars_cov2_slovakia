#ifndef RESTRICTIONS_H
#define RESTRICTIONS_H

#include<vector>
#include<string>
#include<map>

namespace sars_cov2_sk	{
    // singleton
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

            std::map<std::string, float> *ReadStringFloatDictionary(const std::string &json_file, const std::string &dict_name);

        public:

            static void Initialize(const std::string &resctrictions_file);

            static void Check();

            static void UpdateDay(int day);

            static float GetLimitMobility()                         {Check(); return s_singleton_instance->m_today_limit_mobility;};

            static float GetLimitElderlyStochasticInteractions()    {Check(); return s_singleton_instance->m_today_limit_elderly_stochastic_interactions;};

            static float GetLimitStochasticInteractions()           {Check(); return s_singleton_instance->m_today_limit_stochastic_interactions;};

    };
};
#endif
