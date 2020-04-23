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
    const float gauss = RandomGauss(mean, sigma);
    return RoundProbabilistic(gauss);
}

float sars_cov2_sk::RandomExponential(float mean)   {
    return -mean*log(RandomUniform());
}

int sars_cov2_sk::RandomExponentialWithProbabilisticRounding(float mean)    {
    const float exponential = RandomExponential(mean);
    return RoundProbabilistic(exponential);
}

int sars_cov2_sk::RoundProbabilistic(float value) {
    const float decimal_part = value - int(value);
    if (RandomUniform() < decimal_part) {
        return 1 + int(value);
    }
    else    {
        return int(value);
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


vector<string> sars_cov2_sk::SplitAndStripString(string input_string, const string &separator) {
    vector<string> result = SplitString(input_string, separator);
    for (string &x : result)    {
        StripString(&x, " \n\t\r");
    }
    return move(result);
}

vector<string> sars_cov2_sk::SplitString(string input_string, const string &separator)    {
    vector<string> result;
    size_t pos = 0;
    std::string token;
    while ((pos = input_string.find(separator)) != std::string::npos) {
        result.push_back(input_string.substr(0, pos));
        input_string.erase(0, pos + separator.length());
    }
    if (input_string.length() > 0) result.push_back(input_string);
    return move(result);
};

void sars_cov2_sk::StripString(string *input_string, const string &chars_to_remove)    {
    input_string->erase(0,input_string->find_first_not_of(chars_to_remove));
    input_string->erase(input_string->find_last_not_of(chars_to_remove)+1);
}
