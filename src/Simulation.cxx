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

    // Retrieve input data about municipalities
    vector<unsigned int> cities_number_of_citizens = InputData::GetMunicipPopulations();
    vector<string> names = InputData::GetMunicipNames();
    vector<float> longitudes = InputData::GetMunicipLongitudes();
    vector<float> latitudes = InputData::GetMunicipLatitudes();

    vector<PopulationCenter> cities;
    vector<Person> population;

    PopulationCenter::CityAndPersonsFactory(cities_number_of_citizens, names,
                                            longitudes, latitudes,
                                            &population, &cities);
    PopulationCenter::SetMigrations(InputData::GetMigrations(), &cities);

    int population_size = 0;
    for (unsigned int x : cities_number_of_citizens)    {
        population_size += x;
    }

    cout << "Population created\n";

    // Infect randomly number of people defined in config
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
    else { // Infect randomly number of people in municipalities from the corresponding text file
        vector <unsigned int> numbers_of_infected = InputData::GetMunicipInfected();
        for (unsigned int i_city = 0; i_city < cities.size(); i_city++) {
            cities.at(i_city).RandomlyInfectNInhabitants(numbers_of_infected.at(i_city));
        }
    }

    CovidTest genetic_test(ConfigParser::ReadFloatValue("genetic_tests_accuracy"));
    genetic_test.SetNumberOfAvailableTestsDaily(ConfigParser::ReadIntValue("genetic_tests_number_of_kits_daily"));

    cout << "Running the simulation.\n";
    cout << "Number of municipalities: " << cities.size()   << endl;
    cout << "Total population: "         << population_size << endl;
    cout << "day \t\t#infected\texposed\t\tasymptomatic\tsymptomatic\thosp.\t\tcritical\tdead\t\timmune\t\ttest_random\ttest_contacts" << endl;
    for (unsigned int day = 0; day < 1000; day++)    {
        Person::SetDay(day);
        const unsigned int number_of_ill = Person::GetNumberOfInfectedPersonsInPopulation(population);
        if (number_of_ill == 0) {
            break;
        }
        cout << day << "\t\t" << number_of_ill
                    << "\t\t" << Person::CountInPopulation(population, enum_exposed)
                    << "\t\t" << Person::CountInPopulation(population, enum_infective_asymptomatic)
                    << "\t\t" << Person::CountInPopulation(population, enum_infective_symptomatic)
                    << "\t\t" << Person::CountInPopulation(population, enum_needs_hospitalization)
                    << "\t\t" << Person::CountInPopulation(population, enum_critical)
                    << "\t\t" << Person::CountInPopulation(population, enum_dead)
                    << "\t\t" << Person::CountInPopulation(population, enum_immune);

        for (PopulationCenter &city : cities)   {
            city.SendTravelersToAllCities(&cities);
        }

        for (PopulationCenter &city : cities)   {
            city.SimulateDailySpread(ConfigParser::GetTransmissionProbHousehold(), ConfigParser::GetTransmissionProbCity());
        }
        for (PopulationCenter &city : cities)   {
            city.RemoveAllTemporaryOccupants();
        }

        // Testing
        const int positively_tested_1st_round  = genetic_test.TestPeople(&population, ConfigParser::ReadFloatValue("genetic_tests_rate"));
        const int positively_tested_contacts =  genetic_test.TestContactOfPositivesFromYesterday();
        genetic_test.PutToCarantinePositivelyTestedFromYesterday();
        cout    << "\t\t" << positively_tested_1st_round
                << "\t\t" << positively_tested_contacts;


        for (PopulationCenter &city : cities)   {
            city.SaveTheDayToHistory();
        }

        cout << endl;
    }

    Logging logging(ConfigParser::GetResultFileAddress());
    logging.DumpConfigToJson(true);
    logging.DumpHistoryToJson(cities);
}
