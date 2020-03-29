#include "../sars_cov2_sk/HelperFunctions.h"
#include "../sars_cov2_sk/RandomGeneratorPoisson.h"

#include <stdlib.h>
#include <cmath>

double sars_cov2_sk::RandomUniform()  {
    // Be carefull about the brackts, otherwise RAND_MAX will overflow
    return (double(rand())+1) / (double(RAND_MAX)+2);
}

unsigned int sars_cov2_sk::RandomPoisson(float mean_value)  {
    return RandomGeneratorPoisson::Poisson(mean_value);
}
