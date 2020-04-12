
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

    // Takes care of a rounding. If value = 4.8 (for example), 4 will be returned in 20% of cases and 5 in 80% of cases.
    // So if we cound a mean value of rounded 4.8 in a loop, the result will be 4.8 and not 5 as in case of normal rounding
    int RoundProbabilistic(float value);

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