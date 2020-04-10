#include "../sars_cov2_sk/Simulation.h"
#include "../cxxopts/cxxopts.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>

using namespace sars_cov2_sk;
using std::cout;
using std::endl;
using std::string;


int main(int argc, char* argv[]) {
    int verbosity = 0;
    std::string input_config;

    cxxopts::Options options("sars_cov2_sk",
                             "\nSARS-CoV-2 Simulation in Slovakia\n");
    options.add_options()
        ("v,verbosity", "verbosity level", cxxopts::value<int>(verbosity))
        ("c,configuration", "input configuration", cxxopts::value<std::string>(input_config));

    auto results = options.parse(argc, argv);

    if (input_config.empty()) {
        cout << "ERROR: Please specify address of the config file." << endl;
        cout << "       You have to run the code using the following command:" << endl;
        cout << "       bin/main -c <path to config>" << endl;

        exit(0);
    }

    try {
        RunSimulation(input_config);
        cout << "\nSimulation finished\n";
    }
    catch (string e)   {
        cout << e << endl;
    }
    catch (char const *e)   {
        cout << e << endl;
    }
}
