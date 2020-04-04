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