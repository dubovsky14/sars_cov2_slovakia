#include "../sars_cov2_sk/HelperFunctions.h"

#include <stdlib.h>

double sars_cov2_sk::RandomUniform()  {
    // Be carefull about the brackts, otherwise RAND_MAX will overflow
    return (double(rand())+1) / (double(RAND_MAX)+2);
}
