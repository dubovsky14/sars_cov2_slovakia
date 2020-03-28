#include "../sars_cov2_sk/Person.h"
#include "../sars_cov2_sk/PopulationCenter.h"
#include "../sars_cov2_sk/HelperFunctions.h"


#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>

using namespace sars_cov2_sk;
using namespace std;


int main(int argc, char* argv[])    {
    Person::SetDay(0);

    vector<unsigned int> cities_number_of_citizens = {700000, 400000, 100000, 80000, 50000, 700000, 400000, 100000, 80000, 50000};
    vector<string> names;
    for (unsigned int i = 0; i < cities_number_of_citizens.size(); i++)    {
        names.push_back("City_" + std::to_string(i));
    }
    vector<PopulationCenter> cities;
    vector<Person> population;

    PopulationCenter::CityAndPersonsFactory(cities_number_of_citizens, names, &population, &cities);

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
    cout << "day \t\t#infected people\n";
    for (unsigned int day = 0; day < 1000; day++)    {
        Person::SetDay(day);
        const unsigned int number_of_ill = Person::GetNumberOfInfectedPersonsInPopulation(population);
        cout << day << "\t\t" << number_of_ill <<  endl;
        if (number_of_ill == 0) {
            break;
        }

        for (PopulationCenter &city : cities)   {
            city.SimulateDailySpread();
            //cout << "\t\t" << city.GetName() << "\t\t" << Person::GetNumberOfInfectedPersonsInPopulation(*city.GetInhabitants()) << endl;
        }
    }

    cout << "Simulation finished\n";
    cin.get();
}

