
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
    bool IsInVector(const std::vector<T> &input_vector, T element) {
        for (unsigned int i = 0; i < input_vector.size(); i++)   {
            if (input_vector.at(i) == element)   {
                return true;
            }
        }
        return false;
    }

    // return maximal value in vector
    template <class T>
    T MaxInVector(const std::vector<T> &input_vector) {
        if (input_vector.size() == 0)   {
            throw "Cannot find maximal value of empty vector!";
        }

        T maximum = input_vector.at(0);

        for (unsigned int i = 0; i < input_vector.size(); i++)   {
            if (input_vector.at(i) > maximum)   {
                maximum = input_vector.at(i);
            }
        }
        return maximum;
    }

    bool StringIsBool(const std::string &x);

    bool StringIsFloat(const std::string &x);
}

#endif