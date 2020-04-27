#ifndef RANDOM_GENERATORs_H
#define RANDOM_GENERATORs_H

#include <random>

namespace sars_cov2_sk  {
    class RandomGeneratorPoisson    {
        private:
            RandomGeneratorPoisson(float mean_value);
            ~RandomGeneratorPoisson();

            std::poisson_distribution<int>      m_distribution;

            static RandomGeneratorPoisson *s_singleton_instance;

            float m_mean_value;
            float m_lower_limit;
            float m_upper_limit;

        public:
            static unsigned int Poisson(float mean_value);
    };

    class RandomGeneratorGauss    {
        private:
            RandomGeneratorGauss();
            ~RandomGeneratorGauss();

            std::normal_distribution<float>      m_distribution;

            static RandomGeneratorGauss *s_singleton_instance;

        public:
            static float Gauss(float mean, float sigma);

    };

    class RandomGeneratorNegativeBinomial    {
        private:
            RandomGeneratorNegativeBinomial(int number_of_failures, float probability_of_success);
            ~RandomGeneratorNegativeBinomial();

            std::negative_binomial_distribution<int>      m_distribution;

            static RandomGeneratorNegativeBinomial *s_singleton_instance;

            int   m_number_of_failures;
            float m_prob_mean_value;
            float m_prob_lower_limit;
            float m_prob_upper_limit;

        public:
            static float NegativeBinomial(int number_of_failures, float probability_of_success);

    };
}

#endif