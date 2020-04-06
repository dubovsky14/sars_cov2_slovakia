
#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H

#include <vector>
#include <string>

namespace sars_cov2_sk	{
    // Uniform distribution from (0,1) interval. 0 and 1 never happens.
    double RandomUniform();

    unsigned int RandomPoisson(float mean_value);

    float RandomGauss(float mean, float sigma);

    // Generate Gauss first. If 4.8 is generated (for example), 5 will be returned with 80% probability and 4 with 20% probability
    int RandomGaussWithProbabilisticRounding(float mean, float sigma);

    float RandomExponential(float mean);

    int RandomExponentialWithProbabilisticRounding(float mean);

    template <class T>
    bool IsInVector(std::vector<T> input_vector, T element) {
        for (T &x : input_vector)   {
            if (x == element)   {
                return true;
            }
        }
        return false;
    }

    bool StringIsBool(const std::string &x);

    bool StringIsFloat(const std::string &x);
}

#endif