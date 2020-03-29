#ifndef POISSON_GENERATOR_H
#define POISSON_GENERATOR_H

#include <random>

namespace sars_cov2_sk  {
    class RandomGeneratorPoisson    {
        private:
            RandomGeneratorPoisson(float mean_value);
            ~RandomGeneratorPoisson();

            std::poisson_distribution<int>      m_distribution;

            static RandomGeneratorPoisson *s_singletop_instance;

        public:
            static unsigned int Poisson(float mean_value);
            float m_mean_value;
            float m_lower_limit;
            float m_upper_limit;
    };
}

#endif