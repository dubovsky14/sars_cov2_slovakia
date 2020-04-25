#include "../sars_cov2_sk/ROutToReffConvertor.h"

#include "../sars_cov2_sk/ConfigParser.h"
#include "../sars_cov2_sk/InputData.h"

#include <cmath>
#include <vector>

using namespace std;

float sars_cov2_sk::F(int generation)   {
    if (generation == 0) {
        return 1.;
    }
    if (generation == 1) {
        return 0.7058;
    }
    if (generation == 2) {
        return 0.28423;
    }
    if (generation == 3) {
        return 0.06665;
    }
    if (generation == 4) {
        return 0.00891;
    }
    if (generation == 5) {
        return 0.00062;
    }
    if (generation == 6) {
        return 0.00001;
    }
    return 0;
};

float sars_cov2_sk::Calculate_R_eff(float R_out)    {
    const float    result = ( pow(R_out, 7)
                            + 7*F(1)*pow(R_out,6)
                            + (6*F(2) + 15*F(1)*F(1))*pow(R_out,5)
                            + (5*F(3) + 20*F(2)*F(1) + 10*pow(F(1),3))*pow(R_out,4)
                            + (4*F(4) + 12*F(3)*F(1) + 6*pow(F(2),2) + 12*F(2)*pow(F(1),2) + pow(F(1),4))*pow(R_out,3)
                            + (3*F(5) + 6*F(4)*F(1) + 6*F(3)*F(2) + 3*F(1)*pow(F(2),2) + 3*F(3)*pow(F(1),2))*pow(R_out,2)
                            + (2*F(6) + 2*F(5)*F(1) + 2*F(4)*F(2) + pow(F(3),2))*R_out
                            + F(7) );
    return pow(result,(1./7.));
};

float sars_cov2_sk::Get_R_eff()   {
    return Calculate_R_eff(Get_R_out());
}

float sars_cov2_sk::Get_R_out()   {
    const float T_inf_non_hospitalized  = ConfigParser::InfectiousDaysMean();
    const float T_inf_hospitalized      = ConfigParser::HospitalizationStartMean();

    float hospitalized(0), people_total(0);
    const vector<float> *age_hospitalization = InputData::GetAgeHospitalized();
    const vector<float> *age_symptomatic     = InputData::GetAgeSymptomatic();
    const vector<float> *age_distribution    = InputData::GetAgeDistribution();

    for (unsigned int i = 0; i < age_distribution->size(); i++) {
        hospitalized += age_distribution->at(i)*age_symptomatic->at(i)*age_hospitalization->at(i);
        people_total += age_distribution->at(i);
    }
    hospitalized /= people_total;

    const float T_inf_average           = hospitalized*T_inf_hospitalized + (1.-hospitalized)*T_inf_non_hospitalized;
    const float n_random_interactions   = ConfigParser::GetAverageNumberOfCityInteractions();
    const float transmission_prob       = ConfigParser::GetTransmissionProbCity();
    const float migrations_total        = InputData::GetMigrationsTotal();
    const bool  meetings_additional     = !ConfigParser::TravellerMeetingsConstant();
    vector<unsigned int> cities_population = InputData::GetMunicipPopulations();

    float population_total(0.);
    for (unsigned int city_population:cities_population)    {
        population_total += city_population;
    }

    const float R_out = (n_random_interactions + meetings_additional*migrations_total*ConfigParser::GetMobility()/population_total)*T_inf_average*transmission_prob;
    return R_out;
}
