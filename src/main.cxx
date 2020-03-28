#include "../sars_cov2_sk/Person.h"
#include "../sars_cov2_sk/PopulationCenter.h"
#include "../sars_cov2_sk/HelperFunctions.h"


#include <iostream>
#include <vector>
#include <stdlib.h>

using namespace sars_cov2_sk;
using namespace std;


int main(int argc, char* argv[])    {
    Person::SetDay(0);

    vector<unsigned int> cities_number_of_citizens = {700000, 400000, 100000, 80000, 50000, 700000, 400000, 100000, 80000, 50000};
    int population_size = 0;
    for (unsigned int x : cities_number_of_citizens)    {
        population_size += x;
    }


    vector<Person> population;
    for (int i = 0; i < population_size; i++)   {
        if (i % 1000000 == 0)
            cout << i << "//" << population_size << endl;
        population.push_back(Person());
    }

    vector<PopolationCenter> cities;
    int current_person_index = 0;
    for (unsigned int i = 0; i < cities_number_of_citizens.size(); i++) {
        cities.push_back(PopolationCenter(&population, current_person_index, cities_number_of_citizens.at(i), 3.));
    }

    cout << "Number of households in cities:\n";
    for (const PopolationCenter &city : cities) {
        cout << "\t\t" << city.GetHouseholdVector()->size() << endl;
    }

    cout << "Population created\n";

    // Infect 10 random people
    for (int i = 0; i < 10; i++)    {
        const unsigned int index = 0.99* population_size * RandomUniform();
        population.at(index).Infect();
    }

    for (unsigned int day = 0; day < 100; day++)    {
        cout << "\tday " << day << endl;
        Person::SetDay(day);
        for (PopolationCenter &city : cities)   {
            city.SimulateDailySpread();
        }
    }



    cout << "Simulation finished\n";
    cin.get();
}

