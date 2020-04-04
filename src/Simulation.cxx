#include "../sars_cov2_sk/Simulation.h"

#include "../sars_cov2_sk/Person.h"
#include "../sars_cov2_sk/PopulationCenter.h"
#include "../sars_cov2_sk/HelperFunctions.h"
#include "../sars_cov2_sk/InputData.h"
#include "../sars_cov2_sk/Logging.h"
#include "../sars_cov2_sk/ConfigParser.h"
#include "../sars_cov2_sk/CovidTest.h"

#include <string>
#include <vector>
#include <iostream>

using namespace std;

void sars_cov2_sk::RunSimulation(const std::string &config_address)    {
    ConfigParser::InitializeConfig(config_address);

    InputData::Initialize();
    
    vector<unsigned int> cities_number_of_citizens = InputData::GetMunicipPopulations();
    vector<string> names = InputData::GetMunicipNames(); 

    vector<PopulationCenter> cities;
    vector<Person> population;

    PopulationCenter::CityAndPersonsFactory(cities_number_of_citizens, names, &population, &cities);
    PopulationCenter::SetMigrations(InputData::GetMigrations(), &cities);

    int population_size = 0;
    for (unsigned int x : cities_number_of_citizens)    {
        population_size += x;
    }

    cout << "Population created\n";

    if (ConfigParser::GetInfectedInitial() > 0) {
        int infected_already = 0;
        while (infected_already < ConfigParser::GetInfectedInitial())    {
            const unsigned int index = (population_size-1)*RandomUniform();
            if (population.at(index).IsIll())   {
                continue;
            }
            else {
                population.at(index).Infect();
                infected_already++;
            }
        }
    }
    else {
        vector <unsigned int> numbers_of_infected = InputData::GetMunicipInfected();
        for (unsigned int i_city = 0; i_city < cities.size(); i_city++) {
            cities.at(i_city).RandomlyInfectNInhabitants(numbers_of_infected.at(i_city));
        }
    }

    CovidTest genetic_test(0.98);


    cout << "Running the simulation.\n";
    cout << "Number of municipalities: " << cities.size()   << endl;
    cout << "Total population: "         << population_size << endl;
    cout << "day \t\t#infected people\t\tpos. tests\n";
    for (unsigned int day = 0; day < 1000; day++)    {
        Person::SetDay(day);
        const unsigned int number_of_ill = Person::GetNumberOfInfectedPersonsInPopulation(population);
        if (number_of_ill == 0) {
            break;
        }

        int positively_tested_today = 0;
        positively_tested_today +=  genetic_test.TestContactOfPositivesFromYesterday();
        // Testing
        if (day % 1 == 0)   {
            positively_tested_today += genetic_test.TestPeople(&population, 0.2);
        }
        genetic_test.PutToCarantinePositivelyTestedFromYesterday();

        cout << day << "\t\t" << number_of_ill << "\t\t" << positively_tested_today <<  endl;

        for (PopulationCenter &city : cities)   {
            city.SendTravelersToAllCities(&cities);
        }

        for (PopulationCenter &city : cities)   {
            city.SimulateDailySpread(ConfigParser::GetTransmissionProbHousehold(), ConfigParser::GetTransmissionProbCity());
        }
        for (PopulationCenter &city : cities)   {
            city.RemoveAllTemporaryOccupants();
        }
        
        for (PopulationCenter &city : cities)   {
            city.SaveTheDayToHistory();
        }

    }

    Logging logging(ConfigParser::GetResultFileAddress());
    logging.DumpHistoryToJson(cities);  
}
