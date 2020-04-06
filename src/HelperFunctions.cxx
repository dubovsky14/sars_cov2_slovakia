#include "../sars_cov2_sk/HelperFunctions.h"
#include "../sars_cov2_sk/RandomGenerators.h"

#include <stdlib.h>
#include <cmath>
#include <string>

using namespace std;

double sars_cov2_sk::RandomUniform()  {
    // Be carefull about the brackts, otherwise RAND_MAX will overflow
    return (double(rand())+1) / (double(RAND_MAX)+2);
}

unsigned int sars_cov2_sk::RandomPoisson(float mean_value)  {
    return sars_cov2_sk::RandomGeneratorPoisson::Poisson(mean_value);
}

float sars_cov2_sk::RandomGauss(float mean,float sigma)  {
    return sars_cov2_sk::RandomGeneratorGauss::Gauss(mean, sigma);
}

int sars_cov2_sk::RandomGaussWithProbabilisticRounding(float mean, float sigma) {
    const double gauss = RandomGauss(mean, sigma);
    const double decimal_part = gauss - int(gauss);
    if (RandomUniform() < decimal_part) {
        return 1 + int(gauss);
    }
    else    {
        return int(gauss);
    }
}

float sars_cov2_sk::RandomExponential(float mean)   {
    return -mean*log(RandomUniform());
}

int sars_cov2_sk::RandomExponentialWithProbabilisticRounding(float mean)    {
    const double exponential = RandomExponential(mean);
    const double decimal_part = exponential - int(exponential);
    if (RandomUniform() < decimal_part) {
        return 1 + int(exponential);
    }
    else    {
        return int(exponential);
    }
}

bool sars_cov2_sk::StringIsBool(const string &x)    {
    try {
        std::stoi(x);
    }
    catch (...) {
        return false;
    }
    return true;
};

bool sars_cov2_sk::StringIsFloat(const string &x)   {
    try {
        std::stod(x);
    }
    catch (...) {
        return false;
    }
    return true;
};