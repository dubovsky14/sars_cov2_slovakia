
#ifndef ROUT_TO_REFF_CONVERTOR_H
#define ROUT_TO_REFF_CONVERTOR_H

namespace sars_cov2_sk	{
    float F(int generation);

    float Calculate_R_eff(float R_out);

    float Get_R_eff();

    float Get_R_out();
}

#endif