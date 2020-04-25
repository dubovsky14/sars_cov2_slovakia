
#ifndef ROUT_TO_REFF_CONVERTOR_H
#define ROUT_TO_REFF_CONVERTOR_H

namespace sars_cov2_sk	{
    // If there is N people in 0th generation and the disease is spread only in households (no random interactions), this functions returns
    // the ratio between generation "generation" and the 0th generation.
    float F(int generation);

    // Calculate the virus reproduction in whole population, if every infected person infects in average "R_out" others outside its household
    float Calculate_R_eff(float R_out);

    // Get effective R for a configuration given in config file
    float Get_R_eff();

    // Get effective number of people that an infected person infects outside its household in average
    float Get_R_out();
}

#endif