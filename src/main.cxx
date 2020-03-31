#include "../sars_cov2_sk/Simulation.h"

#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>

using namespace sars_cov2_sk;
using namespace std;


int main(int argc, char* argv[])    {
    try {
        if (argc == 2)  {
            const string input_config = argv[1];
            RunSimulation(input_config);
        }
        else {
            cout << "Please specify address of the config file. You have to run the code using the following command" << endl;
            cout << "./bin/main \"<path to config>\"" << endl;
        }
        
        cout << "\nSimulation finished\n";
    }
    catch (string e)   {
        cout << e << endl;
    }
}

