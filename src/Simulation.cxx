#include "../sars_cov2_sk/Simulation.h"

#include "../sars_cov2_sk/Person.h"
#include "../sars_cov2_sk/PopulationCenter.h"
#include "../sars_cov2_sk/HelperFunctions.h"
#include "../sars_cov2_sk/InputData.h"
#include "../sars_cov2_sk/Logging.h"
#include "../sars_cov2_sk/ConfigParser.h"


#include <string>
#include <vector>
#include <iostream>

using namespace std;

void sars_cov2_sk::RunSimulation(const std::string &config_address)    {
    ConfigParser::InitializeConfig(config_address);

    InputData input_info(ConfigParser::GetMunicipalitiesFileAddress());
    vector<unsigned int> cities_number_of_citizens = input_info.GetMunicipPopulations();
    vector<string> names = input_info.GetMunicipNames(); 

    vector<PopulationCenter> cities;
    vector<Person> population;

    PopulationCenter::CityAndPersonsFactory(cities_number_of_citizens, names, &population, &cities);
    PopulationCenter::SetMigrations(input_info.GetMigrations(), &cities);

    int population_size = 0;
    for (unsigned int x : cities_number_of_citizens)    {
        population_size += x;
    }

    cout << "Population created\n";

    // Infect 10 random people
    for (int i = 0; i < 10; i++)    {
        const unsigned int index = 0.99* population_size * RandomUniform();
        population.at(index).Infect();
    }

    cout << "Running the simulation.\n";
    cout << "Number of municipalities: " << cities.size()   << endl;
    cout << "Total population: "         << population_size << endl;
    cout << "day \t\t#infected people\n";
    for (unsigned int day = 0; day < 1000; day++)    {
        Person::SetDay(day);
        const unsigned int number_of_ill = Person::GetNumberOfInfectedPersonsInPopulation(population);
        cout << day << "\t\t" << number_of_ill <<  endl;
        if (number_of_ill == 0) {
            break;
        }

        for (PopulationCenter &city : cities)   {
            city.SendTravelersToAllCities(&cities);
        }

        for (PopulationCenter &city : cities)   {
            city.SimulateDailySpread(ConfigParser::GetTransmissionProbHousehold(), ConfigParser::GetTransmissionProbCity());
        }
        for (PopulationCenter &city : cities)   {
            city.RemoveAllTemporaryOccupants();
            city.SaveTheDayToHistory();
        }
    }

    Logging logging("results/results.json");
    logging.DumpHistoryToJson(cities);  
}
