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

        public:
            static unsigned int Poisson(float mean_value);
            float m_mean_value;
            float m_lower_limit;
            float m_upper_limit;
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
}

#endif