#include "../sars_cov2_sk/RandomGenerators.h"

#include <random>

using namespace std;
using namespace sars_cov2_sk;

static std::default_random_engine   s_generator;

RandomGeneratorPoisson* RandomGeneratorPoisson::s_singleton_instance = nullptr;

RandomGeneratorPoisson::RandomGeneratorPoisson(float mean_value)    {
    delete s_singleton_instance;
    s_singleton_instance = nullptr;
    m_mean_value    = mean_value;
    m_lower_limit   = 0.999*mean_value;
    m_upper_limit   = 1.001*mean_value;

    m_distribution = poisson_distribution<int>(mean_value);

    s_singleton_instance = this;
};

RandomGeneratorPoisson::~RandomGeneratorPoisson()   {

};

unsigned int RandomGeneratorPoisson::Poisson(float mean_value)  {
    // If the generator is not yet initialized
    if (s_singleton_instance == nullptr) {
        s_singleton_instance = new RandomGeneratorPoisson(mean_value);
    }

    if ((mean_value < s_singleton_instance->m_upper_limit) && (mean_value > s_singleton_instance->m_lower_limit))   {
        return s_singleton_instance->m_distribution(s_generator);
    }
    else {
        s_singleton_instance = new RandomGeneratorPoisson(mean_value);
        return s_singleton_instance->m_distribution(s_generator);
    }
};

RandomGeneratorGauss* RandomGeneratorGauss::s_singleton_instance = nullptr;

RandomGeneratorGauss::RandomGeneratorGauss()    {
    m_distribution = normal_distribution<float>(0, 1);
    s_singleton_instance = this;
};

RandomGeneratorGauss::~RandomGeneratorGauss()   {
    delete s_singleton_instance;
    s_singleton_instance = nullptr;
};

float RandomGeneratorGauss::Gauss(float mean, float sigma)  {
    // If the generator is not yet initialized
    if (s_singleton_instance == nullptr) {
        s_singleton_instance = new RandomGeneratorGauss();
    }

    return sigma*s_singleton_instance->m_distribution(s_generator) + mean;
};

RandomGeneratorNegativeBinomial* RandomGeneratorNegativeBinomial::s_singleton_instance = nullptr;

RandomGeneratorNegativeBinomial::RandomGeneratorNegativeBinomial(int number_of_failures, float probability_of_success)    {
    m_number_of_failures    = number_of_failures;
    m_prob_mean_value       = probability_of_success;
    m_prob_lower_limit      = 0.9999*probability_of_success;
    m_prob_upper_limit      = 1.0001*probability_of_success;

    m_distribution = negative_binomial_distribution<int>(number_of_failures, probability_of_success);
    s_singleton_instance = this;
};

RandomGeneratorNegativeBinomial::~RandomGeneratorNegativeBinomial()   {

};

float RandomGeneratorNegativeBinomial::NegativeBinomial(int number_of_failures, float probability_of_success)  {
    delete s_singleton_instance;
    s_singleton_instance = nullptr;

    // If the generator is not yet initialized
    if (s_singleton_instance == nullptr) {
        s_singleton_instance = new RandomGeneratorNegativeBinomial(number_of_failures, probability_of_success);
    }

    return s_singleton_instance->m_distribution(s_generator);

    if ((probability_of_success < s_singleton_instance->m_prob_upper_limit) &&
        (probability_of_success > s_singleton_instance->m_prob_lower_limit) &&
        (number_of_failures    == s_singleton_instance->m_number_of_failures))   {
            return s_singleton_instance->m_distribution(s_generator);
    }
    else {
        s_singleton_instance = new RandomGeneratorNegativeBinomial(number_of_failures, probability_of_success);
        return s_singleton_instance->m_distribution(s_generator);
    }
};